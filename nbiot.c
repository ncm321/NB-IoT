/*********************************************************************************
 *      Copyright:  (C) 2024 LingYun IoT System Studio
 *                  All rights reserved.
 *
 *       Filename:  nbiot.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(17/07/24)
 *         Author:  Niu Chunmin <niuchunmin@2430815397@qq.com>
 *      ChangeLog:  1, Release initial version on "17/07/24 10:46:46"
 *                 
 ********************************************************************************/

#include "nbiot.h"

static inline void msleep(unsigned long ms)
{
	struct timespec cSleep;
	unsigned long   ulTmp;

	cSleep.tv_sec = ms / 1000;
	if( cSleep.tv_sec == 0)
	{
		ulTmp = ms * 10000;
		cSleep.tv_nsec = ulTmp * 100;
	}
	else
	{
		cSleep.tv_nsec = 0;
	}

	nanosleep(&cSleep, 0);
}

void *state_machine_thread(void *arg)
{
	nb_config_t*     nbiot_data = (nb_config_t*)arg;
	int              rv;
	nbiot_data->current_state = STATUS_INIT;


	while(1)
	{
		pthread_mutex_lock(&state_mutex);
		switch(nbiot_data->current_state)
		{
			case STATUS_INIT:
				//		rv = nbiot_reset(&nbiot_data->comport);
				rv = send_atcmd_check_ok(&nbiot_data->comport, "AT", 500);
				if( rv<0 )
				{
					printf ("debug:STATUS_INIT failed.\n");
					pthread_mutex_unlock(&state_mutex);
					break;
				}
				else
					nbiot_data->current_state++;
			case STATUS_PRESEND:
				rv = nb_status_presend(&nbiot_data->comport);
				if( rv<0 )
				{
					printf ("debug:STATUS_PRESEND failed.\n");
					pthread_mutex_unlock(&state_mutex);
					break;
				}
				else
					nbiot_data->current_state++;
			case STATUS_CONFIG:
				rv = nb_status_config(&nbiot_data->comport);
				if( rv<0 )
				{
					printf ("debug : STATUS_CONFIG failed.\n");
					pthread_mutex_unlock(&state_mutex);
					break;
				}
				else
				{
					nbiot_data->current_state++;
					pthread_mutex_unlock(&state_mutex);
				}
			case STATUS_READY:

				printf ("READY!!!!!!\n");
				sleep(30);
				break;
			default:
				pthread_mutex_unlock(&state_mutex);
				break;
		}
	}

	return NULL;
}


void *report_data(void *arg)
{
	nb_config_t*      nbiot_data = (nb_config_t*)arg;
	int               rv;
	char              bufferW[DATA_SIZE];


	printf ("test state:%d\n", nbiot_data->current_state);

	while(1)
	{

		sht2x_get_temp_rh(bufferW, DATA_SIZE);
		rv = comport_send(&nbiot_data->comport, bufferW, DATA_SIZE );
		if( rv<0 )
		{
			printf ("Send AT data failure.\n");
		}
		else
			printf ("Send successful:%s\n",bufferW);
		sleep(10);
	}
}


void *receive_data(void *arg)
{
	nb_config_t*      nbiot_data = (nb_config_t*)arg;
	int               rv;
	int               bytes = 0;
	fd_set            read_fds;
	char              value[256];
	char              bufferR[1024];
	comport_t         comport;
	int               timeout_ms = 5000;
	struct timeval    timeout;
	leds_t            leds =
	{
		.leds = leds_info,
		.count = LED_MAX,
	};

	comport = nbiot_data->comport;

	if( (rv=init_led(&leds))<0 )
	{   
		printf ("initial leds gpio failure,rv=%d\n",rv);
	}

	while(1)
	{

		memset(bufferR, 0, sizeof(bufferR));
		memset(value, 0, sizeof(value));
		printf ("Receiveing data...\n");


		FD_ZERO(&read_fds);
		FD_SET(comport.fd, &read_fds);

		timeout.tv_sec = timeout_ms/1000;
		timeout.tv_usec = (timeout_ms % 1000) * 1000;

		//		printf ("debug:comport->fd:%d\n",comport.fd);
		rv =select((comport.fd)+1, &read_fds, NULL, NULL, &timeout);
		if( rv<0 )
		{
			printf ("Select error\n");
		}
		else if( rv==0 )
		{
			printf ("timeuot\n");
		}
		else
		{
			if( FD_ISSET(comport.fd, &read_fds))
			{
				bytes = comport_recv(&comport, bufferR, sizeof(bufferR), 100);
				if( rv<0 )
				{
					printf ("Read failure.\n");
				}
				bufferR[bytes] = '\0';

				if( strstr(bufferR, "+NNMI:") )
				{
					LEDS_EVENT_G = 1;
					//复制接收到的内容，让处理LED线程去执行
				}
				else
				{
					SEND_EVENT_G = 1;
					//复制接收到的内容，让处理AT命令的回复的线程去执行
				}

#if 0
				if (strstr(bufferR, "0101") && strstr(bufferR, "+NNMI:"))
				{
					printf ("Turning led on\n");
					open_led(&leds, LED_R);
				}
				else if (strstr(bufferR, "0100") && strstr(bufferR, "+NNMI:"))
				{
					printf ("Turning led Off\n");
					close_led(&leds, LED_R);
				}
#endif
				else if( strstr(bufferR, "OK") )
				{

					printf ("Receive : %s\n", bufferR);
				}
			}
		}
	}
}


int main (int argc, char **argv)
{
	pthread_t      report_thread,receive_thread,state_thread;
	const char    *dev = "/dev/ttyUSB0";
	int            rv;
	nb_config_t    nbiot_data;
	comport_t      comport;


	rv = comport_open(&nbiot_data.comport, SERIAL_DEVNAME, 9600, "8n1n");
	if( rv<0 )
	{
		perror("Failed to open comport");
		return -1;
	}
	nbiot_data.current_state = 0;


	if(pthread_create(&state_thread, NULL, state_machine_thread, &nbiot_data) != 0)
	{
		perror("Failed to create state thread");
		return -2;
	}
	if( pthread_create(&report_thread, NULL, report_data,
				&nbiot_data) != 0){
		perror("Failed to create report thread");
		return -3;
	}

	if ( pthread_create(&receive_thread, NULL, receive_data,
				&nbiot_data) !=0 )
	{
		perror("Failed to create receive thread");
		return -4;
	}


	pthread_join(state_thread, NULL);
	pthread_join(report_thread, NULL);
	pthread_join(receive_thread, NULL);

	comport_close(&nbiot_data.comport);


	return 0;
} 

