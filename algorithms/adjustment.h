#include "../headers/Position_list.h"

void bound(pos_t *pos)
{
	if(pos == NULL) return;

	if(pos->room == 1){
		if(pos->loc.x < 110) pos->loc.x = 110;
		if(pos->loc.x > 681) pos->loc.x = 681;
		if(pos->loc.y < 60) pos->loc.y = 60;
		if(pos->loc.x < 526){
			if(pos->loc.y > 373) pos->loc.y = 373;
		}else{
			if(pos->loc.y > 328) pos->loc.y = 328;
		}
	}else if(pos->room == 2)
	{
		if(pos->loc.x < 723) pos->loc.x = 723;
		if(pos->loc.x > 1293) pos->loc.x = 1293;
		if(pos->loc.y < 60) pos->loc.y = 60;
		if(pos->loc.x < 859){
			if(pos->loc.y > 328) pos->loc.y = 328;
		}else{
			if(pos->loc.y > 302) pos->loc.y = 302;
		}
	}else if(pos->room == 3)
	{
		if(pos->loc.x < 879) pos->loc.x = 879;
		if(pos->loc.x > 1450) pos->loc.x = 1450;
		if(pos->loc.y < 524) pos->loc.y = 524;
		if(pos->loc.x < 1303){
			if(pos->loc.y > 332) pos->loc.y = 332;
		}else{
			if(pos->loc.y > 168) pos->loc.y = 168;
		}
	}else if(pos->room == 4)
	{
		if(pos->loc.x < 1167) pos->loc.x = 1167;
		if(pos->loc.x > 2175) pos->loc.x = 2175;
		if(pos->loc.y < 385) pos->loc.y = 385;
		if(pos->loc.y>845) pos->loc.y = 845;
	}
	
	pos->loc.x -= 50;
}

void adjust(pos_t *pos, float a, float b, float c, float d)
{
	if(pos == NULL) return;

	pos->loc.x = (pos->loc.x-a)*b;
	pos->loc.y = (pos->loc.y-c)*d;
}