++ = g++
CPPFLAGS = -Wall -Werror -pedantic --std=c++11 -g --debug
WORDLIST2_OBJECTS = ../../wordlist2.e wordlist.o wlist.o superNode.o

ALL: wordlist2.e

wordlist2.e: wordlist.o wlist.o superNode.o
	$(++) $(CPPFLAGS) -lstdc++ -o ../../wordlist2.e wordlist.o ../../runic/runic.o wlist.o superNode.o

wordlist.o: wordlist.cpp
	$(++) $(CPPFLAGS) -c wordlist.cpp -o wordlist.o

wlist.o: wlist.cpp
	$(++) $(CPPFLAGS) -c wlist.cpp -o wlist.o

superNode.o: superNode.cpp
	$(++) $(CPPFLAGS) -c superNode.cpp -o superNode.o

clean:
	rm $(WORDLIST2_OBJECTS)
