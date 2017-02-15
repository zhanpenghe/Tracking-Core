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

typedef struct agent_list
{
	agent_t *head;
	agent_t *tail;
	int size;
	pthread_mutex_t lock;
}agent_list_t;

//agent, Logger, Blist
typedef struct ALBL
{
	agent_t *agent;
	logger_t *logger;
	blist_t *list;
	agent_list_t *agent_list;
	info_for_calc_t *infos_for_calc;
}agent_logger_blist_t;

void setAgent(agent_t* agent, int con_fd, agent_t* next)
{
	if(agent == NULL) return;
	agent->con_fd = con_fd;
	agent->next = next;
	agent->tid = NULL;
}

void free_agent(agent_t *agent)
{
	if(agent == NULL) return;

	if(agent->tid != NULL) free(agent->tid);
	free(agent);
}

void init_agent_list(agent_list_t *list)
{
	if(list == NULL) return;
	list->head = NULL;
	list->tail = NULL;
	list->size = 0;
	if(pthread_mutex_init(&list->lock, NULL) != 0)
	{
		printf("[ERROR] Agent list mutex init failed\n");
		exit(1);
	}
}

void free_agent_list(agent_list_t *list)
{
	agent_t *curr, *next;
	int i =0;
	if(list == NULL) return;

	curr = list->head;
	while(curr != NULL){
		next = curr->next;
		close(curr->con_fd);
		pthread_cancel(*curr->tid);
		free_agent(curr);
		curr = next;
		printf("%d\n", i);
		i++;
	}
	free(list);
}

void print_ag_list(agent_list_t *list)
{
	agent_t *curr;
	int i =0;
	if(list == NULL) return;

	curr = list->head;
	while(curr != NULL){
		printf("%d %d\n", i, curr->con_fd);
		curr = curr->next;
		i++;
		sleep(1);
	}
}

void add_new_agent_connection(agent_list_t *list, agent_t *agent)
{
	if(list == NULL || agent == NULL) return;

	pthread_mutex_lock(&list->lock);

	agent->next = NULL;
	if(list->head == NULL)
	{
		list->head = agent;
		list->tail = agent;
		list->size = 1;
		pthread_mutex_unlock(&list->lock);
		return;
	}

	list->tail->next = agent;
	list->tail = agent;
	list->size+=1;
	pthread_mutex_unlock(&list->lock);

}

void remove_agent_connection(agent_list_t *list, agent_t *agent)
{
	agent_t *curr, *next, *prev = NULL;
	if(list == NULL || agent == NULL) return;

	pthread_mutex_lock(&list->lock);

	curr = list->head;
	while(curr != NULL){
		next = curr->next;

		if(curr == agent)
		{
			printf("found\n");
			if(curr == list->head)
			{
				printf("head\n");
				//remove the head
				list->head = list->head->next;
				list->size -= 1;
			}
			else if(curr == list->tail)
			{
				printf("tail\n");

				prev->next = NULL;
				list->size -= 1;
				list->tail = prev;
			}
			else if(prev != NULL)
			{
				printf("middle\n");
				prev->next = curr->next;
				list->size -= 1;
			}else{
				printf("WIERD THING HAPPENED WHEN REMOVING AN AGENT CONNECTION\n");
			}
			free_agent(curr);
			return;
		}
		prev = curr;
		curr = next;
	}
	pthread_mutex_unlock(&list->lock);

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

static inline double
microtime_calc (struct timeval *tv)
{
    return tv->tv_sec + (tv->tv_usec / 1000000.0);
}

static inline double
microtime (void)
{
    struct timeval tv;

    if (!gettimeofday (&tv, NULL))
        return microtime_calc (&tv);
    else
        return 0;
}

//only log the measurement
void* logRSSIFromAgent(void *arg)
{
	int len, offset = 0;
	char buf[1025];
	char buf2[1050];
	char ret[] = {'z', '\0'}; 	//message to confirm the data arrival
	char *line;
	double current_time;
	char ctimestr[20];

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
		offset = 0;

		line = strtok(buf,"\n");
		strncpy(buf2, line, strlen(line));
		offset+=(int) strlen(line);
		buf2[offset] = '|';
		offset++;

		current_time = microtime();
		snprintf(ctimestr, 20, "%6.4f", current_time);
		strncpy(buf2+offset, ctimestr, strlen(ctimestr));
		offset+=(int)strlen(ctimestr);

		buf2[offset] = '\n';
		offset++;

		while((line = strtok(NULL, "\n"))!=NULL)
		{
			strncpy(buf2+offset, line, strlen(line));
			offset+=(int) strlen(line);
			buf2[offset] = '\n';
			offset++;
		}
		
		pthread_mutex_lock(&logger->lock);
		log_to_file(logger, buf2, offset);
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
	agent_list_t *agent_list = ((agent_logger_blist_t *) arg)->agent_list;
	info_for_calc_t *infos = ((agent_logger_blist_t *) arg)->infos_for_calc;

	pthread_t temp = pthread_self();

	//set the thread id first;
	agent->tid = malloc(sizeof(pthread_t));
	memcpy(agent->tid, &temp, sizeof(pthread_t));

	add_new_agent_connection(agent_list, agent);

	memset(buf, '0', sizeof(buf));

	while((len = read(agent->con_fd, buf, sizeof(buf)-1))>0)
	{
		printf("get info of len: %d\n", len);
		buf[len] = 0;
		//this need to be changed later.... todo
		pthread_mutex_lock(&logger->lock);
		log_to_file(logger, buf, len);
		log_to_file(logger, "\n", 1);
		pthread_mutex_unlock(&logger->lock);
		pthread_mutex_lock(&list->lock);
		printf("=========================================================\n");
		printf("--\n%s\n--\n", buf);
		store_rssi_from_agent(list, buf, infos);
		pthread_mutex_unlock(&list->lock);

		write(agent->con_fd, ret, 2);
	}

	printf("[INFO] An agent was disconnected... Now delete it from the list. \n");
	remove_agent_connection(agent_list, agent);
}

