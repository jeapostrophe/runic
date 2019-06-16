CC = gcc
CFLAGS = -Wall -Werror -pedantic --std=c11 -g
CPPFLAGS = -Wall -Werror -pedantic --std=c++11 -g
RUNIC_OBJECTS = runic/runic.o
TEST_OBJECTS = test.e test/test.o
MAIN_OBJECTS = compiled main.o
WORDLIST_OBJECTS = wordlist.e wordlist/wordlist.o wordlist/wlist.o

ALL: test.e wordlist.e

main.e: main.o runic.o
	$(CC) $(CFLAGS) -o main.e main.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

test.e: test.o runic.o
	$(CC) $(CPPFLAGS) -lstdc++ -o test.e test/test.o runic/runic.o

test.o: test/test.cpp
	$(CC) $(CPPFLAGS) -c test/test.cpp -o test/test.o

wordlist.e: wordlist.o runic.o wlist.o
	$(CC) $(CPPFLAGS) -lstdc++ -o wordlist.e wordlist/wordlist.o runic/runic.o wordlist/wlist.o

wordlist.o: wordlist/wordlist.cpp
	$(CC) $(CPPFLAGS) -c wordlist/wordlist.cpp -o wordlist/wordlist.o

wlist.o: wordlist/wlist.cpp
	$(CC) $(CPPFLAGS) -c wordlist/wlist.cpp -o wordlist/wlist.o

runic.o: runic/runic.c
	$(CC) $(CFLAGS) -c runic/runic.c -o runic/runic.o

clean:
	rm $(RUNIC_OBJECTS) $(TEST_OBJECTS) $(WORDLIST_OBJECTS) $(MAIN_OBJECTS)