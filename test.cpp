#include <iostream>
#include "runic.h"

using namespace std;

int main(void)
{
	// using c++ for auto!!!!
	/****

	runic_obj_t runic_alloc_atom_str ( runic_t ro, const char* c ) {
	auto ra = runic_alloc_node( ro, strlen(c) );
	runic_atom_write( ra, c );
	return ra; }

	//
	//  root = ("function" . ("int" . ("f" . ((("int" . "x") . NULL) . ("return" . ("+" . (("*" . ("2" . "x")) . ("3" . NULL))))))))
	//

	auto ro = runic_open("example.runic");
	auto rn = runic_alloc_node( ro );
	runic_set_root( ro, rn );

	runic_node_set_left( rn, runic_alloc_atom_str("function") );
	runic_node_set_right( rn, runic_alloc_node( ro ) );
	rn = runic_node_right( rn );

	runic_node_set_left( rn, runic_alloc_atom_str("int") );
	runic_node_set_right( rn, runic_alloc_node( ro ) );
	rn = runic_node_right( rn );

	runic_node_set_left( rn, runic_alloc_atom_str("f") );
	runic_node_set_right( rn, runic_alloc_node( ro ) );
	rn = runic_node_right( rn );

	auto args = runic_alloc_node( ro );
	runic_node_set_left( args, runic_alloc_node( ro ) );
	auto arg = runic_node_right( args );
	runic_node_set_left( arg, runic_alloc_atom_str("int") );
	runic_node_set_right( arg, runic_alloc_atom_str("x") );

	runic_node_set_left( rn, args );
	runic_node_set_right( rn, runic_alloc_node( ro ) );
	rn = runic_node_right( rn );
	runic_node_set_left( rn, runic_alloc_node( ro ) );
	rn = runic_node_left( rn );

	runic_node_set_left( rn, runic_alloc_atom_str("return") );
	runic_node_set_right( rn, runic_alloc_node( ro ) );
	rn = runic_node_right( rn );

	runic_node_set_left( rn, runic_alloc_atom_str("+") );
	runic_node_set_right( rn, runic_alloc_node( ro ) );
	rn = runic_node_right( rn );

	// XXX

	runic_close(ro);

	****/

	return 0;
}