#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct beacon{
	int8_t flag;//sent or not sent
	int8_t rssi;
	double startime;
	double timestamp;
	struct beacon *next;
	char mac[18];
}beacon_t;

typedef struct list{
	char *loc;
	beacon_t *head;
	int size;
}list_t;


typedef struct msg{
	struct msg *next;
	int8_t rssi;
	double timestamp;
	char mac[18];
}msg_t;


typedef struct messages{
	int size;
	msg_t *first;
	msg_t *last;
}mlist_t;


void set_beacon(beacon_t *beacon, int8_t rssi, double timestamp, beacon_t *next, char* mac){
	if(beacon == NULL) return;
	beacon->rssi = rssi;
	beacon->startime = timestamp;
	beacon->timestamp = timestamp;
	beacon->next = next;
	strncpy(beacon->mac, mac,18);
	beacon->flag = 0;
}

list_t *init_list(char *loc)
{
	list_t *lst =  (list_t *) malloc(sizeof(list_t));
	lst->loc = loc;
	lst->head = NULL;
	lst->size = 0;
	return lst;
}

void set_message(msg_t *m, beacon_t *b, msg_t *next){
	if(m == NULL || b == NULL) return;
	printf("setting_message\n");

	m->rssi = b->rssi;
	m->timestamp = b->timestamp;
	m->next = next;
	strncpy(m->mac, b->mac, 18);
	printf("setting_message_done\n");

}

void add_message_to_send(mlist_t *lst, beacon_t *b)
{
	msg_t *temp;

	if(lst == NULL || b == NULL) return;

	if(lst->size == 0){
		printf("adding a message_01\n");

		lst->first = (msg_t*) malloc(sizeof(msg_t));
		set_message(lst->first, b, NULL);
		lst->last = lst->first;
		lst->size = 1;
		printf("finished_01\n");
		return;
	}

	printf("adding a message_02\n");

	temp = (msg_t*) malloc(sizeof(msg_t));
	set_message(temp, b, NULL);
	lst->last->next = temp;
	lst->last = temp;
	lst->size++;
	printf("finished\n");
}


void print_list(list_t *lst)
{
	beacon_t *ptr;
	if(lst == NULL){
		printf("[ERROR]The list is NULL");
		return;
	} 
	printf("\n\n--------------------Size: %d-------------------\n", lst->size);
	ptr = lst->head;

	while(ptr!=NULL){
		printf("beacon: %s\trssi: %d\tstarttime: %f\ttimestamp: %f\tsent: %d\n", ptr->mac, ptr->rssi,ptr->startime,ptr->timestamp, ptr->flag);
		ptr = ptr->next;
	}

	printf("\n");
}

void free_beacon_list(list_t *lst){
	beacon_t *ptr;
	if(lst == NULL) return;

	while(lst->head!=NULL)
	{
		ptr =  lst->head;
		lst->head = lst->head->next;
		free(ptr);
	}

	free(lst);
}

void print_msg_list(mlist_t *lst)
{
	msg_t *ptr;

	if(lst==NULL){
		printf("[ERROR]The msg list is NULL");
		return;
	}
	printf("\n\n------------Type: MSG_LIST   Size: %d-------------\n", lst->size);
	ptr = lst->first;

	while(ptr!=NULL){
		printf("Beacon: %s\trssi: %d\ttimestamp: %f\n", ptr->mac, ptr->rssi,ptr->timestamp);
		ptr = ptr->next;
	}
	printf("\n");
}

mlist_t *insert(list_t *lst, int8_t rssi, double timestamp, char *mac, mlist_t *messages){

	beacon_t *ptr, *prev;
	int8_t inserted = 0;


	if(lst == NULL) return NULL;
	printf("====================================\n");
	printf("inputbeacon: %s\trssi: %d\ttimestamp: %f\n", mac, rssi,timestamp);
	if(lst->size == 0){
		printf("[INFO_00]insert new node for mac: %s\n", mac);
		ptr = (beacon_t*) malloc(sizeof(struct beacon));
		set_beacon(ptr, rssi, timestamp, NULL, mac);
		lst->head = ptr;
		lst->size = 1;
		print_list(lst);
		return messages;
	}
	printf("[DEBUG_01]\n");
	ptr = lst->head;

	while(ptr!=NULL)
	{
		printf("[DEBUG_AAA]\n");
		printf("beacon: %s\trssi: %d\tstarttime: %f\ttimestamp: %f\tsent: %d\n", ptr->mac, ptr->rssi,ptr->startime,ptr->timestamp, ptr->flag);
		prev = ptr;
		if(strcmp(mac, ptr->mac)==0){
			printf("[INFO_01]FOUND\n");
			inserted = 1;
			if(timestamp*1000>(ptr->startime*1000+100) && !ptr->flag){
				printf("[INFO_01]A message need to be sent\n");
				add_message_to_send(messages,ptr);
				ptr->startime = ((int)((timestamp - ptr->startime)/100)+0.0)*100.0+ptr->startime;//tobecheck...
				ptr->timestamp = timestamp;
				ptr->rssi = rssi;
				ptr->flag = 0;
			}else if(timestamp*1000>(ptr->startime*1000+100) && ptr->flag){
				printf("[INFO_01]Find zombie node, updating it.\n");
				ptr->startime = ((int)((timestamp - ptr->startime)/100)+0.0)*100.0+ptr->startime;//tobecheck...
				ptr->timestamp = timestamp;
				ptr->rssi = rssi;
				ptr->flag = 0;
			}else if(timestamp*1000<=(ptr->startime*1000+100) && rssi>ptr->rssi){
				printf("[INFO_01]Updating unsent message whattttt?\n");
				ptr->timestamp = timestamp;
				ptr->rssi = rssi;
			}
		}else{
			printf("[INFO_02]NOT_FOUND\n");
			if(timestamp*1000>(ptr->startime*1000+100))
			{
				printf("[INFO_02]A message need to be sent\n");
				add_message_to_send(messages, ptr);
				ptr->flag = 1;
			}
		}
		printf("next\n");
		ptr = ptr->next;
	}
	printf("[DEBUG_02]\n");

	if(!inserted){
		ptr = (beacon_t*) malloc(sizeof(struct beacon));
		set_beacon(ptr, rssi, timestamp, NULL, mac);

		prev->next = ptr;
		lst->size++;
	}
	print_list(lst);
	printf("[DEBUG_03]\n");

	return messages;
}

void free_msg_list(mlist_t *m, int done)
{
	msg_t *ptr;
	if(m == NULL) return;

	while(m->first!=NULL)
	{
		ptr = m->first;
		m->first = m->first->next;
		free(ptr);
	}

	m->first =NULL;
	m->size = 0;
	if(done == 1){
		free(m);
	}
}




void send_message(mlist_t *m, char *send_msg, int tcpSock, char *readBuffer, int msg_size, int displayEnable,char *loc)
{
	msg_t *ptr;
	char rssistr[15];
	char ctimestr[20];
	if(m == NULL || send_msg == NULL) return;
	if(m->size!=0){
		snprintf(send_msg, 672, "%s|%d", loc, m->size);
		ptr = m->first;
		while(ptr!=NULL){
			snprintf(rssistr, 15, "%d", ptr->rssi);
			snprintf(ctimestr,20,"%6.4f",ptr->timestamp);
			strcat(send_msg, "\n");
			strcat(send_msg, ptr->mac);     // Beacon MAC
			strcat(send_msg, "|");
			strcat(send_msg, rssistr); // RSSI
			strcat(send_msg, "|");
			strcat(send_msg, ctimestr);
			ptr = ptr->next;
		}
		printf("*******message:\n%s\n",send_msg);
		if( write(tcpSock, send_msg, strlen(send_msg)) < 0 ) {
			perror("ERROR: Send beacon info error");
        }
        if (displayEnable){
            printf("INFO: Send\n");
		}
		memset(readBuffer, 0, msg_size);
		if (read(tcpSock, readBuffer, msg_size) < 0 ) {
			perror("ERROR: Cannot read from socket");
        }
		if (displayEnable){
			printf("INFO: Received.\n");
		}
	}
	printf("cleanning mem\n");
	free_msg_list(m,0);
	printf("cleaned\n");
}



