#include "head.h"

#define DEBUG_IFERMD

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
	unsigned char *mac_ermd = NULL;
	struct sockaddr_in addr;
	socklen_t addr_len;
	redisContext* conn;
	redisReply* reply;
	
	ip_ermd = (char *)malloc(IP_SIZE*sizeof(char));
	addr_len = sizeof(addr);
	memset(&addr, 0, addr_len);
	getpeername(sockfd, (struct sockaddr *)&addr, &addr_len);
	sprintf(ip_ermd, "%s", inet_ntoa(addr.sin_addr));
	log_debug("[ermc_ermd] sockfd : %d   ip: %s  \n", sockfd,ip_ermd);
	
	conn = redisConnect("127.0.0.1", 6379);  
    if(conn->err) 
	{
		log_error("[ermc_ermd] Redis connection error: %s. \n", conn->errstr);
		goto amd_close;
	}
	
	unsigned char recv_buf[200];
	int recv_len;
	struct timeval timeout;
	fd_set fds;
	
	memset(recv_buf, 0, sizeof(recv_buf));

	while(1)
	{
		timeout.tv_sec=0;  
        timeout.tv_usec=200000; 
		FD_ZERO(&fds);
		FD_SET(sockfd,&fds);
		switch(select(sockfd+1,&fds,NULL,NULL,&timeout))
		{
			case -1: 
				//goto amd_close;
				break;
			case 0:
				//printf("timeout\n");
				break;
			default:
				if(FD_ISSET(sockfd,&fds))
				{
					if ((recv_len = recv(sockfd, recv_buf, sizeof(recv_buf), 0)) >0)
					{
						unsigned char time_message[TIME_SIZE];
						int i,type, num_alarm;
						time_t time_seconds; 
						struct tm now_time;
#ifdef DEBUG_IFERMD						
						printf("[ermc_ermd] [ermd %s] recv_len : %d \n",ip_ermd, recv_len);
						for (i = 0; i<recv_len; i++)
							printf(" %02x ", recv_buf[i]);
						printf("\n");
#endif					
						if(mac_ermd==NULL) //first message 
						{
							mac_ermd = (char *)malloc(MAC_SIZE*sizeof(char));
							sprintf(mac_ermd, "%02x%02x%02x%02x%02x%02x", recv_buf[0], recv_buf[1],recv_buf[2], recv_buf[3], recv_buf[4], recv_buf[5]); 
							log_info("[ermc_ermd] Connection a new client : ip: %s  mac: %s. \n",ip_ermd, mac_ermd);
						}
						type = recv_buf[TYPE];
						num_alarm = recv_buf[NUMALARM]*10+recv_buf[NUMALARM+1];
						if(type>=0 && type<=4)
						{
							//get time
							time_seconds = time(NULL);
							localtime_r(&time_seconds, &now_time); //thread-safe
							sprintf(time_message, "%d/%d/%d %d:%d:%d", (now_time.tm_year+1900), now_time.tm_mon+1, now_time.tm_mday,now_time.tm_hour,now_time.tm_min,now_time.tm_sec);
							if(type==0)
							{
								Alarm alarm;
								
								reply = redisCommand(conn, "select 1");
								reply = redisCommand(conn, "incr mykey");
								alarm.id = reply->integer;
								alarm.num = num_alarm;
								alarm.mac = mac_ermd;
								strcpy(alarm.time, time_message);
								get_json_id(alarm.alarmID);
								reply = redisCommand(conn, "HSET %d title SendAlarm fin 1 id %s master_id ermc1 web_id EscapeRoom device_id %s device_time %s num %d", alarm.id, alarm.alarmID,alarm.mac,alarm.time,alarm.num);
							}
						}
					}
				}
				break;
		}
	}
amd_close:
	if(conn !=NULL)
		redisFree(conn);
	if(reply !=NULL)
		freeReplyObject(reply);
	if(mac_ermd!=NULL)
	{
		log_error("[ermc_ermd] [ermd %s] closed. \n", mac_ermd);
	}	
	else
	{
		log_error("[ermc_ermd] [ermd] closed. \n");
	}
	close(sockfd);
    pthread_exit(NULL); 		
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