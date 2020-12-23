#ifndef _LOG_H
#define _LOG_H
/***************************************************************************************
* Log Level
* 0 DEBUG   : Informational events that are most useful to debug an application.     
*			  Only display on screen
*             conf -d: enable debug infomation
* 1 INFO    : Informational messages of the progress of the application
*             Display on screen and save in file(PATHLOG_ERMCSYS)
***************************************************************************************/
extern const char *log_file;
extern int log_lin;
extern const char *log_fun;
extern int log_lev;

void enable_debug();
int write_log(const char *file, const char * fmt, ...);

#define DEBUG     0
#define INFO      1

#define PATHLOG_ERMCSYS "ermcSys.log"

#define log_debug(fmt, arg...) \
	{log_file = __FILE__; log_lin = __LINE__; log_fun =__func__; log_lev = DEBUG; write_log(PATHLOG_ERMCSYS, fmt, ##arg);}

#define log_info(fmt, arg...)  {log_lev = INFO; write_log(PATHLOG_ERMCSYS, fmt, ##arg);}

#endif