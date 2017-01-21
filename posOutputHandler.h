#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include "position.h"	//agent handler, rssi_q are here...  need to be restructure to make it look nice..


void handle_output_connection(pos_list_t *pos_list, int connfd){
	int len, offset=0;
	char recv_buf[32];
	char send_buf[1024];

	memset(recv_buf, '0', 32);
	memset(send_buf, '0', 1024);
	//trigger sending by app..
	while((len = read(connfd, recv_buf, sizeof(recv_buf)-1))>0)
	{
		recv_buf[len] = 0;
		get_pos(pos_list, send_buf, &offset);
		printf("%s\n", send_buf);
		if(offset == 0){
			printf("No data\n");
			write(connfd, recv_buf, len);
		}else{
			write(connfd, send_buf, offset);
		}
	}
}


void *start_pos_output_thread(void *arg){
	struct sockaddr_in serv_addr;
	int listen_fd;
	int connfd;
	int yes = 1;

	pos_list_t *pos_list = (pos_list_t*) arg;

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(9998);

	setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	if(bind(listen_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
	{
		printf("[ERROR] Failed to bind to port 9998\n");
		exit(0);
	}

	if(listen(listen_fd, 10) == -1)
	{
		printf("[ERROR] Failed to listen to port 9998\n");
		exit(0);
	}

	while(1)
	{
		printf("[INFO] Listening on port 9998...(OUTPUT PORT)\n");
		connfd = accept(listen_fd, (struct sockaddr*)NULL, NULL);

		handle_output_connection(pos_list, connfd);
	}
}