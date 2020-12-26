#include "head.h"

void ermd_sig_segv_handler(int sig)
{
    log_fatal("[ermc_ermd] segv_error(value: %d). \n", sig);
    exit(1);
}

void ermd_sig_close_handler(int sig)
{
	if(sig == SIGHUP)
	{
		log_debug("[ermc_ermd] recv SIGHUP..\n");
		exit(0);
	}
}

void ermd_install_signal_handler()
{
	signal(SIGSEGV,ermd_sig_segv_handler);                                         
	signal(SIGHUP, ermd_sig_close_handler);
	//signal(SIGTERM, sig_close_handler);                                        
    //signal(SIGINT, sig_close_handler);
}


void *client(void *num)
{
	int sockfd = *(int*)num;
	unsigned char *ip_ermd = NULL;
	struct sockaddr_in addr;
	socklen_t addr_len;
	
	ip_ermd = (char *)malloc(IP_SIZE*sizeof(char));
	addr_len = sizeof(addr);
	memset(&addr, 0, addr_len);
	getpeername(sockfd, (struct sockaddr *)&addr, &addr_len);
	sprintf(ip_ermd, "%s", inet_ntoa(addr.sin_addr));
	log_debug("[emc_emd] sockfd : %d   ip: %s  \n", sockfd,ip_ermd);
}


int interface_ermd()
{
	int sockfd_server, socked_client;
	struct sockaddr_in server_addr,client_addr;
	int reuse = 1;
	
	ermd_install_signal_handler();
	
	// create a new sccket  (AF_INET: IPv4)
	if((sockfd_server=socket(AF_INET,SOCK_STREAM,0))<0)  
	{  
		log_fatal("[ermc_ermd] create socket error: %s\n", strerror(errno));  
		return -1; 
	}
	
	//create an interface
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;  //Address family
	server_addr.sin_port = htons(PORTERMD);  //Port number 
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // Internet address
	
	if (setsockopt(sockfd_server, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
	{
		log_fatal("[ermc_ermd]setsockopet error: %s\n",strerror(errno));
		close(sockfd_server);
		return -1;
	}
	if (bind(sockfd_server,(struct sockaddr *) &server_addr,sizeof(server_addr)) ==-1)
	{
		log_fatal("[ermc_ermd] bind socket error: %s\n",strerror(errno));
		close(sockfd_server);
		return -1;
	}
	if(listen(sockfd_server,5) == -1)
	{
		log_fatal("[ermc_ermd] listen socket error: %s\n",strerror(errno));
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
			log_error("[ermc_ermd] accept socket error: %s\n",strerror(errno));
			break;
		}
		
		ret = pthread_create(&thread_id, NULL,(void *)client, &socked_client);
		if(ret !=0)
		{
			log_error("[ermc_ermd] pthread_create client error. \n");
			break;
		}
	}
}