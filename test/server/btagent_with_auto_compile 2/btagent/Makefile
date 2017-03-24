OBJS = btagent
LIBS = -lbluetooth
CC = gcc
FILE = btagent_v0.8.6.c


agent:
	$(CC) -o $(OBJS) $(FILE) $(LIBS)

run:
	sudo hciconfig hci0 down
	sudo hciconfig hci0 up
	sudo ./btagent -a 10.237.43.181 -w -d

clean:
	rm $(OBJS)
