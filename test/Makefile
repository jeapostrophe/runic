++ = g++
CPPFLAGS = -Wall -Werror -pedantic --std=c++11 -g --debug
TEST_OBJECTS = ../test.e test.o

ALL: test.e

test.e: test.o
	$(++) $(CPPFLAGS) -lstdc++ -o ../test.e test.o ../runic/runic.o

test.o: test.cpp
	$(++) $(CPPFLAGS) -c test.cpp -o test.o

clean:
	rm $(TEST_OBJECTS)