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

typedef struct circle{
	point_t center;
	float radius;
}circle_t;

void set_point(point_t *p, float x, float y)
{
	if(p == NULL) return;
	p->x = x;
	p->y = y;
}

void set_circle(circle_t *c, float x, float y, float radius)
{
	if(c == NULL) return;

	set_point(&c->center, x, y);
	c->radius = radius;
}

float get_distance(point_t *p1, point_t *p2)
{
	if(p1 == NULL || p2 == NULL) return NAN;// change to signal later... something wrong...

	return sqrt(pow((p1->x - p2->x), 2) + pow((p1->y - p2->y), 2));
}

float get_slope(point_t *p1, point_t *p2)
{
	if(p1 == NULL || p2 == NULL) return NAN;// change to signal later... something wrong...
	printf("(p2y(%f) - p1y(%f))/(p2x(%f)-p1x(%f)) = %f..\n",p2->y,p1->y,p2->x,p1->x,(p2->y - p1->y)/(p2->x - p1->x) );
	return (p2->y - p1->y)/(p2->x - p1->x);
}

void get_line_by_points(point_t *p1, point_t *p2, line_t *line)
{
	if(p1 == NULL || p2 == NULL || line == NULL) return;

	line->slope = (p2->y - p1->y)/(p2->x - p1->x);
	line->y_int = (p1->y - line->slope*p1->x);
}

void get_line_by_slope_point(point_t *p, float slope, line_t *line)
{
	if(p == NULL || line == NULL) return;

	line->slope = slope;
	line->y_int = (p->y - line->slope * p->x);
}

void find_interception(line_t *l1, line_t *l2, point_t *p)
{
	if(l1 == NULL || l2 == NULL || p==NULL) return;
	if(l1->slope == l2->slope) return;

	p->x = (l1->y_int - l2->y_int)/(l2->slope - l1->slope);
	p->y = (l2->slope*l1->y_int - l1->slope*l2->y_int)/(l2->slope-l1->slope);
}

float get_perpendicular_slope_of_line(line_t *l)
{
	if(l == NULL) return NAN;

	return (-1.0)/l->slope;
}

float get_perpendicular_slope(float slope)
{
	if(slope == 0) return NAN;

	return (-1.0)/slope;
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

float rssi_to_distance(int rssi, float PA, float PN)
{
	return powf(10, (PA-(float)rssi)/(10.0*PN));
}

/*
 * This function is not actually doing intersection counting.
 * It's only for the RTLS Algorithm.
 */
int intersection_count(circle_t *c1, circle_t *c2)
{
	if(c1 == NULL || c2 == NULL) return -1;
	if(c1->center.x == c2->center.x && c1->center.y == c2->center.y) return -1;

	float distance = get_distance(&c1->center, &c2->center);
	if(distance<c1->radius || distance<c2->radius) return -1;

	if(distance>(c1->radius+c2->radius)) return 0;
	else if(distance == (c1->radius+c2->radius)) return 1;
	else return 2;
}

/*
 * solve ax^2+bx+c=0
 */
void solve_quad(float a, float b, float c, float *s1, float *s2)
{
	if(b*b-4*a*c < 0){
		printf("test\n");
		*s1 = NAN;
		*s2 = NAN;
	}else{
		printf("a: %f, b: %f, c: %f\n", a, b, c);
		*s1 = ((-1*b)+sqrt(b*b - 4*a*c))/(2*a);
		*s2 = ((-1*b)-sqrt(b*b - 4*a*c))/(2*a);
	}
}

void get_intersections_of_a_circle_and_a_line(circle_t *c, line_t *l, point_t *p1, point_t *p2)
{
	if(c==NULL || l==NULL)
	{
		p1->x = NAN;
		p2->x = NAN;
		p1->y = NAN;
		p2->y = NAN;
		return;
	}

	float r = c->radius;
	float k = l->slope;
	float m = l->y_int;
	float a = c->center.x;
	float b = c->center.y;

	p1->x = (-1*sqrt(-1*a*a*k*k+2*a*b*k-2*a*k*m-b*b+2*b*m+k*k*r*r-m*m+r*r)+a+b*k-k*m)/(k*k+1);
	p1->y = k*p1->x+b;

	p2->x = (sqrt(-1*a*a*k*k+2*a*b*k-2*a*k*m-b*b+2*b*m+k*k*r*r-m*m+r*r)+a+b*k-k*m)/(k*k+1);
	p2->y = k*p2->x+b;

}

/*
 * Function to derive two intersections of two circles.
 * Only apply to circles that have two intersections.
 */
void get_intersections_of_circles(circle_t *c1, circle_t *c2, point_t *p1, point_t *p2)
{
	if(c1==NULL || c2==NULL) return;

	float d = get_distance(&c1->center, &c2->center);
	float r1 = c1->radius;
	float r2 = c2->radius;

	float r = (r2*r2-r1*r1-d*d)/(-2.0*d);
	printf("d: %f, d_per: %f\n", d, r);
	
	if(c1->center.x != c2->center.x && c1->center.y != c2->center.y){
		float k = get_slope(&c1->center, &c2->center);
		float a = c1->center.x;
		float b = c1->center.y;

		point_t temp1, temp2, per;

		temp1.x = (a*k*k+a-sqrt((k*k+1)*r*r))/(k*k+1);
		temp1.y = (b*k*k+b-k*sqrt((k*k+1)*r*r))/(k*k+1);

		temp2.x = (a*k*k+a+sqrt((k*k+1)*r*r))/(k*k+1);
		temp2.y = (b*k*k+b+k*sqrt((k*k+1)*r*r))/(k*k+1);
		//printf("temps: (%f, %f), (%f, %f)\n", temp1.x, temp1.y, temp2.x, temp2.y);
	
		float d1 = get_distance(&temp1, &c2->center);
		float d2 = get_distance(&temp2, &c2->center);

		if(d2>d1){
			per.x = temp1.x;
			per.y = temp1.y;
		}else{
			per.x = temp2.x;
			per.y = temp2.y;
		}
		//printf("%f, %f\n", d1, d2);
		line_t per_l;

		get_line_by_slope_point(&per, -1/k, &per_l);
		//printf("l: slope:%f, y_int:%f\n", per_l.slope, per_l.y_int);
		get_intersections_of_a_circle_and_a_line(c1,&per_l, p1, p2);
	}else if(c1->center.x == c2->center.x && c1->center.y != c2->center.y)
	{
		float y = 0;
		if(c1->center.y+d == c2->center.y)
		{
			y = c1->center.y+r;
		}else{
			y = c1->center.y-r;
		}

		p1->y = y;
		p2->y = y;
		p1->x = sqrt(c1->radius*c1->radius-(y-c1->center.y)*(y-c1->center.y))+c1->center.x;
		p2->x = -1*sqrt(c1->radius*c1->radius-(y-c1->center.y)*(y-c1->center.y))+c1->center.x;
	}else if(c1->center.x != c2->center.x && c1->center.y == c2->center.y)
	{
		float x = 0;
		if(c1->center.x+d == c2->center.x)
		{
			x = c1->center.x+r;
		}else{
			x = c1->center.x-r;
		}

		p1->x = x;
		p2->x = x;
		p1->y = sqrt(c1->radius*c1->radius-(x-c1->center.x)*(x-c1->center.x))+c1->center.y;
		p2->y = -1*sqrt(c1->radius*c1->radius-(x-c1->center.x)*(x-c1->center.x))+c1->center.y;
	}
}
