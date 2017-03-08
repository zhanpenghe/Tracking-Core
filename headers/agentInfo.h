#include <stdlib.h>
#include <stdio.h>
#include "roomInfo.h"

typedef struct agent_info
{
	int room_id;
	char mac[18];
	char id[20];
	int x;
	int y;
}agent_info_t;

int get_agent_count(param_slist_t *list)
{
	int result = 0;
	init_param_slist(list);
	read_param_file("./parameters/list_agent.txt", list);
	result = list->size;
	return result;
}

void load_agent_infos(agent_info_t infos[], param_slist_t *list)
{
	param_s_t *curr = NULL;
	int i = 0, size = list->size;

	while((curr = next_param_s(list))!=NULL && i<size)
	{
		strcpy(infos[i].id, curr->key);

		strncpy(infos[i].mac, next_val(curr)->val, 17);
		infos[i].mac[17] = 0;

		infos[i].x = atoi(next_val(curr)->val);
		infos[i].y = atoi(next_val(curr)->val);

		infos[i].room_id = atoi(next_val(curr)->val);
		i++;
	}
}

char* get_mac_by_id(agent_info_t infos[], int size, char *id)
{
	int i=0;
	if(id == NULL) return NULL;
	while(i<size)
	{
		if(strcmp(infos[i].id, id)==0) return infos[i].mac;
		i++;
	}
	return NULL;
}