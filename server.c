/*
 * Tracking core server
 * @Author Adam Ho
 */


#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <pthread.h>
#include <signal.h>

#include "posOutputHandler.h"
#include "headers/server.h"

int listenfd = 0; //file descriptors
int port = 9999;	//default port
int agent_num = 0;
int room_num = 0;

//Loggers for I/O
logger_t *logger;

//data structures
agent_list_t *agent_list;	// agent handlers 
pos_calc_t *info_for_calculation;	//information for calculation
blist_t *list;	//store all the rssi
pos_list_t *position_list;	//store all the position

int main(int argc, char **argv)
{
	char hostname[100];
	int connfd = 0;
	int listenfd = start_server_thread();

	load_commandline_arg(argc, argv);

	//get agent information from list_agent.txt
	param_slist_t *agent_infos_p = (param_slist_t*) malloc(sizeof(param_slist_t));
	agent_num = get_agent_count(agent_infos_p);

	agent_info_t agent_infos[agent_num];		//to be used later in calculation

	load_agent_infos(agent_infos, agent_infos_p);
	free_param_slist(agent_infos_p);

	param_slist_t *room_infos_p = (param_slist_t*) malloc(sizeof(param_slist_t));
	room_num = get_room_count(room_infos_p);

	room_info_t room_infos[room_num];		//to be used later in calculation

	load_room_infos(room_infos, room_infos_p);
	free_param_slist(room_infos_p);

	//install a signal handler here
	if (signal(SIGINT, SIGINT_Handler) == SIG_ERR) {
		printf("SIGINT install error\n");
		exit(1);
    }

    logger = (logger_t*) malloc(sizeof(logger_t));
    init_logger(logger, 'w', "log.txt");
	printf("[INFO] Successfully created logger for rssi\n");


    agent_list = (agent_list_t *)malloc(sizeof(agent_list_t));
    init_agent_list(agent_list);

    list = (blist_t *) malloc(sizeof(blist_t));
    init_blist(list, 10);

    position_list = (pos_list_t *) malloc(sizeof(pos_list_t));
    init_pos_list(position_list);

	gethostname(hostname, sizeof(hostname));
	printf("[INFO] Local Machine Hostname: %s\n", hostname);

	start_calculation_thread(agent_infos, room_infos);
	start_output_thread();

	while(1)
	{
		printf("[INFO] Listening on port %d...(AGENT PORT)\n", port);
		connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);

		//set up agent handling thread
		agent_thread_init(connfd);
	}
}

void load_commandline_arg(int argc, char** argv)
{
	int c;
	setvbuf(stdout, NULL, _IONBF, 0);
	opterr = 0;

	while((c = getopt(argc, argv, "p:h")) != -1){
		switch(c){
			case 'p':
				port = atoi(optarg);
				break;
			case 'h':
                usage();
                exit(0);
                break;
			default:
				printf("-h for help.\n");
				exit(EXIT_FAILURE);
		}
	}
}

static void usage(){
	printf("IoT Eye Tracking Core Server program.\n"
		"Usage:\n");
	printf("\t./server [options]\n");
	printf("Options:\n"
			"\t-p [port_number]        Host server port. [Default 9999].\n"
			"\t-h                      Help.\n");
}

void SIGINT_Handler(int sig)
{
	printf("\n[INFO] QUITING THE PROGRAM HERE\n");
	void *tret;

	//lock everything to prevent I/O's from other threads..
	pthread_mutex_lock(&logger->lock);
	pthread_mutex_lock(&list->lock);

	free_agent_list(agent_list);

	shutdown(listenfd, SHUT_RDWR);
	close(listenfd);
	printf("[INFO] All sockets were shut down.\n");
	pthread_mutex_unlock(&logger->lock);
	pthread_mutex_unlock(&list->lock);

	//pthread_mutex_lock(&position_list->lock);
	free(info_for_calculation);
	free_logger(logger);
	printf("[INFO] Logger is freed.\n");
	print_blist(list);
	free_blist(list);
	printf("[INFO] Blist is freed.\n");
	free_pos_list(position_list);

	exit(0);
}

//return the file descriptor
int start_server_thread()
{
	struct sockaddr_in serv_addr;
	int yes = 1;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);

	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	if(bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
	{
		printf("[ERROR] Failed to bind to port %d\n", port);
		exit(0);
	}

	if(listen(listenfd, 10) == -1)
	{
		printf("[ERROR] Failed to listen to port %d\n", port);
		exit(0);
	}

	return listenfd;
}

void start_calculation_thread(agent_info_t infos[], room_info_t room_infos[])
{
	pthread_t tid;

	printf("[INFO] Total # of agents is %d.\n",agent_num);

	info_for_calculation = (pos_calc_t *) malloc(sizeof(pos_calc_t));
	info_for_calculation->list = list;
	info_for_calculation->pos_list = position_list;
	info_for_calculation->agent_num = agent_num;
	info_for_calculation->infos = infos;
	info_for_calculation->room_infos = room_infos;

	pthread_create(&tid, NULL, pos_generation, (void*)info_for_calculation);

	printf("[INFO] Calcultion thread started\n");
}

void start_output_thread()
{
	pthread_t tid;

	pthread_create(&tid, NULL, start_pos_output_thread, position_list);
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

	agent_logger_blist_t albl;
	albl.agent = agent;	//agent thread
	albl.logger = logger;	//logger for I/O
	albl.list = list;	//blist
	albl.agent_list = agent_list; //all the agent threads

	pthread_create(&tid, NULL, log_and_storeRSSIFromAgent, (void*)&albl);

	printf("thread created\n");
}

void agent_thread_init_log(int connfd)
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
	agent_and_logger_t al;
	al.agent = agent;
	al.logger = logger;

	pthread_create(&tid, NULL, log_and_storeRSSIFromAgent, (void*)&al);
	printf("thread created\n");
}

