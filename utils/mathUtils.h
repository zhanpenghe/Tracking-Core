/*
 * Implemented several basic math operations for position calculation.
 * @Author Adam Ho
 */

#include <stdlib.h>
#include <math.h>

typedef struct line{

	float slope;
	float y_int;

}line_t;

typedef struct point
{
	float x;
	float y;
}point_t;

float get_distance(point_t *p1, point_t *p2)
{
	if(p1 == NULL || p2 == NULL) return 1;// change to signal later... something wrong...

	return sqrt(pow((p1->x - p2->x), 2) + pow((p1->y - p2->y), 2));
}

float get_slope(point_t *p1, point_t *p2)
{
	if(p1 == NULL || p2 == NULL) return 1;// change to signal later... something wrong...

	return (p2->y - p1->y)/(p2->x - p1->x);
}

void get_line(point_t *p1, point_t *p2, line_t *line)
{
	if(p1 == NULL || p2 == NULL || line == NULL) return;

	line->slope = (p2->y - p1->y)/(p2->x - p1->x);
	line->y_int = (p1->y - line->slope*p1->x);
}

void get_point_by_ratio(point_t *p1, point_t *p2, point_t *target, float d1, float d2)
{
	float r1, r2;
	if(p1 == NULL || p2 == NULL || target == NULL || d1 < 0 || d2 < 0) return;

	r1 = d1/(d1+d2);
	r2 = d2/(d1+d2); 
	target->x = r2*p1->x+r1*p2->x;
	target->y = r2*p1->y+r1*p2->y;
}

void find_interception(line_t *l1, line_t *l2, point_t *p)
{
	if(l1 == NULL || l2 == NULL || p==NULL) return;
	if(l1->slope == l2->slope) return;

	p->x = (l1->y_int - l2->y_int)/(l2->slope - l1->slope);
	p->y = (l2->slope*l1->y_int - l1->slope*l2->y_int)/(l2->slope-l1->slope);
}
