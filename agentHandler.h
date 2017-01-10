#include <stdio.h>
#include <pthread.h>

#include "utils/logger.h"

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


void setAgent(agent_t* agent, int con_fd, agent_t* next)
{
	if(agent == NULL) return;
	agent->con_fd = con_fd;
	agent->next = next;
	agent->tid = NULL;
}

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

