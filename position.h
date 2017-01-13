#include "agentHandler.h"

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
		if(curr->size >= 3){
			get_rssi_for_calc(curr, rssi_pairs, agent_num);
			printf("\nCurrent beacon:\n");
			print_beacon(curr);
			printf("Data get from beacon:\n");
			for(i=0; i<agent_num; i++)
			{
				if(rssi_pairs[i].rssi == 0) continue;
				printf("%s: %d\n", rssi_pairs[i].mac, rssi_pairs[i].rssi);
			}
		}else{
			printf("Too less info for position calculation. (%s, %d)\n", curr->mac, curr->size);
		}
		curr = curr->next;
	}
	printf("[INFO] Done with calculation. Unlocking blist.\n");
	pthread_mutex_unlock(&list->lock);

}

void *pos_generation(void *arg)
{
	pos_calc_t *info;
	blist_t *list;

	int agent_num = 0;

	if(arg == NULL){
		printf("[ERROR] NULL BLIST!!!\n");
		return NULL;
	}

	info = (pos_calc_t *)arg;
	list = info->list;
	agent_num = info->agent_num;

	while(1)
	{
		sleep(1);
		calc_all_beacon_pos(list, agent_num);
	}
}
