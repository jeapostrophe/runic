CC = gcc
CFLAGS = -Wall -Werror -pedantic --std=c11 -g
CPPFLAGS = -Wall -Werror -pedantic --std=c++11 -g
RUNIC_OBJECTS = runic.o
TEST_OBJECTS = test test.o
MAIN_OBJECTS = compiled main.o

ALL: test

#compiled: main.o
#	$(CC) $(CFLAGS) -o compiled main.o

#main.o: main.c
#	$(CC) $(CFLAGS) -c main.c -o main.o

test: test.o runic.o
	$(CC) $(CPPFLAGS) -lstdc++ -o test test.o runic.o

test.o: test.cpp
	$(CC) $(CPPFLAGS) -c test.cpp -o test.o

runic.o: runic.c
	$(CC) $(CFLAGS) -c runic.c -o runic.o

clean:
	rm $(RUNIC_OBJECTS) $(TEST_OBJECTS)