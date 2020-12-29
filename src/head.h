#ifndef _HEAD_H
#define _HEAD_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <hiredis/hiredis.h>
#include "log.h"

//Process
#define NM_PROCESS 2
#define ERMC_ERMD  0
#define ERMC_WEB   1

//ERMC
#define IP_SIZE		16
#define MAC_SIZE	18
#define TIME_SIZE   20

//Interface ERMC_ERMD
#define PORTERMD  7070
#define TYPE 6
#define NUMALARM  7
#define ENDALARM1 9
#define ENDALARM2 10

//Interface ERMC_web
#define SIZE_ALARMID   19
#define SSIZE_MAX   1024
#define PORTEWEB 9090
#define IP_WEB "47.100.100.146"

typedef struct process
{
	int nb;
	time_t timestamp;
}Process;

typedef struct alarmRedis
{
	unsigned char *mac;
	unsigned char time[TIME_SIZE];
	int num;
	int id;
}Alarm;

int interface_ermd();
int interface_web();
int get_json_id(unsigned char *id);

#endif