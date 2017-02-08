#include "../headers/beaconInfo.h"
#include "../headers/server.h"
#include "adjustment.h"

typedef struct calc_prep
{
	agent_info_t *infos;
	int info_indexes[3];
	int rssi[3];
	int room;
}calc_prep_t;

typedef struct rssi_pair{
	int rssi;
	char mac[18];	//agent mac addr
}rssi_pair_t;

void print_prep(calc_prep_t *p)
{
	int i = 0;
	if(p == NULL) return;

	printf("Printing prep:\nRoom: %d.\n", p->room);
	while(i<3)
	{
		printf("ROOM_INFO #%d: %s\n", i, p->infos[p->info_indexes[i]].mac);
		printf("mac: %s\trssi: %d\n", p->infos[p->info_indexes[i]].mac, p->rssi[i]);
		i++;
	}
}

/*
 * A function to derive postion coordinate from RSSI's.
 * All algorithm change will be made here.
 *
 * @Param calc_prep_t *prep: A pointer pointing to information needed for the algorithm.
 * @Param pos_list_t *list: A list that store all the position results.
 *
 */
void calculate(calc_prep_t *prep, pos_t *result)
{

	float d0, d1, d2;
	float PN = get_PN(), PA = get_PA();
	point_t p0, p1, p2, p01, p02, p12;
	line_t l01, l02, l12;
	if(prep == NULL || result == NULL) return;

	result->room = prep->room;
	// init everything to calculate
	set_point(&p0, (int)prep->infos[prep->info_indexes[0]].x, (int)prep->infos[prep->info_indexes[0]].y);
	set_point(&p1, (int)prep->infos[prep->info_indexes[1]].x, (int)prep->infos[prep->info_indexes[1]].y);
	set_point(&p2, (int)prep->infos[prep->info_indexes[2]].x, (int)prep->infos[prep->info_indexes[2]].y);

	d0 = rssi_to_distance(prep->rssi[0], PA, PN);
	d1 = rssi_to_distance(prep->rssi[1], PA, PN);
	d2 = rssi_to_distance(prep->rssi[2], PA, PN);

	//printf("d: %f..%f..%f\n", d0, d1, d2);

	get_point_by_ratio(&p0, &p1, &p01, d0, d1);
	get_point_by_ratio(&p0, &p2, &p02, d0, d2);
	get_point_by_ratio(&p1, &p2, &p12, d1, d2);

	get_line_by_slope_point(&p01, get_perpendicular_slope(get_slope(&p0, &p1)), &l01);
	get_line_by_slope_point(&p02, get_perpendicular_slope(get_slope(&p0, &p2)), &l02);
	get_line_by_slope_point(&p12, get_perpendicular_slope(get_slope(&p1, &p2)), &l12);

	//printf("slope: %f, intersect: %f\n", l01.slope, l01.y_int);
	//printf("slope: %f, intersect: %f\n", l02.slope, l02.y_int);
	//printf("slope: %f, intersect: %f\n", l12.slope, l12.y_int);

	find_interception(&l01, &l02, &p0);
	//printf("x1:%fy1:%f\n", p0.x, p0.y);
	find_interception(&l01, &l12, &p1);
	//printf("x2:%fy2:%f\n", p1.x, p1.y);
	find_interception(&l02, &l12, &p2);
	//printf("x3:%fy3:%f\n", p2.x, p2.y);

	result->loc.x = (p0.x+p1.x+p2.x)/3.0;
	result->loc.y = (p0.y+p1.y+p2.y)/3.0;
}


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
	else if(pairs[0].rssi>=0) return -1;

	//sort the first 3..
	max_index = pairs[0].rssi > pairs[1].rssi ? 0 :1;
	second_max_index = max_index == 0? 1: 0;
	
	if(pairs[2].rssi < pairs[second_max_index].rssi){
		third_max_index = 2;
	}else if(pairs[2].rssi > pairs[max_index].rssi)
	{
		third_max_index = second_max_index;
		second_max_index = max_index;
		max_index = 2;
	}else{
		third_max_index = second_max_index;
		second_max_index = 2;
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
	printf("%d..%d..%d\n", infos[info1].room_id, infos[info2].room_id, infos[info3].room_id);
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
		prep->room = infos[info2].room_id;
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