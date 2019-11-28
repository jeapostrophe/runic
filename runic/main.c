#include "runic.h"

int main(int argc, char* argv[])
{
	runic_t r;
	runic_obj_t ro;
	r = runic_open("db.runic", READWRITE);
	ro.base = r.base;
	ro.offset = 4077;
	if (runic_obj_ty(ro) == NODE) {
		runic_set_root(&r, ro);
		runic_shrink(&r);
	}
	runic_remaining(r, false);
	return 0;
}
