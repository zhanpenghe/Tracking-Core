#include <stdlib.h>
#include <stdio.h>
#include "../utils/param_reader.h"

typedef struct room_info
{
	int room_id;
	float a;
	float b;
	float c;
	float d;
}room_info_t;

int get_room_count(param_slist_t *list)
{
	int result = 0;
	init_param_slist(list);
	read_param_file("./parameters/list_room.txt", list);
	result = list->size;
	return result;
}

void load_room_infos(room_info_t infos[], param_slist_t *list)
{
	param_s_t *curr = NULL;
	int i = 0, size = list->size;

	while((curr = next_param_s(list))!=NULL && i<size)
	{
		infos[i].room_id = atoi(curr->key);

		infos[i].a = atof(next_val(curr)->val);
		infos[i].b = atof(next_val(curr)->val);
		infos[i].c = atof(next_val(curr)->val);
		infos[i].d = atof(next_val(curr)->val);
		i++;
	}
}


