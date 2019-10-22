#include <iostream>
#include <string> // string
#include "../../runic/runic.h"  // runic

// preprocessor statements
#ifndef SUPERNODE_H
#define SUPERNODE_H

#define SUPERNODE_MEMORY_SIZE 0x23

using namespace std;

class superNode {
public:
	superNode(runic_t &r, string input); // allocs a super node in file space
	superNode(runic_t &r); // returns the root super node in memory

	superNode left() const; // gives right child
	superNode right() const; // gives left child
	string read() const; // gives atom
	int bf() const;	// returns current bf
	bool empty() const; // returns safety check on emptiness
	int str_loc() const; // location of the string associated with this superNode

	bool balance(); // balances this node and all subnodes.
	bool set_left(superNode left); // attaches another super node to itself as child
	bool set_right(superNode right);  // same
	bool write(string input); // alters existing atom value
	bool set_root(runic_t &r);  // attaches itself to file as root

	~superNode();

private:
	runic_obj_t super_node, atom, children; // the root node, value, and child carrier
	bool isempty; // variable for empty (meaningless) superNodes
	int balance_factor; // balance factor of tree

	superNode(); // generates an empty super node
	superNode(runic_obj_t node); // generates a super node from existing node data in file

	bool set_bf(int new_bf); // sets balance factor of tree
};

/*
	Tree Format:
	Each atom is attached as the right child of every other node.
	All accessors and mutators therefore need to make nested calls.
	We modify every other node to manipulate the overall tree,
	but not the individual tree layers upon which the atom resides.
            ... ^ predecessors
          s_Node (L)    ...    s_Node (R)
           /   \                 ...
       child   Atom              ...
       /   \
 s_Node(L)   s_Node(R)
   ...         ...   V successors
 */

#endif /* superNode.h */