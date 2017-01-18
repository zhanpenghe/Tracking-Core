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
	int size;
}pos_list_t;

/************************init and free*******************************/
void init_pos(pos_t *p, int x, int y, int room)
{
	if(p == NULL) return;

	p->loc->x = x;
	p->loc->y = y;
	p->room = room;
	p->next = NULL;
}

void free_pos(pos_t *p)
{
	if(p==NULL) return;
	free(p);
}

void init_b_pos(b_pos_t *b, char *mac)
{
	if(b == NULL) return;
	b->head = NULL;
	b->tail = NULL;
	b->next = NULL;
	b->size = 0;
	strncpy(b->mac, mac, 17);
	b->mac[17] = 0;
}

void free_b_pos(b_pos_t *b)
{
	if(b == NULL) return;

	clear_b_pos(b);

	free(b);
}

void clear_b_pos(b_pos_t *b)
{
	pos_t *curr, *next;
	if(b == NULL) return;

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

void init_pos_list(pos_list_t *list)
{
	if(list == NULL) return;

	list->head = NULL;
	list->tail = NULL;
	list->size = 0;
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
		temp->size -= 1;
	}

	b->tail->next = p;
	b->tail = p;
	b->size+=1;
}

void add_pos_to_list(pos_list_t *list, int x, int y, char *mac, int room)
{
	b_pos_t *temp;
	pos_t *pos;
	if(list == NULL) return;

	pos = (pos_t *)malloc(sizeof(pos_t));
	init_pos(pos, x, y, room);
	if(list->head == NULL)
	{
		temp = (b_pos_t *)malloc(sizeof(b_pos_t));
		init_b_pos(temp);

		add_pos_to_beacon(temp, pos);

		list->head = temp;
		list->tail = temp;
		list->size = 1;
		return;
	}

	temp = list->head;
	while(temp != NULL)
	{
		if(strcmp(temp->mac, mac) == 0)
		{
			printf("FOUND\n");
			if(temp->tail->room != rooom) clear_b_pos(temp);
			add_pos_to_beacon(temp,pos);
			return;
		}
		temp = temp->next;
	}

	temp = (b_pos_t *)malloc(sizeof(b_pos_t));
	init_b_pos(temp);

	add_pos_to_beacon(temp, pos);

	list->tail->next = temp;
	list->tail = temp;
	list->size+=1;
}

void get_pos_by_mac(pos_list_t *list, char *mac)
{
	b_pos_t *curr;
	if(list == NULL) return;

	curr = list->head;

	while(curr!=NULL)
	{
		if(strcmp(mac, curr->mac) == 0)
		{
			//found....
		}
		curr = curr->next;
	}

}


