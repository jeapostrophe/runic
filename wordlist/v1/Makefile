++ = g++
CPPFLAGS = -Wall -Werror -pedantic --std=c++11 -g --debug
WORDLIST1_OBJECTS = ../../wordlist1.e wordlist.o wlist.o

ALL: wordlist1.e

wordlist1.e: wordlist.o wlist.o
	$(++) $(CPPFLAGS) -lstdc++ -o ../../wordlist1.e wordlist.o ../../runic/runic.o wlist.o

wordlist.o: wordlist.cpp
	$(++) $(CPPFLAGS) -c wordlist.cpp -o wordlist.o

wlist.o: wlist.cpp
	$(++) $(CPPFLAGS) -c wlist.cpp -o wlist.o

clean:
	rm $(WORDLIST1_OBJECTS)