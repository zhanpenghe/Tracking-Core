OBJS = server
LIBS = -lpthread
CC = gcc
FILE = ./server.c

server:
	$(CC) -o $(OBJS) $(FILE) $(LIBS)

clean:
	rm $(OBJS)