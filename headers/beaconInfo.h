#include "agentInfo.h"

typedef struct beacon_info
{
	char id[64];
	char mac[18];
}beacon_info_t;

int get_beacon_count(param_slist_t *list)
{
	int result = 0;
	init_param_slist(list);
	read_param_file("parameters/list_beacon.txt", list);
	result = list->size;
	return result;
}

void load_beacon_infos(beacon_info_t infos[], param_slist_t *list)
{
	param_s_t *curr = NULL;
	int i = 0, size = list->size;

	while((curr = next_param_s(list))!=NULL && i<size)
	{
		strncpy(infos[i].id, curr->key, strlen(curr->key));
		infos[i].id[strlen(curr->key)] = 0;

		strncpy(infos[i].mac, next_val(curr)->val, 17);
		infos[i].mac[17] = 0;

		next_val(curr);

		i++;
	}
}