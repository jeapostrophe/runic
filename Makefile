CC = gcc
CFLAGS = -Wall -Werror -pedantic --std=c11 -g --debug
MAIN_OBJECTS = main.e main.o

ALL: main.e

main.e: main.o
	$(CC) $(CFLAGS) -o main.e main.o runic/runic.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

clean:
	rm $(MAIN_OBJECTS)