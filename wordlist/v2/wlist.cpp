#include <iostream>
#include <string> // string
#include <cstring> // memcmp, strlen
#include "wlist.h" // wlist
#include "superNode.h" // superNode
#include "PicoSHA2/picosha2.h" // sha256

using namespace std;

string __get_hash_val(string input) {
	string hash_hex_str;
	picosha2::hash256_hex_string(input, hash_hex_str);
	return hash_hex_str;
}

void insert_item(runic_t &r, string value) {
	int loc;
	superNode node(r); // generate a potential superNode
	string hash_val = __get_hash_val(value);
	if ((loc = insert_base_val(r, node, hash_val)) != -1)// fall through to find the correct location
		printf("OK @ %d\n", loc);
	else // something got mal-formed
		printf("NO\n");
}

int insert_next_val(runic_t &r, superNode parent, superNode node, string value, bool l=false) {
	if (node.empty()){ // if no root exists
		superNode newNode(r,value); // allocate one with this value
		if (!newNode.empty()) {
			if (l)
				parent.set_left(newNode);
			else
				parent.set_right(newNode);
				
			return newNode.str_loc(); // return it's location
		} else
			return -1; // alloc failed
	}

	// otherwise a root exists, determine left or right
	int dir = memcmp(node.read().c_str(), value.c_str(),
		(value.length() < node.read().length()) ? value.length() : node.read().length());

	if (dir > 0) // if less than 0, insert left subtree
		return insert_next_val(r, node, node.left(), value, true);
	else // insert right subtree
		return insert_next_val(r, node, node.right(), value);
	
	return -1; // if you reach here, operation failed miserably.
}

int insert_base_val(runic_t &r, superNode node, string value) {
	if (node.empty()){ // if no root exists
		superNode newNode(r,value); // allocate one with this value
		if (!newNode.empty()) {
			newNode.set_root(r);
			return newNode.str_loc(); // return it's location
		} else
			return -1; // alloc failed
	}

	// otherwise a root exists, determine left or right
	int dir = memcmp(node.read().c_str(), value.c_str(),
		(value.length() < node.read().length()) ? value.length() : node.read().length());

	if (dir > 0) // if greater than 0, insert left subtree
		return insert_next_val(r, node, node.left(), value, true);
	else // insert right subtree
		return insert_next_val(r, node, node.right(), value);
	
	return -1; // if you reach here, operation failed miserably.
}

void lookup_item(runic_t r, string value) {
	int loc;
	superNode node(r);
	string hash_val = __get_hash_val(value);
	if ((loc = lookup_next_val(node, hash_val)) != -1)
		printf("YES @ %d\n", loc);
	else // doesnt exist or otherwise mal-formed
		printf("NO\n");
}

int lookup_next_val(superNode node, string value) {
	if (node.empty())  // no root node means no value
		return -1;

	// otherwise a root exists, determine left or right
	int dir = memcmp(node.read().c_str(), value.c_str(),
		(value.length() < node.read().length()) ? value.length() : node.read().length());

	if (dir == 0) // root is the correct node
		return node.str_loc();
	else if (dir > 0) // left is the correct subtree
		return lookup_next_val(node.left(), value);
	else // right is the correct subtree
		return lookup_next_val(node.right(), value);
}
