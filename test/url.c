#include "../utils/mongoose.c"
#include "test.c"

static const char *s_http_port = "8000";
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
	if(strcmp(part, "test") == 0)
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

	int i;
	char beacon_id[32];
	char error_msg[] = "Please provide a beacon id.";

	if (ev == MG_EV_HTTP_REQUEST) {
		struct http_message *hm = (struct http_message *) p;
		char url[100];

		snprintf(url, sizeof(url), "%.*s", (int) hm->uri.len, hm->uri.p);
		i = get_beacon_name_from_url(url, beacon_id);
		if(i == 1){
			mg_printf(c, 
				"HTTP/1.1 200 OK\r\n"
				"Content-Type: application/json\r\n"
				"Content-Length: %d\r\n"
				"\r\n"
				"%s",
				(int) strlen(beacon_id), beacon_id);
		}else if(i  == 0)
		{
			mg_printf(c, 
				"HTTP/1.1 200 OK\r\n"
				"Content-Type: application/json\r\n"
				"Content-Length: %d\r\n"
				"\r\n"
				"%s",
				(int) strlen(error_msg), error_msg);
		}else{

		}
	}
}

int main() {
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