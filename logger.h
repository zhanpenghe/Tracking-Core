/*
 * A simple Logger for c programs
 * @Author Adam Ho
 *
 * Usage:
 *
 * eg01: //keep the logger in buffer and use log_to_file() to log for multiple times
 * logger_t logger = (logger_t*) malloc(sizeof(logger_t));
 * 
 * init_logger(logger, mode, filepath);
 * log_to_file(logger, buf, len);
 *
 * free_logger(logger);
 *
 * eg02: //one time writinhg...
 * char str[] = "blablabla\n";
 * write_to_file("./testlogger.txt", 'a', str, strlen(str));
 */
#include <stdio.h>
#include <stdlib.h>

typedef struct logger{
	
	FILE* fd; //file descriptor
	char mode; // 'w' for overwriting, 'a' for appending

}logger_t;

void init_logger(logger_t *logger, char mode, char *path)
{
	if(logger == NULL || (mode != 'w' && mode !='a') || path == NULL) return;

	switch(mode){
		case 'w':
			logger->fd = fopen(path, "w");
			break;
		case 'a':
			logger->fd = fopen(path, "a");
			break;
		default:
			printf("something wrong\n");
			break;
	}

	logger->mode = mode;
	if(logger->fd == NULL) printf("Failed to open %s to write.\n", path);
}

void log_to_file(logger_t *logger, char *str, int len){
	if(logger == NULL) return;
	//printf("str: %s|len:%d\n",str,len);
	fwrite(str, len, 1, logger->fd);
}

void free_logger(logger_t *logger)
{
	if(logger == NULL) return;

	fclose(logger->fd);
	free(logger);
}


void write_to_file(char *filepath, char mode, char *buf, int len)
{
	logger_t *logger;
	if(buf == NULL || filepath == NULL) return;

	logger = (logger_t*) malloc(sizeof(logger_t));
	init_logger(logger, mode, filepath);

	log_to_file(logger, buf, len);

	free_logger(logger);
}


