#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

//#include "position.h"	//agent handler, rssi_q are here...  need to be restructure to make it look nice..
#include "./utils/mongoose.c"
#include "agentHandler.h"

static const char *s_http_port = "9998";
static struct mg_serve_http_opts s_http_server_opts;

int get_beacon_name_from_url(char *str, char *beacon_id)
{
	char *part, *beacon;
	int i = 0;

	if(str == NULL) return -1;

	while(str[i] == '/'){
		i++;
	}
	part = str+i;
	
	part = strtok(part, "/");

	//check url
	if(strcmp(part, "tag") == 0)
	{
		//get beacon id
		beacon = part;
		beacon = strtok(NULL,"/");

		if(beacon == NULL) return 0;

		strncpy(beacon_id, beacon, strlen(beacon)+1);
		return 1;
	}
	return 0;
}

static void ev_handler(struct mg_connection *c, int ev, void *p) {
	int i, offset = 1, pos_offset = 0, len = 0;
	char beacon_id[32];
	char pos_message[128];
	char *found_mac = NULL;
	char error_msg[] = "\"Tag Not Found\"";
	char msg1[] = "\"BeaconID\": ", msg2[] = "\"Success\": ", msg3[] = "\"Message\": ", msg4[] = "true", msg5[] = "false"; 
	beacon_info_t *infos;
	pos_list_t *list;

	if (ev == MG_EV_HTTP_REQUEST) {
		struct http_message *hm = (struct http_message *) p;
		char url[100];

		snprintf(url, sizeof(url), "%.*s", (int) hm->uri.len, hm->uri.p);
		i = get_beacon_name_from_url(url, beacon_id);
		if(i == 1){
			found_mac = get_beacon_mac_addr(beacon_id);

			if(found_mac != NULL){
				pos_message[0] = '{';
				
				//x, y
				list = get_pos_list();
				pos_offset = get_pos_by_mac(list, found_mac, pos_message, offset);

				if(pos_offset > offset){
					offset = pos_offset;
					//beacon id
					len = (int) strlen(msg1);
					strncpy(pos_message+offset, msg1, len);

					offset += len; 

					pos_message[offset] = '\"';
					len = (int) strlen(beacon_id);
					strncpy(pos_message+offset+1, beacon_id,len);
					pos_message[1+offset+len]='\"';
					pos_message[2+offset+len]=',';
					pos_message[3+offset+len]=' ';
					offset+=(4+len);

					//success
					len = strlen(msg2);
					strncpy(pos_message+offset, msg2, len);
					offset+=len;

					len = strlen(msg4);
					strncpy(pos_message+offset, msg4, len);
					offset+=len;

					pos_message[offset] = '}';
					pos_message[offset+1] = 0;
					printf("%s\n", pos_message);

					mg_printf(c,
						"HTTP/1.1 200 OK\r\n"
						"Content-Type: application/json\r\n"
						"Content-Length: %d\r\n"
						"\r\n"
						"%s",
						(int)strlen(pos_message), pos_message);
					return;
				}
			}
		}
		
		if(i  == 0 || found_mac == NULL || offset >= pos_offset)
		{
			offset = 0;
			pos_message[0] = '{';
			len = (int) strlen(msg1);
			strncpy(pos_message+1, msg1, len);

			offset += (1+len);
			//printf("%s\n", pos_message);
			pos_message[offset] = '\"';
			len = (int) strlen(beacon_id);
			strncpy(pos_message+offset+1, beacon_id,len);
			pos_message[1+offset+len]='\"';
			pos_message[2+offset+len]=',';
			pos_message[3+offset+len]=' ';
			offset+=(4+len);
			//printf("%s\n", pos_message);

			len = strlen(msg2);
			strncpy(pos_message+offset, msg2, len);
			offset+=len;
			len = strlen(msg5);
			strncpy(pos_message+offset, msg5, len);
			offset+=len;
			pos_message[offset]=',';
			pos_message[offset+1]=' ';
			offset+=2;
			//printf("%s\n", pos_message);

			len = strlen(msg3);
			strncpy(pos_message+offset, msg3, len);
			offset+=len;
			len = strlen(error_msg);
			strncpy(pos_message+offset,error_msg, len);
			offset+=len;

			pos_message[offset] = '}';
			pos_message[offset+1] = 0;
			printf("%s\n", pos_message);

			mg_printf(c,
				"HTTP/1.1 200 OK\r\n"
				"Content-Type: application/json\r\n"
				"Content-Length: %d\r\n"
				"\r\n"
				"%s",
				(int) strlen(pos_message), pos_message);
		}else{

		}
	}
}

int start_web_server() {
	struct mg_mgr mgr;
	struct mg_connection *nc;

	mg_mgr_init(&mgr, NULL);
	printf("Starting web server on port %s\n", s_http_port);
	nc = mg_bind(&mgr, s_http_port, ev_handler);
	if (nc == NULL) {
		printf("Failed to create listener\n");
		return 1;
	}
	// Set up HTTP server parameters
	mg_set_protocol_http_websocket(nc);
	s_http_server_opts.document_root = ".";  // Serve current directory
	s_http_server_opts.enable_directory_listing = "yes";

	for (;;) {
		mg_mgr_poll(&mgr, 1000);
	}
	mg_mgr_free(&mgr);
	return 0;
}

void *start_pos_output_thread(void *arg){
	start_web_server();
}


/*
 *this is deprecated because the api is migrated into this program..

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
}*/