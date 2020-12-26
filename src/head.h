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

//Interface ERMC_ERMD
#define PORTERMD  7070

typedef struct process
{
	int nb;
	time_t timestamp;
}Process;

int interface_ermd();

#endif