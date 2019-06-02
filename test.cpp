#include <iostream>
#include <cstring>
#include "runic.h"

using namespace std;

int main(void)
{
	auto ro = runic_open("example2.runic", CREATEWRITE);
	runic_obj_atom_t* ra = NULL;
	runic_obj_node_t* rn2 = NULL;
	runic_obj_node_t* rn = NULL;

	printf("%lld\n", ro.sb.st_size);

	rn = runic_alloc_node(&ro);
	rn2 = runic_alloc_node(&ro);
	ra = runic_alloc_atom(&ro, 17);

    strcpy(ra->value, "THIS_IS_16_CHARS");
	
	runic_close(ro);

	return 0;
}
