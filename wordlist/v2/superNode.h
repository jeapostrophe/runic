#include <iostream>
#include <string> // string
#include "../../runic/runic.h"  // runic

// preprocessor statements
#ifndef SUPERNODE_H
#define SUPERNODE_H

using namespace std;

class superNode {
public:
	superNode(runic_t &r, string input); // allocs a super node in file space
	superNode(runic_t &r); // returns the root super node in memory

	superNode left(); // gives right child
	superNode right(); // gives left child
	string read(); // gives atom
	int bf();	// returns current bf
	bool empty(); // returns safety check on emptiness
	int str_loc();

	bool set_bf(int new_bf);
	bool set_left(superNode left); // attaches another super node to itself as child
	bool set_right(superNode right);  // same
	bool write(string input); // alters existing atom value
	bool set_root(runic_t &r);  // attaches itself to file as root

	~superNode();

private:
	runic_obj_t super_node, atom, child_carrier;
	bool isempty; // variable for empty (meaningless) superNodes
	int balance_factor;
	superNode(runic_obj_t node); // generates a super node from existing node data in file
	superNode(); // generates an empty super node
};

/*
	Tree Format:
	Each atom is attached as the right child of every other node.
	All accessors and mutators therefore need to make nested calls.
	We modify every other node to manipulate the overall tree,
	but not the individual tree layers upon which the atom resides.
			... ^ predecessors
		  s_Node (L)	...		s_Node (R)
			/ \					...
	   child   Atom				...
		/   \
 s_Node(L)  s_Node(R)
   ...		...  V successors
 */

#endif /* superNode.h */