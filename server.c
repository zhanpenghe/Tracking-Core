#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#include "agentHandler.h"

int port = 9999;

int main(int argc, char *argv[])
{
	int listenfd = 0, connfd = 0; //file descriptors
	struct sockaddr_in serv_addr;
	struct sockaddr_in* client_addr = malloc(sizeof(struct sockaddr_in));
	char ip_str[INET_ADDRSTRLEN];

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
		connfd = accept(listenfd, (struct sockaddr*)client_addr, NULL);
		/*struct in_addr ipAddr = client_addr->sin_addr;
		inet_ntop(AF_INET, &ipAddr, ip_str, INET_ADDRSTRLEN);*/
		printf("Get connection from %s:%d\n", inet_ntoa(client_addr->sin_addr),(int)ntohs(client_addr->sin_port));
		agent_t *agent = malloc(sizeof(agent_t));
		setAgent(agent, connfd, NULL);
		
		sleep(100);
	}
}
