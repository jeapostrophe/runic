#include <iostream>
#include <cstring> // memcmp
#include <unistd.h> // access
#include "../runic/runic.h"  // runic
#include "wlist.h" // wlist

using namespace std;

int main(int argc, char* argv[]) {
	runic_t r;
	// just gonna assume this program is used properly
	// since its so simple, not doing error checks
	// on insert and lookup themselves
	if(memcmp("insert", argv[2], 6) != 0) {
		r = runic_open(argv[1], READONLY);
		lookup_item(r, argv[3]);
	} else {
		if (access(argv[1], F_OK) != -1) {
			r = runic_open(argv[1], READWRITE);
		} else {
			r = runic_open(argv[1], CREATEWRITE);
		}
		insert_item(&r, argv[3]);
	}
	runic_close(r);
	return 0;
}
