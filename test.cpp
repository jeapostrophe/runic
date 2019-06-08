#include <iostream>
#include <cstring>
#include "runic.h"

using namespace std;

int main(void)
{
	auto ro = runic_open("example2.runic", CREATEWRITE);
	runic_obj_t ra;
	runic_obj_t rn2;
	runic_obj_t rn;

	printf("%lld\n", ro.sb.st_size);

	rn = runic_alloc_node(&ro);
	rn2 = runic_alloc_node(&ro);
	ra = runic_alloc_atom(&ro, 1);
	
	runic_close(ro);

	return 0;
}
