#include <iostream>
#include <vector> // vector
#include <cstring> // memcmp, strlen
#include "wlist.h" // wlist
#include "superNode.h" // superNode

using namespace std;

int insert_next_val(runic_t &r, string value) {
	superNode node(r);
	if (node.empty()) {
		node = superNode(r, value);
		return node.str_loc();
	}
	
	int dir = memcmp(node.read().c_str(), value.c_str(),
		(value.length() < node.read().length()) ? value.length() : node.read().length());
	
	if (dir >= 0 || /* balance is unbalanced */1) {
		/* put in left side recursively */
	} else {
		/* put in right side recursively */
	}
	return 0;
	/* balance tree */
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

int lookup_next_val(superNode node, string value) {
	if (node.empty())
		return -1;

	int dir = memcmp(node.read().c_str(), value.c_str(),
		(value.length() < node.read().length()) ? value.length() : node.read().length());

	if (dir == 0)
		return node.str_loc();
	else if (dir > 0)
		return lookup_next_val(node.left(), value);
	else /* dir < 0 */
		return lookup_next_val(node.right(), value);
}

void lookup_item(runic_t r, string value) {
	int loc;
	superNode node(r);
	if ((loc = lookup_next_val(node, value)) != -1) {
		printf("YES @ %d\n", loc);
	} else {
		// doesnt exist or otherwise mal-formed
		printf("NO\n");
	}
}

