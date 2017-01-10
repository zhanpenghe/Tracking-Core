/*
 * A parameter reader for multiple values.
 * Parameters will be store in a list as <key, [values]>
 * @Author Adam Ho
 * 
 * Usage example:
 * 
 * param_slist_t *list = (param_slist_t*) malloc(sizeof(param_slist_t));
 * param_s_t *curr = NULL;
 * val_t *temp = NULL;
 * read_param_file("../parameters/list_agent.txt", list);
 *
 * while((curr = next_param_s(list))!=NULL)
 * {
 * 		printf("\n%s\n", curr->key);
 * 		while((temp = next_val(curr))!=NULL) printf("%s\n", temp->val);
 * 	}
 * free_param_slist(list);
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "configure.h"


typedef struct value{
	char* val;
	struct value* next;
}val_t;

//one key with multiple values... failed to come up with a good name
typedef struct param_s
{
	char *key;
	val_t *start;
	val_t *end;
	val_t *position;
	int counter;
	int size;
	struct param_s *next;
}param_s_t;

typedef struct param_slist
{
	param_s_t *start;
	param_s_t *end;
	param_s_t *position;
	int counter;
	int size;
}param_slist_t;

/**************************init and free functions************************************/

void init_val(val_t *val, char* str)
{
	if(val == NULL || str == NULL) return;
	val->val = str;
	val->next = NULL;
}

void free_val(val_t *val)
{
	if(val == NULL) return;
	free(val->val);
	free(val);
}

void init_param_s(param_s_t *p)
{
	if(p == NULL) return;
	p->key = NULL;
	p->start = NULL;
	p->end = NULL;
	p->position = NULL;
	p->size = 0;
	p->counter = 0;
}

void free_param_s(param_s_t *p)
{
	val_t *curr, *next;
	if(p == NULL) return;

	curr = p->start;
	while(curr != NULL){
		next = curr->next;
		printf("freeing val: %s\n", curr->val);
		free_val(curr);
		curr = next;
	}
	free(p);
}

void init_param_slist(param_slist_t *list)
{
	if(list == NULL) return;

	list->counter = 0;
	list->start = NULL;
	list->end = NULL;
	list->position = NULL;
	list->size = 0;
}

void free_param_slist(param_slist_t *list)
{
	param_s_t *curr, *next;
	if(list == NULL) return;

	curr = list->start;
	while(curr != NULL)
	{
		next = curr->next;
		printf("\nFREEING PARAM: %s\n", curr->key);
		free_param_s(curr);
		curr = next;
	}
	free(list);
}

/*************************String--->param_s_t functions******************************/

void add_val_to_params(param_s_t *p, val_t * v)
{
	if(p == NULL || v == NULL) return;
	if(p->key == NULL) return;

	v->next = NULL;
	if(p->size == 0)
	{
		p->start = v;
		p->end = v;
		p->size = 1;
		p->position = v;
		return;
	}

	p->end->next = v;
	p->end = v;
	p->size+=1;
}

val_t *next_val(param_s_t *p)
{
	if(p == NULL) return NULL;
	if(p->position == NULL) return NULL;

	if(p->counter == p->size){	//one round end.. reset counter
		p->counter = 0;
		return NULL;
	}

	val_t *temp = p->position;
	p->position = (p->position->next != NULL) ? p->position->next : p->start;
	p->counter+=1;

	return temp;
}


void set_param_s_from_str(param_s_t *p, char *line, size_t len)
{
	char c;
	char *temp;
	val_t *v;
	int i = 0, start = 0, end = 0;

	if(p == NULL || line == NULL) return;

	init_param_s(p);

	while(i<(int)len)
	{
		c = *(line + i);

		if(c == '|'  && start != i)
		{
			end = i-1;
			trim_str(line, &start, &end, (int) len);
			temp = (char*) malloc(end-start+2);
			strncpy(temp, line+start, end-start+1);
			temp[end-start+1] = 0;

			if(p->key == NULL){
				p->key = temp;
			}else{
				v = (val_t*)malloc(sizeof(val_t));
				init_val(v, temp);
				add_val_to_params(p, v);
			}
			start = i+1;
		}
		i++;
	}
	if(start >= (int) len) return;
	
	end = i-1;
	trim_str(line, &start, &end, (int) len);
	temp = (char*) malloc(end-start+2);
	strncpy(temp, line+start, end-start+1);
	temp[end-start+1] = 0;

	v = (val_t*)malloc(sizeof(val_t));
	init_val(v, temp);
	add_val_to_params(p, v);
}

/**********************param_slist_t functions*******************************/
void add_param_s_to_list(param_slist_t *list, param_s_t *p)
{
	if(list == NULL || p == NULL) return;
	if(p->key == NULL) return;	// there's no point to add empty things to the list

	p->next = NULL;
	if(list->size == 0)
	{
		list->start = p;
		list->end = p;
		list->size = 1;
		list->position = list->start;
		return;
	}

	list->end->next = p;
	list->end = p;
	list->size+=1;
}


param_s_t *next_param_s(param_slist_t *list)
{
	if(list == NULL) return NULL;
	if(list->position == NULL) return NULL;

	if(list->counter == list->size){	//one round end.. reset counter
		list->counter = 0;
		return NULL;
	}

	param_s_t *temp = list->position;
	list->position = (list->position->next != NULL) ? list->position->next : list->start;
	list->counter+=1;

	return temp;
}

void read_param_file(const char* filename, param_slist_t *list)
{
	FILE* fp;
	char* line = NULL;
	size_t len = 0, read;

	if(filename == NULL){
		printf("[ERROR] Configuration filename is null\n");
		exit(EXIT_FAILURE);
	}

	fp = fopen(filename, "r");
	if (fp == NULL){
		printf("[ERROR] Failed to open configuration file: %s\n", filename);
		exit(EXIT_FAILURE);
	}

	while ((read = getline(&line, &len, fp)) != -1) {

		if(line[0]=='#' || read < (size_t)3) continue;

		printf("\n%s\n", line);

		param_s_t * p = (param_s_t*) malloc(sizeof(param_s_t));
		set_param_s_from_str(p, line, read);
		
		add_param_s_to_list(list, p);
    }

    fclose(fp);
}
