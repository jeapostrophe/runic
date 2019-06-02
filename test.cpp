#include <iostream>
#include "runic.h"

using namespace std;

int main(void)
{
	auto ro = runic_open("example2.runic", CREATEWRITE);

	printf("%lld\n", ro.sb.st_size);

    runic_obj_node_t* rn = runic_alloc_node(&ro);
	
	runic_close(ro);

	return 0;
}
