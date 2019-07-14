#include <iostream>
#include <string>
#include <cstring> // memcmp, strlen
#include "../../runic/runic.h" // runic
#include "wlist.h" // wlist

using namespace std;

int insert_next_val(runic_t &r, string value)
{
	int loc = -1;
	int ans;
	runic_obj_t ra, next, current = runic_root(r);
	next = current; // current and next now point to the root node
	if ((ans = runic_obj_ty(current)) == -1) { // there is no node at root
		next = runic_alloc_node(&r); // make a node
		runic_set_root(&r, next); // set it as root
		ra = runic_alloc_atom_str(&r, value.c_str()); // make a string
		runic_node_set_left(&next, ra); // attach it to left of root
		loc = ra.offset; // return the location of the string
	} 
	else if (ans == NODE) { // a root node exists
		do {
			current = next; // make nodes equal
			next = runic_node_right(current); // get the next node
		} while (next.offset >= DEFAULT_ROOT); // stop when next is null
		next = runic_alloc_node(&r); // make a new node in memory
		runic_node_set_right(&current, next); // attach it to current
		ra = runic_alloc_atom_str(&r, value.c_str()); // make a new string
		runic_node_set_left(&next, ra); // attach it to next
		loc = ra.offset;
	}
	else if (ans == ATOM) {
		// in this case the root node is a string which we cannot add to
		// abort insert operation and report up to the caller
		return -1;
	}
	return loc;
}

void insert_item(runic_t &r, string value) {
	int loc;
	if ((loc = insert_next_val(r, value)) != -1) {
		printf("OK @ %d\n", loc);
	} else {
		// something got mal-formed
		printf("NO\n");
	}
}

int lookup_next_val(runic_t r, string value)
{
	int loc = -1;
	char c[256];
	int ans;
	runic_obj_t ra, next, current = runic_root(r);
	next = current; // current and next now point to the root node
	if ((ans = runic_obj_ty(current)) == -1) { // no root node means no string
		return -1;
	} 
	else if (ans == NODE) { // a root node exists
		do {
			current = next; // make nodes equal
			next = runic_node_right(current); // get the next node
			ra = runic_node_left(current); // check its string
			runic_atom_read(ra, c); // deposit string into c
			if (memcmp(value.c_str(), c,
				(value.length() < strlen(c)) ? value.length() : strlen(c)) == 0) { // if the strings match
				loc = ra.offset; // get the loc
				return loc; // return loc
			}
		} while (next.offset >= DEFAULT_ROOT); // stop when next is null
	} 
	else if (ans == ATOM) { // if the root node is a string then lets find out what it is
		runic_atom_read(current, c); // deposit string into c
		if (memcmp(value.c_str(), c,
			(value.length() < strlen(c)) ? value.length() : strlen(c)) == 0) {
			loc = current.offset;
		}
	}
	return loc;
}

void lookup_item(runic_t r, string value) {
	int loc;
	if ((loc = lookup_next_val(r, value)) != -1) {
		printf("YES @ %d\n", loc);
	} else {
		// doesnt exist or otherwise mal-formed
		printf("NO\n");
	}
}
