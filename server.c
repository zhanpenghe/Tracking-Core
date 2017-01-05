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

int main(int argc, char *argv[])
{
	int listenfd = 0, connfd = 0; //file descriptors
	struct sockaddr_in serv_addr;
	int port = 9999, len = 0;

	char buf[1025];

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, '0', sizeof(serv_addr));
	memset(buf, '0', sizeof(buf));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(9999);

	bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

	listen(listenfd, 10);

	while(1)
	{
		printf("Listening on port %d...\n", port);
		connfd = accept(listenfd, NULL, NULL);
		break;
	}

	while((len = read(connfd, buf, sizeof(buf)-1))>0)
	{
		buf[len] = 0;
		printf("%s\n", buf);
	}
}
