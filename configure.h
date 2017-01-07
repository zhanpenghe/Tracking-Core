/*
 * A .ini file reader for program configuration
 * @Author Adam Ho
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct parameter
{
	char* key;
	char* value;
	struct parameter* next; 
}param_t;

typedef struct paramList{
	param_t* head;
	param_t* end;
	param_t* position;
	int counter;
	int size;
}paramList_t;

void setParam(param_t *p, char* key, char* value)
{
	if(p == NULL) return;
	p->key = key;
	p->value = value;
	p->next = NULL;
}

void freeParam(param_t *p)
{
	if(p == NULL) return;
	free(p->key);
	free(p->value);
	free(p);
}

void init_param_list(paramList_t *list)
{
	if(list == NULL) return;
	list->head = NULL;
	list->end = NULL;
	list->position = NULL;
	list->size = 0;
	list->counter = 0;
}

void free_param_list(paramList_t *list)
{
	param_t *curr, *next;
	if(list == NULL) return;
	printf("size: %d\n", list->size);
	curr = list->head;
	while(curr!=NULL)
	{
		next = curr->next;
		printf("freeing param|key|%s|val|%s\n", curr->key, curr->value);
		freeParam(curr);
		curr = next;
	}
}

void add_param(paramList_t *list, param_t *p)
{
	if(list == NULL || p == NULL) return;

	p->next = NULL;
	if(list->size == 0)
	{
		list->head = p;
		list->position = p;
		list->end = p;
		list->size = 1;
		return;
	}

	list->end->next = p;
	list->end = p;
	list->size+=1;
}

param_t *next_param(paramList_t *list)
{
	if(list == NULL) return NULL;
	if(list->position == NULL) return NULL;

	if(list->counter == list->size){	//one round end.. reset counter
		list->counter = 0;
		return NULL;
	}

	param_t *temp = list->position;
	list->position = (list->position->next != NULL) ? list->position->next : list->head;
	list->counter+=1;

	return temp;
}

void trim_str(char* str, int* begin, int* end, int len)
{
	if(str == NULL || begin == NULL|| end == NULL) return;

	while(*begin<len)
	{
		if(*(str+*begin) == ' ') *begin+=1;
		else break;
	}

	while(*end>=0)
	{
		if(*(str+*end) == ' ' || *(str+*end) == 0 ||*(str+*end) == '\n') *end-=1;
		else break;
	}
}

void setParamFromString(param_t *p, char* line, size_t len)
{
	char c;
	char *key, *value;
	int i = 0;
	int key_start = 0, key_end = 0, val_start = 0, val_end = len-1;

	if(p == NULL || line == NULL) return;

	while(i < (int) len)
	{
		c = *(line+i);
		if(c == '=')
		{
			key_end = i-1;
			val_start = i+1;
		
			trim_str(line, &key_start, &key_end, (int) len);
			key = (char*) malloc(key_end-key_start+2);
			strncpy(key, line+key_start, key_end-key_start+1);
			key[key_end-key_start+2] = 0;

			trim_str(line, &val_start, &val_end, (int) len);
			value = (char*) malloc(val_end-val_start+2);
			strncpy(value, line+val_start, val_end-val_start+1);
			value[val_end-val_start+2] = 0;

			setParam(p, key, value);
			break;
		}
		i++;
	}
}

/*
 *	@Param filename or path of the configuration file eg. "conf.ini"
 */
void read_ini_file(const char* filename, paramList_t *list)
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

		if(line[0]=='#' || read <= (size_t)3) continue;

        param_t *p = (param_t*)malloc(sizeof(param_t));
        setParamFromString(p, line, read);
        add_param(list, p);
    }

    fclose(fp);
}