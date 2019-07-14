#include <iostream>
#include <cstring>
#include "../runic/runic.h"

using namespace std;

int main(void)
{
	//// a very basic test that proves the runic api is working.
	auto r = runic_open("../runic_files/example.runic", CREATEWRITE);
	char c[256];
	runic_obj_t ra, rn, rn2, ro1, ro2, ro3;
	cout << r.sb.st_size << endl;
	
	rn = runic_alloc_node(&r);
	rn2 = runic_alloc_node(&r);
	ra = runic_alloc_atom_str(&r, "HELLO_WORLD");
	runic_node_set_left(&rn, rn2);
	runic_node_set_right(&rn, ra);
	runic_set_root(&r, rn);
	ro1 = runic_root(r);
	ro2 = runic_node_left(rn);
	ro3 = runic_node_right(rn);

	if (rn.base != ro1.base) {
		cout << "fail test 1" << endl;
	}
	if (rn.offset != ro1.offset) {
		cout << "fail test 2" << endl;
	}
	if (rn2.base != ro2.base) {
		cout << "fail test 3" << endl;
	}
	if (rn2.offset != ro2.offset) {
		cout << "fail test 4" << endl;
	}
	if (ra.base != ro3.base) {
		cout << "fail test 5" << endl;
	}
	if (ra.offset != ro3.offset) {
		cout << "fail test 6" << endl;
	}
	if (rn2.base != ro3.base) {
		cout << "fail test 7" << endl;
	}
	if (rn.base != ro2.base) {
		cout << "fail test 8" << endl;
	}
	runic_atom_read(ra, c);
	cout << runic_atom_size(ra) << endl;
	cout << c << endl;
	runic_close(r);
	return 0;
}
