#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "log.h"

const char *log_file = NULL;
int log_lin = 0;
const char *log_fun = NULL;
int log_lev = 0; 

int flag_debug = 1;

int get_log(char *log_message)
{
	struct tm now_time;
	time_t time_seconds = time(NULL);
	char str[200];
	
	bzero(&now_time, sizeof(now_time));
	localtime_r(&time_seconds, &now_time); //thread-safe
	
	sprintf(str, "%04d-%02d-%02d ", now_time.tm_year + 1900, now_time.tm_mon + 1, now_time.tm_mday);
	strcpy(log_message,str);
	sprintf(str, "%02d:%02d:%02d ", now_time.tm_hour, now_time.tm_min, now_time.tm_sec);
	strcat(log_message,str);
	
	switch(log_lev)
	{
		case DEBUG:
			sprintf(str, "[DEBUG] %s:%d:%s()", log_file, log_lin,log_fun);
			break;
		case INFO:
			sprintf(str, "[INFO] ");
			break;
		default: break;
	}
	
	strcat(log_message,str);

	strcat(log_message," : ");
	
	return 0;
}

int write_log(const char *file, const char * fmt, ...) 
{
	char *buffer, *str;
	int ret = 0;
	
	buffer = (char *)malloc(200*sizeof(char));
	str = (char *)malloc(200*sizeof(char));
	
	get_log(buffer);
	
	va_list ap;
	va_start(ap, fmt);
	vsprintf(str,fmt, ap);
	va_end(ap);
	
	strcat(buffer,str);
	
	switch(log_lev)
	{
		case DEBUG:
			if(flag_debug)
				printf("%s",buffer);
			break;
		case INFO:
			printf("%s",buffer);
			//ret = write_log_file(file, buffer);
			break;	
		default: break;
	}
}