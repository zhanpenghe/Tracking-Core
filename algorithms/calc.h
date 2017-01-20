#include "../headers/agentInfo.h"
#include "ajustment.h"

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
	float PN = 3.5, PA = -47;
	point_t p0, p1, p2, p01, p02, p12;
	line_t l01, l02, l12;
	if(prep == NULL || result == NULL) return;

	// init everything to calculate
	set_point(&p0, (int)prep->infos[prep->info_indexes[0]].x, (int)prep->infos[prep->info_indexes[0]].y);
	set_point(&p1, (int)prep->infos[prep->info_indexes[1]].x, (int)prep->infos[prep->info_indexes[1]].y);
	set_point(&p2, (int)prep->infos[prep->info_indexes[2]].x, (int)prep->infos[prep->info_indexes[2]].y);

	d0 = rssi_to_distance(prep->rssi[0], PA, PN);
	d1 = rssi_to_distance(prep->rssi[1], PA, PN);
	d2 = rssi_to_distance(prep->rssi[2], PA, PN);

	printf("d: %f..%f..%f\n", d0, d1, d2);

	get_point_by_ratio(&p0, &p1, &p01, d0, d1);
	get_point_by_ratio(&p0, &p2, &p02, d0, d2);
	get_point_by_ratio(&p1, &p2, &p12, d1, d2);

	get_line_by_slope_point(&p01, get_perpendicular_slope(get_slope(&p0, &p1)), &l01);
	get_line_by_slope_point(&p02, get_perpendicular_slope(get_slope(&p0, &p2)), &l02);
	get_line_by_slope_point(&p12, get_perpendicular_slope(get_slope(&p1, &p2)), &l12);

	printf("slope: %f, intersect: %f\n", l01.slope, l01.y_int);
	printf("slope: %f, intersect: %f\n", l02.slope, l02.y_int);
	printf("slope: %f, intersect: %f\n", l12.slope, l12.y_int);

	find_interception(&l01, &l02, &p0);
	printf("x1:%fy1:%f\n", p0.x, p0.y);
	find_interception(&l01, &l12, &p1);
	printf("x2:%fy2:%f\n", p1.x, p1.y);
	find_interception(&l02, &l12, &p2);
	printf("x3:%fy3:%f\n", p2.x, p2.y);

	result->loc.x = (p0.x+p1.x+p2.x)/3.0;
	result->loc.y = (p0.y+p1.y+p2.y)/3.0;
}


