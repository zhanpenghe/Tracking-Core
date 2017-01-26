#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "../algorithms/calc.h"

int agent_num = 0;

void main(){
	int i = 0;
	//get agent information from list_agent.txt
	param_slist_t *agent_infos_p = (param_slist_t*) malloc(sizeof(param_slist_t));
	agent_num = get_agent_count(agent_infos_p);

	agent_info_t agent_infos[agent_num];

	load_agent_infos(agent_infos, agent_infos_p);
	free_param_slist(agent_infos_p);

	for(;i<agent_num;i++)
		printf("%s\n", agent_infos[i].mac);

	calc_prep_t prep;

	prep.infos = agent_infos;
	prep.info_indexes[0] = 3;
	prep.info_indexes[1] = 4;
	prep.info_indexes[2] = 5;
	prep.rssi[0] = -53;
	prep.rssi[1] = -79;
	prep.rssi[2] = -83;
	prep.room = 2;

	pos_t pos;

	calculate(&prep, &pos);
	printf("%f....%f\n", pos.loc.x, pos.loc.y);
}