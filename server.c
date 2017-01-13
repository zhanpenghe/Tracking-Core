/*
 * Tracking core server
 * @Author Adam Ho
 */

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
#include <signal.h>

#include "position.h"
#include "headers/server.h"
#include "headers/agentInfo.h"

int listenfd = 0; //file descriptors
int port = 9999;	//default port
logger_t *logger;

agent_t *agent_connections[20];	//need to be change..todo
int counter = 0;
int agent_num = 0;

pos_calc_t *info_for_calculation;

blist_t *list;	// blist.. holds all the information

int main(int argc, char **argv)
{
	char hostname[100];
	int connfd = 0;
	int listenfd = start_server_thread();

	load_commandline_arg(argc, argv);

	//get agent information from list_agent.txt
	param_slist_t *agent_infos = (param_slist_t*) malloc(sizeof(param_slist_t));
	agent_num = get_agent_count(agent_infos);

	agent_info_t infos[agent_num];		//to be used later in calculation

	load_agent_infos(infos, agent_infos);
	free_param_slist(agent_infos);

	//install a signal handler here
	if (signal(SIGINT, SIGINT_Handler) == SIG_ERR) {
			printf("SIGINT install error\n");
			exit(1);
    }

    logger = (logger_t*) malloc(sizeof(logger_t));
    init_logger(logger, 'w', "log.txt");

    list = (blist_t *) malloc(sizeof(blist_t));
    init_blist(list, 10);

    printf("[INFO] Successfully created logger\n");
	gethostname(hostname, sizeof(hostname));
	printf("[INFO] Local Machine Hostname: %s\n", hostname);

	start_calculation_thread();

	while(1)
	{
		printf("[INFO] Listening on port %d...\n", port);
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
	
	int i = 0;
	for(;i<counter; i++)
	{
		printf("%d\n", i);
		close(agent_connections[i]->con_fd);
		pthread_cancel(*agent_connections[i]->tid);

		free(agent_connections[i]->tid);
		free(agent_connections[i]);
	}

	shutdown(listenfd, SHUT_RDWR);
	close(listenfd);
	printf("[INFO] All sockets were shut down.\n");
	pthread_mutex_unlock(&logger->lock);
	pthread_mutex_unlock(&list->lock);

	free(info_for_calculation);
	free_logger(logger);
	printf("[INFO] Logger is freed.\n");
	print_blist(list);
	free_blist(list);
	printf("[INFO] Blist is freed.\n");
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

void start_calculation_thread()
{
	pthread_t tid;

	printf("[INFO] Total # of agents is %d.\n",agent_num);

	info_for_calculation = (pos_calc_t *) malloc(sizeof(pos_calc_t));
	info_for_calculation->list = list;
	info_for_calculation->agent_num = agent_num;

	pthread_create(&tid, NULL, pos_generation, (void*)info_for_calculation);

	printf("[INFO] Calcultion thread started\n");
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
	albl.agent = agent;
	albl.logger = logger;
	albl.list = list;

	agent_connections[counter] = agent;
	counter+=1;	
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

