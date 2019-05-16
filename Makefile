CC = gcc
CFLAGS = -Wall -Werror -pedantic --std=c11 -g 
RUNIC_OBJECTS = runic.o

ALL: runic.o

#compiled: main.o
#	$(CC) $(CFLAGS) -o compiled main.o

#main.o: main.c
#	$(CC) $(CFLAGS) -c main.c -o main.o

runic.o: runic.c
	$(CC) $(CFLAGS) -c runic.c -o runic.o

clean:
	rm $(RUNIC_OBJECTS)