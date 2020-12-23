#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
//#include <pthread.h>
//#include <sys/socket.h>
//#include <sys/types.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
#include "log.h"

#define VERSION     0.10
//#define PORTEMD     7070

//#define IP_SIZE		16

/*
int pid_ermc_web, pid_ermc_ermd;

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

/*************************************************************************
* multi process 
* parent : ermc, 
* clids : ermc_ermd : inerface ermc_ermd, server
*		  ermc_web: interface ermc_web, client
* how to use :  display_usage()
*************************************************************************/

int main(int argc, char** argv)
{
	//int sockfd_server, socked_client;
	//struct sockaddr_in server_addr,client_addr;	
	float version = VERSION;
	//int reuse = 1;
	
	log_info("----------------------------------------------------------------------------\n");
	log_info("AMC version: %.2f \n", version);
	
	log_debug("hello\n");
	
/*	
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
	*/
}