/********************************************************************************
 *      Copyright:  (C) 2024 LingYun IoT System Studio
 *                  All rights reserved.
 *
 *       Filename:  nbiot.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(17/07/24)
 *         Author:  Niu Chunmin <niuchunmin@2430815397@qq.com>
 *      ChangeLog:  1, Release initial version on "17/07/24 10:24:41"
 *                 
 ********************************************************************************/

#ifndef  _NBIOT_H_
#define  _NBIOT_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <errno.h>
#include "sht20.h"
#include "logger.h"
#include "comport.h"
#include "at_cmd.h"
#include "nb_at.h"
#include "leds.h"

#define    SERIAL_DEVNAME    "/dev/ttyUSB0"
#define    DATA_SIZE         512
enum status_s{
	STATUS_INIT,
	STATUS_PRESEND,
	STATUS_CONFIG,
	STATUS_READY,
}status_t;

typedef struct nb_config_s{
	comport_t     comport;
	int           current_state;
}nb_config_t;

pthread_mutex_t   state_mutex;


void *state_machine_thread(void *arg);
void  float_to_hex_string(float f, char hexStr[9]);
void *report_data(void *arg);
void *receive_data(void *arg);

#endif
