#include <getopt.h>
#include <sys/prctl.h>
#include "head.h"

//#include <errno.h>
//#include <pthread.h>
//#include <sys/socket.h>
//#include <sys/types.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>

#define VERSION     0.10
//#define PORTEMD     7070

//#define IP_SIZE		16


int pid_ermc_web, pid_ermc_ermd;

void ParentCycle()
{
	printf("Parent process %d\n", getpid());
	//signal(SIGCHLD, sub_quit_signal_handle);
	//signal(SIGSEGV,sig_segv_handler); 
	//signal(SIGINT, sig_close_handler);
	//atexit(server_on_exit);
	while(1)
		pause();
}

/*
void *client(void *num)
{
	int sockfd = *(int*)num;
	unsigned char *ip_emd = NULL;
	struct sockaddr_in addr;
	socklen_t addr_len;
	
	ip_emd = (char *)malloc(IP_SIZE*sizeof(char));
	addr_len = sizeof(addr);
	memset(&addr, 0, addr_len);
	getpeername(sockfd, (struct sockaddr *)&addr, &addr_len);
	sprintf(ip_emd, "%s", inet_ntoa(addr.sin_addr));
	printf("[emc_emd] sockfd : %d   ip: %s  \n", sockfd,ip_emd);
}
*/


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
	
	if (argc ==1)
	{
		flag_ermd = 1;
		flag_web = 1;
		flag_option = 1;
	}else if(argc>1)
	{
		flag_option = 2;
		while((oc = getopt_long(argc, argv, "dh",long_options, &option_index)) != -1)
		{
			switch(oc)
			{
				case 0:
					if (strcmp(long_options[option_index].name, "ermd") == 0)
						flag_ermd = 0;
					else if (strcmp(long_options[option_index].name, "web") == 0)
						flag_web = 0;
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
			//ret = interface_ermd();
			while(1);
			return ret;
		}
		else //parent process 
		{
			//pro[AMC_AMD].nb = 1;
			//time(&(pro[AMC_AMD].timestamp));
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
			//pro[AMC_ACCA].nb = 1;
			//time(&(pro[AMC_ACCA].timestamp));
		}
		pid_ermc_web = pid_web;
	}
	
	ParentCycle();
}
	
/*	

//int sockfd_server, socked_client;
	//struct sockaddr_in server_addr,client_addr;	
	
	//int reuse = 1;
	// create a new sccket  (AF_INET: IPv4)
	if((sockfd_server=socket(AF_INET,SOCK_STREAM,0))<0)  
	{  
		printf("[emc_emd] create socket error: %s\n", strerror(errno));  
		return -1; 
	}
	//create an interface
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;  //Address family
	server_addr.sin_port = htons(PORTEMD);  //Port number 
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // Internet address
	
	if (setsockopt(sockfd_server, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
	{
		printf("[emc_emd]setsockopet error: %s\n",strerror(errno));
		close(sockfd_server);
		return -1;
	}
	if (bind(sockfd_server,(struct sockaddr *) &server_addr,sizeof(server_addr)) ==-1)
	{
		printf("[emc_emd] bind socket error: %s\n",strerror(errno));
		close(sockfd_server);
		return -1;
	}
	if(listen(sockfd_server,5) == -1)
	{
		printf("[emc_emd] listen socket error: %s\n",strerror(errno));
		close(sockfd_server);
		return -1;
	}
	while(1)
	{
		int ret;
		pthread_t thread_id;
		socklen_t cliaddr_len = sizeof(client_addr);
		socked_client = accept(sockfd_server, (struct sockaddr*)&client_addr,&cliaddr_len);
		if(socked_client<0)
		{
			printf("[emc_emd] accept socket error: %s\n",strerror(errno));
			break;
		}
		
		ret = pthread_create(&thread_id, NULL,(void *)client, &socked_client);
		if(ret !=0)
		{
			printf("[emc_emd] pthread_create client error. \n");
			break;
		}
	}
	
}*/