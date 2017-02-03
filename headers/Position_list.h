#include <stdlib.h>
#include <string.h>
#include "../utils/mathUtils.h"

typedef struct position
{
	int room;
	point_t loc;
	struct position *next;
}pos_t;

typedef struct b_pos{

	pos_t *head;
	pos_t *tail;
	char mac[18];
	struct b_pos *next;
	int size;
	int max;
}b_pos_t;

typedef struct pos_list{
	b_pos_t *head;
	b_pos_t *tail;
	pthread_mutex_t lock;
	int size;
}pos_list_t;

void bound(pos_t *pos);


/************************init and free*******************************/
void init_pos(pos_t *p, int x, int y, int room)
{
	if(p == NULL) return;

	p->loc.x = x;
	p->loc.y = y;
	p->room = room;
	p->next = NULL;
}

void free_pos(pos_t *p)
{
	if(p==NULL) return;
	free(p);
}

void init_b_pos(b_pos_t *b, char *mac, int max)
{
	if(b == NULL) return;
	b->head = NULL;
	b->tail = NULL;
	b->next = NULL;
	b->size = 0;
	strncpy(b->mac, mac, 17);
	b->mac[17] = 0;
	b->max = max;
}

void clear_b_pos(b_pos_t *b)
{
	pos_t *curr, *next;
	if(b == NULL) return;
	printf("clear..\n");
	curr = b->head;
	while(curr != NULL)
	{
		next = curr->next;
		free_pos(curr);
		curr = next;
	}
	b->size = 0;
	b->head = NULL;
	b->tail = NULL;
}

void free_b_pos(b_pos_t *b)
{
	if(b == NULL) return;

	clear_b_pos(b);

	free(b);
}

void init_pos_list(pos_list_t *list)
{
	if(list == NULL) return;

	list->head = NULL;
	list->tail = NULL;
	list->size = 0;
	if(pthread_mutex_init(&list->lock, NULL) != 0)
	{
		printf("[ERROR] POS_LIST mutex lock init failed\n");
		exit(1);
	}
}

void free_pos_list(pos_list_t *list)
{
	b_pos_t *curr, *next;
	if(list == NULL) return;

	curr = list->head;
	while(curr != NULL)
	{
		next = curr->next;
		free_b_pos(curr);
		curr = next;
	}

	free(list);
}

/******************************add and delete***********************************/
void add_pos_to_beacon(b_pos_t *b, pos_t *p)
{
	pos_t *temp;
	if(b == NULL || p == NULL) return;

	p->next = NULL;
	if(b->head == NULL || b->max == 1)
	{
		b->head = p;
		b->tail = p;
		b->size = 1;
		return;
	}

	if(b->size >= b->max){
		temp = b->head;
		b->head = b->head->next;
		
		if(b->head == NULL) b->tail =NULL;

		temp->next = NULL;
		free_pos(temp);
		b->size -= 1;
	}

	b->tail->next = p;
	b->tail = p;
	b->size+=1;
}

void add_pos_to_list(pos_list_t *list, pos_t *pos, char *mac, int buf_size)
{
	b_pos_t *temp;
	if(list == NULL || pos == NULL || mac == NULL) return;

	if(list->head == NULL){
		temp = (b_pos_t *)malloc(sizeof(b_pos_t));
		init_b_pos(temp, mac, buf_size);
		add_pos_to_beacon(temp, pos);

		list->head = temp;
		list->tail = temp;
		list->size = 1;
		return;
	}

	temp = list->head;
	while(temp != NULL){
		if(strcmp(temp->mac, mac)==0){
			printf("FOUND\n");
			if(temp->size != 0 || temp->head != NULL){
				printf("debug,,\n");
				if(temp->tail->room != pos->room)	clear_b_pos(temp);
			}
			printf("test..\n");
			add_pos_to_beacon(temp, pos);
			return;
		}
		temp = temp->next;
	}

	temp = (b_pos_t *)malloc(sizeof(b_pos_t));
	init_b_pos(temp, mac, buf_size);

	add_pos_to_beacon(temp, pos);

	list->tail->next = temp;
	list->tail = temp;
	list->size+=1;
}

void add_pos_info_to_list(pos_list_t *list, int x, int y, char *mac, int room)
{
	pos_t *pos;
	if(list == NULL) return;

	pos = (pos_t *)malloc(sizeof(pos_t));
	init_pos(pos, x, y, room);

	add_pos_to_list(list, pos, mac, 8);
}

void get_average_pos(b_pos_t *b, pos_t *result)
{
	pos_t *curr;
	result->loc.x = 0;
	result->loc.y = 0;

	if(b == NULL) return;

	curr = b->head;
	if(b->size == 1)
	{
		printf("x:%f, y:%f\n", curr->loc.x, curr->loc.y);
		result->loc.x = b->head->loc.x;
		result->loc.y = b->head->loc.y;
		return;
	}

	while(curr != NULL)
	{
		printf("x:%f, y:%f\n", curr->loc.x, curr->loc.y);
		result->room = curr->room;
		result->loc.x += curr->loc.x;
		result->loc.y += curr->loc.y;
		curr = curr->next;
	}
	printf("x_sum: %f, y_sum: %f, size: %d\n", result->loc.x, result->loc.y, b->size);
	result->loc.x /= b->size;
	result->loc.y /= b->size;
}
/*
void get_pos(pos_list_t *list, char *buf, int *length)
{
	b_pos_t *curr;
	pos_t average;
	int i = 0, offset = 0, len = 0;
	char temp[11];

	if(list == NULL) return;
	
	pthread_mutex_lock(&list->lock);
	printf("[INFO] Start retreving position\n");
	curr = list->head;

	while(curr!=NULL && i < 12)
	{
		strncpy(buf+offset, curr->mac, 17);
		buf[offset+17] = '|';
		offset+=18;
		get_average_pos(curr, &average);
		printf("%f|%f... %d|%d\n", average.loc.x, average.loc.y, (int) average.loc.x, (int) average.loc.y);
		sprintf(temp,"%d", (int)average.loc.x);
		len = strlen(temp);
		//printf("%s..%d\n", temp, len);
		strncpy(buf+offset, temp, len);
		offset+=len;
		buf[offset] = '|';
		offset++;

		sprintf(temp,"%d", (int)average.loc.y);
		len = strlen(temp);
		//printf("%s..%d\n", temp, len);
		strncpy(buf+offset, temp, len);
		offset+=len;
		if(curr->next != NULL){
			buf[offset] = '\n';
			offset++;
		}
		curr = curr->next;
		i++;
	}

	buf[offset] = 0;
	*length = offset;
	printf("[INFO] Done\n");

	pthread_mutex_unlock(&list->lock);

}
*/
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
		if(pos->loc.x <= 930){
			pos->loc.x = 800;
			pos->loc.y = 200;
		}else{
			pos->loc.x = 1050;
			pos->loc.y = 200;
		}
	}else if(pos->room == 3){
		if(pos->loc.x <= 1100){
			pos->loc.x = 950;
			pos->loc.y = 430;
		}else{
			pos->loc.x = 1253;
			pos->loc.y = 430;
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
			pos->loc.y = 730;
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


int get_pos_by_mac(pos_list_t *list,char *mac, char *buf, int offset)
{
	b_pos_t *curr;
	pos_t average;
	int len =0;
	char temp[11];
	char x_str[] = "\"X\": ", y_str[] = "\"Y\": ";

	if(list == NULL) return 0;
	
	pthread_mutex_lock(&list->lock);
	printf("\n[INFO] Start retreving position\n");
	curr = list->head;
	while(curr!=NULL)
	{
		if(strcmp(curr->mac, mac) == 0 && curr->size > 0){
			get_average_pos(curr, &average);
			//zone_adjust(&average);
			printf("%s: %d, %d\n", curr->mac, (int)average.loc.x, (int)average.loc.y);

			strncpy(buf+offset, y_str, strlen(y_str));
			offset+=(int)strlen(y_str);

			sprintf(temp,"%d", (int)average.loc.y);

			strncpy(buf+offset, temp, strlen(temp));
			len = (int)strlen(temp);
			buf[offset+len] = ',';
			buf[offset+len+1] = ' ';
			offset+=(len+2);

			strncpy(buf+offset, x_str, strlen(x_str));
			offset+=(int)strlen(x_str);

			sprintf(temp,"%d", (int)average.loc.x);
			strncpy(buf+offset, temp, strlen(temp));
			len = (int)strlen(temp);
			buf[offset+len] = ',';
			buf[offset+len+1] = ' ';
			offset+=(len+2);

			buf[offset] = 0;
			break;
		}
		curr = curr->next;
	}
	printf("[INFO] Done\n");
	pthread_mutex_unlock(&list->lock);
	return offset;
}


