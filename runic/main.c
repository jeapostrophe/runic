#include "runic.h"

int main(int argc, char* argv[])
{
	runic_t r;
	r = runic_open("db.runic", CREATEWRITE);
	runic_remaining(r, false);
	return 0;
}
