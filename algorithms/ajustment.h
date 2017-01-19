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

void zone_adjust(pos_t *pos)
{
	if(pos == NULL) return;

	if(pos->room == 1){
		if(pos->loc.x <= 370 && pos->loc.y <= 220){
			pos->loc.x = 200;
			pos->loc.y = 160;
		}else if(pos->loc.x <= 370 && pos->loc.y >= 220){
			pos->loc.x = 200;
			pos->loc.y = 260;
		}else if(pos->loc.x > 350){
			pos->loc.x = 450;
			pos->loc.y = 229;
		}else{
			pos->loc.x = 247;
			pos->loc.y = 184;
		}
	}else if(pos->room == 2){
		if(pos->loc.x <= 900){
			pos->loc.x = 800;
			pos->loc.y = 200;
		}else{
			pos->loc.x = 1050;
			pos->loc.x = 200;
		}
	}else if(pos->room == 3){
		if(pos->loc.x <= 1100){
			pos->loc.x = 950;
			pos->loc.y = 430;
		}else{
			pos->loc.x = 1253;
			pos->loc.x = 430;
		}
	}else if(pos->room == 4){
		if(pos->loc.x <= 1350){
			pos->loc.x = 1250;
			pos->loc.y = 650;
		}else if(pos->loc.x > 1359 && pos->loc.y <= 620){
			pos->loc.x = 1500;
			pos->loc.y = 490;
		}else{
			pos->loc.x = 1500;
			pos->loc.x = 730;
		}
	}else if(pos->room == 5){
		if(pos->loc.x <= 1950 && pos->loc.y <= 630){
			pos->loc.x = 1760;
			pos->loc.y = 490;
		}else if(pos->loc.x > 1950 && pos->loc.y >= 630){
			pos->loc.x = 2000;
			pos->loc.y = 490;
		}else if(pos->loc.x <= 1950 && pos->loc.y > 630){
			pos->loc.x = 1760;
			pos->loc.y = 730;
		}else{
			pos->loc.x = 2000;
			pos->loc.y = 730;
		}
	}
}

void adjust(pos_t *pos, float a, float b, float c, float d)
{
	if(pos == NULL) return;

	pos->loc.x = a*pos->loc.x+b;
	pos->loc.y = c*pos->loc.y+b;
}