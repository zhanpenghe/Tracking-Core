OBJS = tracking_server
LIBS = -lpthread -lm
CC = gcc
FILE = ./server.c
FLAG = -o

server:
	$(CC) $(FLAG) $(OBJS) $(FILE) $(LIBS)

clean:
	rm $(OBJS)
