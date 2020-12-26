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
	int flag_web, flag_ermd;
	int flag_option=0; 
	float version = VERSION;
	int option_index = 0;
	static struct option long_options[] =
	{
		{"ermd", no_argument, 0, 0}, 
		{"web", no_argument, 0, 0},
		{"debug", no_argument, 0, 'd'},
		{"help", no_argument, 0, 'h'},
		{0, 0, 0, 0}
	};
	
	flag_ermd = 1;
	flag_web = 1;
	flag_option = 1;
	
	if(argc>1)
	{
		flag_option = 2;
		while((oc = getopt_long(argc, argv, "dh",long_options, &option_index)) != -1)
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
	
	
	log_info("----------------------------------------------------------------------------\n");
	log_info("AMC version: %.2f \n", version);
	log_debug("hello\n");
	
	
	pid_t pid_ermd, pid_web;
	int pid_ermc = getpid();
	int ret;
	log_info("[ermc] Parent process ermc: %d. \n", pid_ermc);
	pid_ermc_web = 0;
	pid_ermc_ermd = 0;
	
	if(flag_ermd)
	{
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
}