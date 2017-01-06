#include <stdio.h>
#include <pthread.h>


typedef struct agent{
	pthread_t *tid;
	int con_fd;	//file descriptor fo the socket
	struct agent *next;
}agent_t;

void setAgent(agent_t* agent, int con_fd, agent_t* next)
{
	if(agent == NULL) return;
	agent->con_fd = con_fd;
	agent->next = next;
	agent->tid = NULL;
}

void printRSSIFromAgent(agent_t *agent)
{
	int len;
	char buf[1025];
	char ret[] = {'z', '\0'}; 	//message to confirm the data arrival
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







