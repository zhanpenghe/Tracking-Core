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
	printf("add b\n");
	p->next = NULL;
	if(b->head == NULL)
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

	add_pos_to_list(list, pos, mac, 10);
}

void get_average_pos(b_pos_t *b, point_t *result)
{
	pos_t *curr;
	result->x = 0;
	result->y = 0;

	if(b == NULL) return;

	curr = b->head;
	while(curr != NULL)
	{
		printf("x:%f, y:%f\n", curr->loc.x, curr->loc.y);
		result->x += curr->loc.x;
		result->y += curr->loc.y;
		curr = curr->next;
	}
	result->x /= b->size;
	result->y /= b->size;
}

void get_pos(pos_list_t *list, char *buf, int *length)
{
	b_pos_t *curr;
	point_t average;
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
		printf("%f|%f... %d|%d\n", average.x, average.y, (int) average.x, (int) average.y);
		sprintf(temp,"%d", (int)average.x);
		len = strlen(temp);
		//printf("%s..%d\n", temp, len);
		strncpy(buf+offset, temp, len);
		offset+=len;
		buf[offset] = '|';
		offset++;

		sprintf(temp,"%d", (int)average.y);
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


