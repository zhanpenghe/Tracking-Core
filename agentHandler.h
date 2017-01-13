#include <stdio.h>

#include "utils/logger.h"
#include "headers/Rssi_Queue.h"

typedef struct agent{
	pthread_t *tid;
	int con_fd;	//file descriptor of the socket
	struct agent *next;
}agent_t;

typedef struct agentAndLogger
{
	agent_t *agent;
	logger_t *logger;
}agent_and_logger_t;

//agent, Logger, Blist
typedef struct ALBL
{
	agent_t *agent;
	logger_t *logger;
	blist_t *list;
}agent_logger_blist_t;


void setAgent(agent_t* agent, int con_fd, agent_t* next)
{
	if(agent == NULL) return;
	agent->con_fd = con_fd;
	agent->next = next;
	agent->tid = NULL;
}

//only print out the measurement to console
void* printRSSIFromAgent(void *arg)
{
	int len;
	char buf[1025];
	char ret[] = {'z', '\0'}; 	//message to confirm the data arrival

	printf("Start handling...\n");
	agent_t *agent = (agent_t *) arg;
	pthread_t temp = pthread_self();

	//set the thread id first;
	agent->tid = malloc(sizeof(pthread_t));
	memcpy(agent->tid, &temp, sizeof(pthread_t));

	memset(buf, '0', sizeof(buf));

	while((len = read(agent->con_fd, buf, sizeof(buf)-1))>0)
	{
		buf[len] = 0;
		printf("%s\n", buf);
		write(agent->con_fd, ret, 2);
	}
}

//only log the measurement
void* logRSSIFromAgent(void *arg)
{
	int len;
	char buf[1025];
	char ret[] = {'z', '\0'}; 	//message to confirm the data arrival

	printf("Start handling...\n");
	agent_t *agent = ((agent_and_logger_t *) arg)->agent;
	logger_t *logger = ((agent_and_logger_t *) arg)->logger;
	pthread_t temp = pthread_self();

	//set the thread id first;
	agent->tid = malloc(sizeof(pthread_t));
	memcpy(agent->tid, &temp, sizeof(pthread_t));

	memset(buf, '0', sizeof(buf));

	while((len = read(agent->con_fd, buf, sizeof(buf)-1))>0)
	{
		buf[len] = 0;
		pthread_mutex_lock(&logger->lock);
		log_to_file(logger, buf, len);
		log_to_file(logger, "\n", 1);
		pthread_mutex_unlock(&logger->lock);
		write(agent->con_fd, ret, 2);
	}
}

void* log_and_storeRSSIFromAgent(void *arg)
{
	int len;
	char buf[1025];
	char ret[] = {'z', '\0'}; 	//message to confirm the data arrival

	printf("Start handling...\n");
	agent_t *agent = ((agent_logger_blist_t *) arg)->agent;
	logger_t *logger = ((agent_logger_blist_t *) arg)->logger;
	blist_t *list = ((agent_logger_blist_t *) arg)->list;
	pthread_t temp = pthread_self();

	//set the thread id first;
	agent->tid = malloc(sizeof(pthread_t));
	memcpy(agent->tid, &temp, sizeof(pthread_t));

	memset(buf, '0', sizeof(buf));

	while((len = read(agent->con_fd, buf, sizeof(buf)-1))>0)
	{
		buf[len] = 0;
		//this need to be changed later.... todo
		pthread_mutex_lock(&logger->lock);
		log_to_file(logger, buf, len);
		log_to_file(logger, "\n", 1);
		pthread_mutex_unlock(&logger->lock);

		store_rssi_from_agent(list, buf);

		write(agent->con_fd, ret, 2);
	}

}

