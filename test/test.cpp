#include <iostream>
#include <cstring>
#include "../runic/runic.h"

using namespace std;

int main(void)
{
	auto r = runic_open("../runic_files/example2.runic", CREATEWRITE);
	char c[256];
	runic_obj_t ra;
	runic_obj_t rn2;
	runic_obj_t rn;
    runic_obj_t ro1, ro2, ro3;
	printf("%lld\n", r.sb.st_size);
	rn = runic_alloc_node(&r);
	rn2 = runic_alloc_node(&r);
	ra = runic_alloc_atom_str(&r, "HELLO_WORLD");
    runic_node_set_left(&rn, rn2);
    runic_node_set_right(&rn, ra);
    runic_set_root(&r, rn);
    ro1 = runic_root(r);
    ro2 = runic_node_left(rn);
    ro3 = runic_node_right(rn);
    if (rn.base == ro1.base) {
        printf("pass test 1\n");
    }
    if (rn.offset == ro1.offset) {
        printf("pass test 2\n");
    }
    if (rn2.base == ro2.base) {
        printf("pass test 3\n");
    }
    if (rn2.offset == ro2.offset) {
        printf("pass test 4\n");
    }
    if (ra.base == ro3.base) {
        printf("pass test 5\n");
    }
    if (ra.offset == ro3.offset) {
        printf("pass test 6\n");
    }
    if (rn2.base == ro3.base) {
        printf("pass test 7\n");
    }
    if (rn.base == ro2.base) {
        printf("pass test 8\n");
    }
	runic_atom_read(ra, c);
    printf("%lu ,", runic_atom_size(ra));
    printf(" %s\n" , c);
	runic_close(r);
	return 0;
}
