#include <iostream>
#include "../runic/runic.h"
#include "wlist.h"

using namespace std;

int main(int argc, char* argv[]) {
	runic_t r;
	r = runic_open(argv[1], READWRITE);
	// just gonna assume this program is used properly
	// since its so simple, not doing error checks
	// on insert and lookup themselves
	if(memcmp("insert", argv[2], 6) != 0) {
		lookup_item(r, argv[3]);
	} else {
		insert_item(&r, argv[3]);
	}
	runic_close(r);
	return 0;
}