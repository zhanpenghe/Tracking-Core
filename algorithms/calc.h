#include "../headers/agentInfo.h"


typedef struct calc_prep
{
	agent_info_t *infos;
	int info_indexes[3];
	int rssi[3];
	int room;
}calc_prep_t;

void print_prep(calc_prep_t *p)
{
	int i = 0;
	if(p == NULL) return;

	printf("Printing prep:\nRoom: %d.\n", p->room);
	while(i<3)
	{
		printf("mac: %s\trssi: %d\n", p->infos[p->info_indexes[i]].mac, p->rssi[i]);
		i++;
	}
}



