#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include "agentHandler.h"

int port = 9999;

void agent_thread_init(int connfd);

int main(int argc, char *argv[])
{
	int listenfd = 0, connfd = 0; //file descriptors
	struct sockaddr_in serv_addr;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);

	bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	listen(listenfd, 10);

	while(1)
	{
		printf("Listening on port %d...\n", port);
		connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);

		//set up agent handling thread
		agent_thread_init(connfd);
		//sleep(100);
	}
}

void agent_thread_init(int connfd)
{
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);
	agent_t *agent;
	pthread_t tid;

	getpeername(connfd, (struct sockaddr *)&addr, &len);
	printf("Get connection from  %s:%d\n", inet_ntoa(addr.sin_addr),(int)ntohs(addr.sin_port)); 

	//start agent handling thread
	agent = malloc(sizeof(agent_t));
	setAgent(agent, connfd, NULL);
	pthread_create(&tid, NULL, printRSSIFromAgent, (void*)agent);
	printf("thread created\n");
}





