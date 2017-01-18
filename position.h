#include "agentHandler.h"
#include "headers/agentInfo.h"


//store all info that is needed to calculate postions
typedef struct pos_calc
{
	blist_t *list;
	int agent_num;	//total number of agents... from config.h
	agent_info_t *infos;
}pos_calc_t;


int get_room_num_from_mac(agent_info_t infos[], char *mac, int agent_num)
{
	int i = 0;

	while(i < agent_num)
	{
		if(strcmp(infos[i].mac, mac) == 0)
		{
			return infos[i].room_id;
		}
		i++;
	}
	//just exit the program if there is something wrong
	printf("[ERROR] Cannot find the agent info(%s) from configs. Something wrong..\n", mac);
	raise(SIGINT);
	return -1;
}


int get_room_num(rssi_pair_t pairs[], agent_info_t infos[], int agent_num)
{
	int i;
	int max_index, second_max_index, third_max_index;
	int roomid1, roomid2, roomid3;
	if(agent_num < 3) return -1;

	//sort the first 3..
	max_index = pairs[0].rssi > pairs[1].rssi ? 0 :1;
	second_max_index = max_index == 0? 1: 0;
	if(pairs[2].rssi > pairs[second_max_index].rssi)
	{
		third_max_index = 2;
	}else if(pairs[2].rssi > pairs[max_index].rssi)
	{
		third_max_index = second_max_index;
		second_max_index = 2;
	}else{
		third_max_index = second_max_index;
		second_max_index = max_index;
		max_index = 2;
	}
	i = 3;
	printf("iasdui\n");
	while(i<agent_num)
	{
		if(pairs[i].rssi >= 0){
			i++;
			continue;
		}
		if(pairs[i].rssi > pairs[max_index].rssi)
		{
			third_max_index = second_max_index;
			second_max_index = max_index;
			max_index = i;
		}else if(pairs[i].rssi > pairs[second_max_index].rssi)
		{
			third_max_index = second_max_index;
			second_max_index = i;
		}else if(pairs[i].rssi > pairs[third_max_index].rssi)
		{
			third_max_index = i;
		}
		i++;
	}
	printf("%s..%s...%s\n", pairs[max_index].mac, pairs[second_max_index].mac, pairs[third_max_index].mac);
	roomid1 = get_room_num_from_mac(infos, pairs[max_index].mac, agent_num);
	roomid2 = get_room_num_from_mac(infos, pairs[second_max_index].mac, agent_num);
	roomid3 = get_room_num_from_mac(infos, pairs[third_max_index].mac, agent_num);
	printf("%d..%d...%d\n", roomid1, roomid2, roomid3);
	if(roomid1 == roomid2) return roomid1;
	if(roomid1 == roomid3) return roomid1;
	if(roomid2 == roomid3) return roomid3;

	return -1;
}


void calc_all_beacon_pos(blist_t *list, int agent_num, agent_info_t infos[])
{
	int8_t i = 0;
	int8_t room = -1;
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
			room = get_room_num(rssi_pairs, infos, agent_num);
			printf("In room: %d\n", room);
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
	agent_info_t *infos;

	int agent_num = 0;

	if(arg == NULL){
		printf("[ERROR] NULL BLIST!!!\n");
		return NULL;
	}

	info = (pos_calc_t *)arg;
	list = info->list;
	agent_num = info->agent_num;
	infos = info->infos;

	while(1)
	{
		sleep(1);
		calc_all_beacon_pos(list, agent_num, infos);
	}
}
