/*********************************************************************************
 *      Copyright:  (C) 2024 LingYun IoT System Studio
 *                  All rights reserved.
 *
 *       Filename:  nb_at.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(20/07/24)
 *         Author:  Niu Chunmin <niuchunmin@2430815397@qq.com>
 *      ChangeLog:  1, Release initial version on "20/07/24 13:30:07"
 *                 
 ********************************************************************************/

#include "nb_at.h"


static inline int check_at_ready(comport_t *comport)
{
	int             times = 6;
	int             ready = 0;

	while( times-- )
	{
		if( 0 == send_atcmd_check_ok(comport, "AT", 500) )
		{
			ready = 1;
			break;
		}
	}

	return ready;
}

int nbiot_reset(comport_t *comport)
{
	int             rv;
	/*  
		rv = send_atcmd(comport, "AT+NRB", 5000, "OK", AT_ERRSTR, NULL, 0);
		if( rv < 0)
		{
		log_error("send AT command to reset NB-IoT failed, rv=%d\n", rv);
		return -1;
		}
		*/

	if( check_at_ready(comport) )
	{
		log_info("send AT to reset NB-IoT and AT command ready\n");
		return 0;
	}
	else
	{
		log_info("send AT to reset NB-IoT but AT command not ready\n");
		return -3;
	}

}



int  nb_status_present(comport_t *comport)
{
	int rv=0;

	rv = send_atcmd(comport, "AT+CGMI", 5000, "OK", AT_ERRSTR, NULL, 0);
	if( rv < 0)
	{
		log_error("send AT+AGMI command to  failed, rv=%d\n", rv);
		return -1;
	}

	rv = send_atcmd(comport, "AT+CGMM", 5000, "OK", AT_ERRSTR, NULL, 0);
	if( rv < 0)
	{
		log_error("send AT+CGMM command to  failed, rv=%d\n", rv);
		return -2;
	}


	rv = send_atcmd(comport, "AT+CIMI", 5000, "OK", AT_ERRSTR, NULL, 0);
	if( rv < 0)
	{
		log_error("send AT+CIMI command to  failed, rv=%d\n", rv);
		return -3;
	}

	return 0;
}


int nb_status_config(comport_t *comport)
{
	int rv;

	rv = send_atcmd(comport, "AT+NCONFIG=AUTOCONNECT,TRUE", 5000, "OK", AT_ERRSTR, NULL, 0);
	if( rv < 0)
	{
		log_error("send AT+NCONFIG=AUTOCONNECT,TRUE command to  failed, rv=%d\n", rv);
		return -1;
	}

	rv = send_atcmd(comport, "AT+CFUN=1", 5000, "OK", AT_ERRSTR, NULL, 0);
	if( rv < 0)
	{
		log_error("send AT+CFUN command to  failed, rv=%d\n", rv);
		return -1;
	}

	rv = send_atcmd(comport, "AT+CGATT=1", 5000, "OK", AT_ERRSTR, NULL, 0);
	if( rv < 0)                 
	{         
		log_error("send AT+CFUN command to  failed, rv=%d\n", rv);
		return -1;
	}

	rv = send_atcmd(comport, "AT+CGATT?", 5000, "OK", AT_ERRSTR, NULL, 0);
	if( rv < 0)                 
	{         
		log_error("send AT+CFUN command to  failed, rv=%d\n", rv);
		return -1;
	}

	rv = send_atcmd(comport, "AT+CEREG?", 5000, "OK", AT_ERRSTR, NULL, 0);
	if( rv < 0)                 
	{         
		log_error("send AT+CFUN command to  failed, rv=%d\n", rv);
		return -1;
	}

	rv = send_atcmd(comport, "AT+CGPADDR", 5000, "OK", AT_ERRSTR, NULL, 0);
	if( rv < 0)                 
	{         
		log_error("send AT+CFUN command to  failed, rv=%d\n", rv);
		return -1;
	}

	rv = send_atcmd(comport, "AT+NCDP=221.229.214.202,5683", 5000, "OK", AT_ERRSTR, NULL, 0);
	if( rv < 0)                  
	{  
		log_error("send AT+CFUN command to  failed, rv=%d\n", rv);
		return -1;
	}

	rv = send_atcmd(comport, "AT+NCDP?", 5000, "OK", AT_ERRSTR, NULL, 0);
	if( rv < 0)                  
	{  
		log_error("send AT+CFUN command to  failed, rv=%d\n", rv);
		return -1;
	}

	rv = send_atcmd(comport, "AT+NMSTATUS?", 5000, "OK", AT_ERRSTR, NULL, 0);
	if( rv < 0)                  
	{  
		log_error("send AT+CFUN command to  failed, rv=%d\n", rv);
		return -1;
	}

	return 0;

}
