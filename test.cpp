#include <iostream>
#include "runic.h"

using namespace std;

int main(void)
{
	auto ro = runic_open("example.runic", READONLY);

	printf("%d\n", ro.fd);

	runic_close(ro);

	return 0;
}
