
#include "headers/Rssi_Queue.h"

//store all info that is needed to calculate postions
typedef struct pos_calc
{
	blist_t *list;
	int agent_num;	//total number of agents... from config.h
}pos_calc_t;

void calc_all_beacon_pos(blist_t *list, int agent_num)
{
	int8_t i = 0;
	beacon_t *curr;
	rssi_pair_t rssi_pairs[agent_num];

	pthread_mutex_lock(&list->lock);
	printf("[INFO] Calcluating position. The blist is locked here.\n");
	curr = list->head;
	while(curr != NULL)
	{
		//calc position here
		curr = curr->next;
	}
	printf("[INFO] Done with calculation. Unlocking blist.\n");
	pthread_mutex_unlock(&list->lock);

}

void pos_generation(void *arg)
{
	pos_calc_t *info;
	blist_t *list;

	int agent_num = 0;

	if(arg == NULL){
		printf("[ERROR] NULL BLIST!!!\n");
		return;
	}

	info = (pos_calc_t *)arg;
	list = info->list;

	while(1)
	{
		calc_all_beacon_pos(list, agent_num);
	}
}
