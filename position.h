#include "agentHandler.h"
#include "algorithms/calc.h"

//store all info that is needed to calculate postions
typedef struct pos_calc
{
	blist_t *list;
	int agent_num;	//total number of agents... from config.h
	room_info_t *room_infos;
	agent_info_t *infos;
	pos_list_t *pos_list;
}pos_calc_t;

int get_info_from_room(agent_info_t infos[], int room,int agent_num, int found1, int found2)
{
	int i = 0;

	while(i < agent_num)
	{
		if(infos[i].room_id == room && i != found1 && i != found2) return i;
		i++;
	}
	//just exit the program if there is something wrong
	printf("[ERROR] Cannot find the agent info(room: %d with found:[%s, %s]) from configs. Something wrong..\n", room,infos[found1].mac, infos[found2].mac);
	raise(SIGINT);
	return -1;
}

int get_info_from_mac(agent_info_t infos[], char *mac, int agent_num)
{
	int i = 0;

	while(i < agent_num)
	{
		if(strcmp(infos[i].mac, mac) == 0) return i;
		i++;
	}
	//just exit the program if there is something wrong
	printf("[ERROR] Cannot find the agent info(%s) from configs. Something wrong..\n", mac);
	raise(SIGINT);
	return -1;
}

//this function is very sloppy because of the data structure.. need some hashmap implementation to improve the whole thing...
int get_room_num(rssi_pair_t pairs[], agent_info_t infos[], int agent_num, calc_prep_t *prep)
{
	int i;
	int max_index, second_max_index, third_max_index;
	int info1, info2, info3;
	if(agent_num < 3) return -1;

	//sort the first 3..
	max_index = pairs[0].rssi > pairs[1].rssi ? 0 :1;
	second_max_index = max_index == 0? 1: 0;
	if(pairs[2].rssi > pairs[second_max_index].rssi){
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
	info1 = get_info_from_mac(infos, pairs[max_index].mac, agent_num);
	info2 = get_info_from_mac(infos, pairs[second_max_index].mac, agent_num);
	info3 = get_info_from_mac(infos, pairs[third_max_index].mac, agent_num);

	// fill all the information needed to compute the position
	if(infos[info1].room_id == infos[info2].room_id){
		prep->room = infos[info1].room_id;
		prep->info_indexes[0] = info1;
		prep->info_indexes[1] = info2;
		prep->rssi[0] = pairs[max_index].rssi;
		prep->rssi[1] = pairs[second_max_index].rssi;

		prep->info_indexes[2] = get_info_from_room(infos, prep->room, agent_num,  info1, info2);
		i = 0;
		for(;i<agent_num; i++){
			if(strcmp(pairs[i].mac, infos[prep->info_indexes[2]].mac)==0){
				prep->rssi[2] = pairs[i].rssi;
				return prep->room;
			}
		}
		return -1;
	}
	if(infos[info1].room_id == infos[info3].room_id){
		prep->room = infos[info1].room_id;
		prep->info_indexes[0] = info1;
		prep->info_indexes[1] = info3;
		prep->rssi[0] = pairs[max_index].rssi;
		prep->rssi[1] = pairs[third_max_index].rssi;

		prep->info_indexes[2] = get_info_from_room(infos, prep->room, agent_num, info1, info3);
		i = 0;
		for(;i<agent_num; i++){
			if(strcmp(pairs[i].mac, infos[prep->info_indexes[2]].mac)==0){
				prep->rssi[2] = pairs[i].rssi;
				return prep->room;
			}
		}
		return -1;
	}
	if(infos[info2].room_id == infos[info3].room_id){
		prep->room = infos[info1].room_id;
		prep->info_indexes[0] = info2;
		prep->info_indexes[1] = info3;
		prep->rssi[0] = pairs[second_max_index].rssi;
		prep->rssi[1] = pairs[third_max_index].rssi;

		prep->info_indexes[2] = get_info_from_room(infos, prep->room, agent_num, info2, info3);
		i = 0;
		for(;i<agent_num; i++){
			if(strcmp(pairs[i].mac, infos[prep->info_indexes[2]].mac)==0){
				prep->rssi[2] = pairs[i].rssi;
				return prep->room;
			}
		}
		return -1;
	}
	return -1;
}

void calc_all_beacon_pos(blist_t *list, int agent_num, agent_info_t infos[], room_info_t room_infos[], pos_list_t *pos_list)
{
	int8_t i = 0;
	int8_t room = -1, last_room = -1;
	beacon_t *curr;
	rssi_pair_t rssi_pairs[agent_num];
	pos_t *pos;

	calc_prep_t prep;
	prep.infos = infos;
	
	pthread_mutex_lock(&list->lock);
	printf("[INFO] Calcluating position. The blist is locked here.\n");
	curr = list->head;
	while(curr != NULL)
	{
		//calc position here
		if(curr->size >= 3){
			get_rssi_for_calc(curr, rssi_pairs, agent_num);
			/*printf("\nCurrent beacon:\n");
			print_beacon(curr);
			printf("Data get from beacon:\n");
			for(i=0; i<agent_num; i++)
			{
				if(rssi_pairs[i].rssi == 0) continue;
				printf("%s: %d\n", rssi_pairs[i].mac, rssi_pairs[i].rssi);
			}*/
			room = get_room_num(rssi_pairs, infos, agent_num, &prep);
			if(room == -1) printf("SOMETHING WRONG WITH ROOM CALC\n");
			else{// calculate position
				print_prep(&prep);
				pos = (pos_t *) malloc(sizeof(pos_t));
				calculate(&prep, pos);
				adjust(pos, room_infos[room].a, room_infos[room].b, room_infos[room].c, room_infos[room].d);
				zone_adjust(pos);
				printf("DONE WITH CALC\n");
				pthread_mutex_lock(&pos_list->lock);
				add_pos_to_list(pos_list, pos, curr->mac, 10);
				pthread_mutex_unlock(&pos_list->lock);
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
	agent_info_t *infos;
	room_info_t *room_infos;
	pos_list_t *pos_list;

	int agent_num = 0;

	if(arg == NULL){
		printf("[ERROR] NULL BLIST!!!\n");
		return NULL;
	}

	info = (pos_calc_t *)arg;

	list = info->list;
	agent_num = info->agent_num;
	infos = info->infos;
	room_infos = info->room_infos;
	pos_list = info->pos_list;

	if(list == NULL || infos == NULL || room_infos == NULL)
	{
		printf("ERROR WHEN INITIATING THE POS CALCULATION THREAD.\n");
		raise(SIGINT);
		return NULL;
	}

	while(1)
	{
		sleep(1);
		calc_all_beacon_pos(list, agent_num, infos, room_infos, pos_list);
	}
}
