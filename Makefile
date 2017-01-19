OBJS = tracking_server
LIBS = -lpthread -lm
CC = gcc
FILE = ./server.c

server:
	$(CC) -o $(OBJS) $(FILE) $(LIBS)

clean:
	rm $(OBJS)
