/*
 * Data structure for storing measure and get ready for computation
 *
 * @Author Adam Ho
 * 
 *--------------------------
 * Visualization:
 *
 *    blist_entry------>beacon01------------>beacon02------------>beacon03------>NULL
 *                         |                    |                    |
 *                        / \                  / \                  / \
 *                       /   \                /   \                /   \
 *                agent01    agent02    agent01  agent02     agent01   agent02
 *                 /            \          /         \          /         \
 *		  rssi_val01     rssi_val02  rssi_val03  rssi_val04 rssi_val05   rssi_val06
 *            |              |            |           |        |           |
 *        rssi_val07     rssi_val08  rssi_val09  rssi_val10 rssi_val11   rssi_val12
 *            |              |            |           |        |           |
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct rssi_pair{
	int8_t rssi;
	char mac[18];	//agent mac addr
}rssi_pair_t;

typedef struct rssi
{
	int8_t rssi;
	struct rssi *next;
}rssi_t;

typedef struct rssi_queue
{
	rssi_t *head;
	rssi_t *tail;
	char mac[18];
	int size;
	int max;
	struct rssi_queue *next;
}rssi_queue_t;

typedef struct beacon{
	rssi_queue_t *head;
	rssi_queue_t *tail;
	char mac[18];
	struct beacon *next;
	int size;
}beacon_t;

/*
 * This is totally a different thing from the blist in agent program.
 * Check the comment at the top.
 */
typedef struct blist{
	beacon_t *head;
	beacon_t *tail;
	int q_max;
	int size;
	pthread_mutex_t lock;
}blist_t;

/**********************init and free operations*********************/
void init_rssi(rssi_t *r, int8_t rssi_val)
{
	if(r == NULL) return;

	r->rssi = rssi_val;
	r->next = NULL;
}

void free_rssi(rssi_t *r)
{
	if(r == NULL) return;
	printf("free: %d\n", r->rssi);
	free(r);
}

void init_rssi_queue(rssi_queue_t *q, char* mac, int max)
{
	if(q == NULL) return;
	q->head = NULL;
	q->tail = NULL;
	q->next = NULL;
	q->size = 0;
	q->max = max;

	strncpy(q->mac, mac, 17);
	q->mac[17] = 0;
}

void free_rssi_queue(rssi_queue_t *q)
{
	rssi_t *curr, *next;
	if(q == NULL) return;

	curr = q->head;
	while(curr != NULL){
		next = curr->next;
		free_rssi(curr);
		curr = next;
	}

	free(q);
}

void init_beacon(beacon_t *b, char *mac)
{
	if(b == NULL) return;
	b->head = NULL;
	b->tail = NULL;
	b->next = NULL;
	b->size = 0;
	strncpy(b->mac, mac, 17);
	b->mac[17] = 0;
}

void free_beacon(beacon_t *b)
{
	rssi_queue_t *curr, *next;
	if(b == NULL) return;

	curr = b->head;
	while(curr != NULL)
	{
		next = curr->next;
		free_rssi_queue(curr);
		curr = next;
	}
	free(b);
}

void init_blist(blist_t *list, int q_max)
{
	if(list == NULL) return;

	if(q_max > 0) 	list->q_max = q_max;
	else list->q_max = 5;

	list->head = NULL;
	list->tail = NULL;
	list->size = 0;

	if(pthread_mutex_init(&list->lock, NULL) != 0)
	{
		printf("[ERROR] Blist mutex init failed\n");
		exit(1);
	}
}

void free_blist(blist_t *list)
{
	beacon_t *curr, *next;
	if(list == NULL) return;

	curr = list->head;
	while(curr!=NULL)
	{
		next = curr->next;
		free_beacon(curr);
		curr = next;
	}
	free(list);
}

/**********************Queue operations*****************************/
rssi_t *dequeue(rssi_queue_t *q)
{
	rssi_t *result;
	if(q == NULL) return NULL;

	if(q->head == NULL) return NULL;

	result = q->head;

	//delete it from the queue
	q->head = result->next;
	if(result->next == NULL) {
		q->tail = NULL;
	}

	result->next = NULL;
	q->size -= 1;

	return result;
}

void enqueue(rssi_queue_t *q, rssi_t *r)
{
	if(q == NULL || r == NULL) return;

	r->next = NULL;
	if(q->head == NULL)
	{
		q->head = r;
		q->tail = r;
		q->size = 1;
		return;
	}

	q->tail->next = r;
	q->tail = r;
	q->size += 1;
}

void add_rssi_to_q(rssi_queue_t *q, rssi_t *r)
{
	if(q == NULL || r == NULL) return;

	if(q->size < q->max){
		enqueue(q, r);
		return;
	}
	if(q->size >= q->max){
		free_rssi(dequeue(q));
		enqueue(q, r);
	}
}

void get_rssi_from_q(rssi_queue_t *q, rssi_pair_t *pair)
{
	int result = 0;
	float MA = 0.2;
	rssi_t *curr;
	if(pair == NULL) return;

	if(q == NULL){
		pair->rssi = 34;
		return;
	}

	strncpy(pair->mac, q->mac, 17);
	pair->mac[17] = 0;
	if(q->size == 0){
		pair->rssi = 34;
		return;
	}

	curr = q->head;
	if(q->size < q->max)
	{
		result = q->head->rssi;
		while(curr!=NULL)
		{
			if(curr->rssi > result)
			{
				result = curr->rssi;
			}
			curr = curr->next;
		}

		pair->rssi = result;
		return;
	}
	
	result = q->head->rssi;
	while(curr!=q->head)
	{
		if(curr->rssi >result )
		{
			result = curr->rssi;
		}
		curr = curr->next;
	}

	pair->rssi = (int)(MA*((float)curr->rssi) + (1-MA)*((float)result));

	/*while(curr!=q->head)
	{
		avg += (1-MA)/(q->max-1)*(float)curr->rssi;
		curr = curr->next;
	}

	avg = (0.2)*((float)curr->rssi);
	pair->rssi = (int) avg;*/
}

void get_rssi_for_calc(beacon_t *b, rssi_pair_t pairs[], int agent_num)
{
	int i = 0;
	rssi_queue_t *curr;

	if(b == NULL){
		memset(pairs, 0, sizeof(rssi_pair_t)*agent_num);
		return;
	}

	curr = b->head;
	while(curr != NULL)
	{
		get_rssi_from_q(curr, &pairs[i]);
		curr = curr->next;
		i++;
	}

	if(i != b->size) printf("ERROR WHEN GETTING RSSI FROM BLIST!!!!!!\n");

	if(b->size < agent_num)
		memset(&pairs[b->size], 0, sizeof(rssi_pair_t)*(agent_num - b->size));
}


void add_rssi_to_beacon(beacon_t *b, int8_t rssi_val, char *mac, int q_max)
{
	rssi_queue_t *temp;
	rssi_t *r;
	if(b == NULL) return;

	r = (rssi_t *) malloc(sizeof(rssi_t));
	init_rssi(r, rssi_val);

	if(b->head == NULL){
		temp = (rssi_queue_t *) malloc(sizeof(rssi_queue_t));
		init_rssi_queue(temp, mac, q_max);

		enqueue(temp, r);

		b->head = temp;
		b->tail = temp;
		b->size = 1;
		return;
	}

	temp = b->head;
	while(temp!=NULL)
	{
		if(strcmp(temp->mac, mac) == 0){
			add_rssi_to_q(temp, r);
			return;
		}
		temp = temp->next;
	}

	temp = (rssi_queue_t *) malloc(sizeof(rssi_queue_t));
	init_rssi_queue(temp, mac, q_max);

	enqueue(temp,r);

	b->tail->next = temp;
	b->tail = temp;
	b->size+=1;
}

void add_rssi_to_blist(blist_t *list, int8_t rssi_val, char *agent_mac, char *beacon_mac)
{
	beacon_t *temp;
	if(list == NULL || agent_mac == NULL || beacon_mac == NULL) return;
	printf("%d\n", list->size);
	if(list->head == NULL || list->size == 0){
		temp = (beacon_t *) malloc(sizeof(beacon_t));
		init_beacon(temp, beacon_mac);
		add_rssi_to_beacon(temp, rssi_val, agent_mac, list->q_max);

		list->head = temp;
		list->tail = temp;
		list->size = 1;
		return;
	}

	temp = list->head;
	while(temp != NULL)
	{
		if(strcmp(temp->mac, beacon_mac) == 0){
			printf("%s,%s\n", temp->mac, beacon_mac);
			add_rssi_to_beacon(temp, rssi_val, agent_mac, list->q_max);
			return;
		}
		temp = temp->next;
	}

	temp = (beacon_t *) malloc(sizeof(beacon_t));
	init_beacon(temp, beacon_mac);
	
	add_rssi_to_beacon(temp, rssi_val, agent_mac, list->q_max);
	
	list->tail->next = temp;
	list->tail = temp;
	list->size+= 1;
}

/***********************print functions*******************************/
void print_rssi_q(rssi_queue_t *q)
{
	rssi_t *curr;
	if(q == NULL) return;
	printf("agent: %s\tsize: %d\n", q->mac, q->size);
	curr = q->head;
	while(curr!=NULL)
	{
		printf("rssi value: %d\n", curr->rssi);
		curr = curr->next;
		usleep(5000);
	}
}

void print_beacon(beacon_t *b)
{
	rssi_queue_t *curr;
	if( b == NULL ) return;
	printf("------------------beacon(size: %d)-------------------\nMAC: %s\n\n", b->size, b->mac);
	curr = b->head;
	while(curr!=NULL)
	{
		print_rssi_q(curr);
		curr = curr->next;
	}
}

void print_blist(blist_t *list)
{
	beacon_t *curr;
	if(list == NULL) return;
	printf("\n===============================================================\nPrinting Beacon List Now(Size:%d)\n", list->size);
	curr = list->head;
	while(curr!=NULL)
	{
		print_beacon(curr);
		curr = curr->next;
	}
}

void store_rssi_from_agent(blist_t *list, char *msg)
{
	char *line, *temp;
	char *beacon_mac, *rssi_str;

	char agent_mac[18];
	if(list == NULL || msg == NULL) return;

	strncpy(agent_mac, msg, 17);
	agent_mac[17] = 0;
	printf("%s\n", agent_mac);

	line = strtok(msg,"\n");
	while((line = strtok(NULL, "\n"))!=NULL)
	{
		temp = strtok(line, "|");
		beacon_mac = temp;
		temp = strtok(NULL, "|");
		rssi_str = temp;
		temp = strtok(NULL,"|");
		line = temp;
		//printf("%s|%s..%s..%s..\n", agent_mac,beacon_mac, rssi_str, line);
		pthread_mutex_lock(&list->lock);
		printf("[INFO] Adding rssi to list\n");
		add_rssi_to_blist(list, atoi(rssi_str), agent_mac, beacon_mac);	
		//print_blist(list);

		pthread_mutex_unlock(&list->lock);
		printf("[INFO] Done with adding rssi to list\n");

	}

}