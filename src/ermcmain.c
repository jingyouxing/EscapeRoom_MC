#include <getopt.h>
#include <sys/prctl.h>
#include <sys/wait.h>
#include "head.h"

#define VERSION     0.10

int pid_ermc_web, pid_ermc_ermd;
Process pro[NM_PROCESS];

void server_on_exit(void)
{
    log_info("ERMC closed : %d.\n",  getpid());
}

void sub_quit_signal_handle(int sig) 
{   
    pid_t pid;   
	time_t now;
	
	if (sig == SIGCHLD) 
	{   
		pid = wait(NULL);
		if (pid == pid_ermc_ermd)
		{
			log_fatal("ermc_ermd(%d) dead. \n", pid);
			time(&now);
			if((now-pro[ERMC_ERMD].timestamp)<30)
				exit(1);
			sleep(1);
			int pid_ermd = fork();
			int ret;
			if(pid_ermd<0)
			{
				log_fatal("Error in fork pid_amd! \n");
				exit(1);
			}	
			else if (pid_ermd ==0)  //child process pid_amd because return value zero
			{ 
				log_info("[pid_ermd] Child process ermc_ermd: %d. \n", getpid());
				//rename process
				prctl(PR_SET_NAME, "ermc_ermd");
				//child process dies when parent dies
				prctl(PR_SET_PDEATHSIG, SIGHUP);
				//ret = interface_amd();
				while(1);
				return;
			}
			else //parent process 
			{
				pid_ermc_ermd = pid_ermd;
				pro[ERMC_ERMD].nb = 1;
				time(&(pro[ERMC_ERMD].timestamp));
			}
		}
		else if(pid == pid_ermc_web)
		{
			log_fatal("ermc_web(%d) dead. \n", pid);
			time(&now);
			if((now-pro[ERMC_WEB].timestamp)<30)
				exit(1);
			sleep(1);
			int pid_web = fork();
			int ret;
			if(pid_web<0)
			{
				log_fatal("Error in fork ermc_web!\n");
				exit(1);
			}
			else if (pid_web ==0)  //child process ermc_web
			{
				//exec();
				log_info("[ermc_web] Child process ermc_web: %d. \n", getpid());
				//rename process
				prctl(PR_SET_NAME, "ermc_web");
				//child process dies when parent dies
				prctl(PR_SET_PDEATHSIG, SIGHUP);
				//ret = interface_acca();
				while(1);
				return;
			}
			else 
			{
				pid_ermc_web = pid_web;
				pro[ERMC_WEB].nb=1;
				time(&(pro[ERMC_WEB].timestamp));
			}
		}	
    }   
} 

void ParentCycle()
{
	//printf("Parent process %d\n", getpid());
	signal(SIGCHLD, sub_quit_signal_handle);
	//signal(SIGSEGV,sig_segv_handler); 
	//signal(SIGINT, sig_close_handler);
	atexit(server_on_exit);
	while(1)
		pause();
}

int iniRedis()
{
	redisContext* conn;
	redisReply* reply;
	
	conn = redisConnect("127.0.0.1", 6379);  
    if(conn->err)   
	{
		log_fatal("Redis connection error: %s. \n", conn->errstr);
		return -1;
	}
	
	reply = redisCommand(conn, "select 0");
	reply = redisCommand(conn, "flushdb"); 
	freeReplyObject(reply);
	redisFree(conn);
	return 0;
}

int import_ermd(char *file_name)
{
	FILE *fp;
	char buffer[200];	
	int ret;
	
	fp=fopen(file_name,"r");
	if (!fp) 
	{
		log_fatal("Failed to open file %s for reading : %s \n", file_name, strerror(errno));
        return -1;
    }
	if(fgets(buffer, sizeof(buffer), fp)==NULL)
	{
		log_fatal("The file %s is null \n", file_name);
		fclose(fp);
        return -1;
	}
	
	redisContext* conn;
	redisReply* reply;
	char *result = NULL;
	char ermd_ip[IP_SIZE];
	char ermd_mac[MAC_SIZE];
	int type,num=0;
	int nb_ermd = 0;
	
	conn = redisConnect("127.0.0.1", 6379);  
    if(conn->err)   
	{
		log_fatal("Redis connection error: %s. \n", conn->errstr);
		return -3;
	}
	
	while(fgets(buffer, sizeof(buffer), fp) != NULL) 
	{
		int count=0;
		char *commandArr[20]; 
		result = strtok(buffer, ",");
		while( result != NULL ) 
		{
			switch(count)
			{
				case 0:
					strcpy(ermd_mac,result);
					break;
				case 1:
					strcpy(ermd_ip,result);
					break;
				case 2:
					type =atoi(result);
					break;
				case 3:
					num =atoi(result);
					break;
				default: 
					break;
			}
			if(count>=4)
			{
				commandArr[count-4]=(char*)malloc(MAC_SIZE*sizeof(char));
				if (count==(3+num)) //last parametre
					result[strlen(result)-2] = '\0';
				strcpy(commandArr[count-4],result);
			}
			result = strtok(NULL, ",");
			count++;
		}
		//put the info in redis
		reply = redisCommand(conn, "select 0");
		reply = redisCommand(conn, "HSET %s  ip %s  type %d  active 0 time_conn 0 numCommand %d", ermd_mac, ermd_ip, type, num);
		for (int i=0;i<num; i++)
			reply = redisCommand(conn, "HSET %s command%d %s", ermd_mac, i, commandArr[i]);	
		freeReplyObject(reply);
		nb_ermd++;
	}
	if(nb_ermd>0)
	{
		log_info("Import %s OK, %d ERMDs.\n", file_name, nb_ermd);
		ret = 0;
	}
	else 
	{
		log_fatal("Import 0 AMDs. \n");
		ret = -1;
	}
	fclose(fp);
	redisFree(conn); 
	return 0;
}

int display_usage()
{
	printf("\nUsage: ermc [OPTIONS] [cmd [arg [arg ...]]] \n");
	printf("  --web                 Disable web \n");
	printf("  --ermd                Disable ermd \n");
	printf("  -d                    Enable debug infomation \n");
	printf("  -h/--help             Help\n");
	printf("When no command is given, ermc starts with configuration default. (Recommend) \n\n");
	return 0;
}


/*************************************************************************
* multi process 
* parent : ermc, 
* clids : ermc_ermd : inerface ermc_ermd, server
*		  ermc_web: interface ermc_web, client
* how to use :  display_usage()
*************************************************************************/

int main(int argc, char** argv)
{
	//Initialisation
	int oc;
	char *file_name = NULL;
	int flag_web, flag_ermd;
	int flag_option=0; 
	float version = VERSION;
	int option_index = 0;
	static struct option long_options[] =
	{
		{"file", required_argument, 0, 'f'},
		{"ermd", no_argument, 0, 0}, 
		{"web", no_argument, 0, 0},
		{"debug", no_argument, 0, 'd'},
		{"help", no_argument, 0, 'h'},
		{0, 0, 0, 0}
	};
	
	file_name = (char*)calloc(100, sizeof(char));
	flag_ermd = 1;
	flag_web = 1;
	flag_option = 1;
	
	if(argc>1)
	{
		flag_option = 2;
		while((oc = getopt_long(argc, argv, "f:dh",long_options, &option_index)) != -1)
		{
			switch(oc)
			{
				case 0:
					if (strcmp(long_options[option_index].name, "ermd") == 0)
					{	
						flag_ermd = 0;
					}
					else if (strcmp(long_options[option_index].name, "web") == 0)
						{
							flag_web = 0;
						}
					break;
				case 'f': 
					strcpy(file_name, optarg);
					break;
				case 'd': 
					enable_debug();
					printf("d\n");
					break;
				case 'h': 
				case '?': 
					flag_option = 0;
					break;
				default: 
					break;
			}
		}		
	}
	
	if(flag_option==0)
	{
		display_usage();
		exit(1);
	}
	
	if (*file_name == 0)
		strcpy(file_name, "importedInfo.CSV");
	
	log_info("----------------------------------------------------------------------------\n");
	log_info("AMC version: %.2f \n", version);
	
	if(iniRedis()<0)
	{
		log_fatal("Redis error\n");
		exit(1);
	}
	
	pid_t pid_ermd, pid_web;
	int pid_ermc = getpid();
	int ret;
	log_info("[ermc] Parent process ermc: %d. \n", pid_ermc);
	pid_ermc_web = 0;
	pid_ermc_ermd = 0;
	
	if(flag_ermd)
	{
		if (import_ermd(file_name) <0)
		{
			log_fatal("Import %s KO.\n", file_name);
			free(file_name);
			exit(1);
		}
		free(file_name);
		//creat a new process ermc_ermd
		pid_ermd = fork();
		if(pid_ermd<0)
		{
			log_fatal("Error in fork pid_ermd! \n");
			exit(1);
		}
		else if (pid_ermd ==0)  //child process pid_amd because return value zero
		{ 
			log_info("[pid_ermd] Child process ermc_ermd: %d. \n", getpid());
			//rename process
			prctl(PR_SET_NAME, "ermc_ermd");
			//child process dies when parent dies
			prctl(PR_SET_PDEATHSIG, SIGHUP);
			ret = interface_ermd();
			return ret;
		}
		else //parent process 
		{
			pro[ERMC_ERMD].nb = 1;
			time(&(pro[ERMC_ERMD].timestamp));
		}
		pid_ermc_ermd = pid_ermd;
	}
	if(flag_web)
	{
		pid_web = fork();
		if(pid_web<0)
		{
			log_fatal("Error in fork ermc_web!\n");
			exit(1);
		}
		else if (pid_web ==0)  //child process ermc_web
		{
			//exec();
			log_info("[ermc_web] Child process ermc_web: %d. \n", getpid());
			//rename process
			prctl(PR_SET_NAME, "ermc_web");
			//child process dies when parent dies
			prctl(PR_SET_PDEATHSIG, SIGHUP);
			while(1);
			//ret = interface_acca();
			return ret;
		}
		else //parent process 
		{
			pro[ERMC_WEB].nb = 1;
			time(&(pro[ERMC_WEB].timestamp));
		}
		pid_ermc_web = pid_web;
	}
	
	ParentCycle();
	return 0;
}