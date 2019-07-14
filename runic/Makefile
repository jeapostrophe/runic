CC = gcc
CFLAGS = -Wall -Werror -pedantic --std=c11 -g --debug
RUNIC_OBJECTS = runic.o

ALL: runic.o

runic.o: runic.c
	$(CC) $(CFLAGS) -c runic.c -o runic.o

clean:
	rm $(RUNIC_OBJECTS)