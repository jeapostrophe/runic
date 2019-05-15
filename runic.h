/****
 * runic.h - API for .runic filetype
 * 			 covering opening, closing, and 
 * 			 modifying source code files 
 * 			 directly on disk using a tree
 * 			 format.
****/

#include <stdio.h>

#ifndef RUNIC_H
#define RUNIC_H

typedef struct runic
{
	int fd;
	struct stat sb;
	uint8_t* addr;
} runic_t;

// incomplete. all c structs require one named member.
typedef struct runic_obj
{
	int temp;
} runic_obj_t;

enum runic_file_modes // enums or #define?? it looks like #define is used for other flags
{
	readOnly, readWrite
};

// TODO: Start by building API based on README.md
// Go function by function. First three functions
// listed.

runic_t runic_open(const char* path, int mode);
void mmap_failed(runic_t ro);
void safe_fstat(runic_t* runic_file);

void runic_close(runic_t runic_file);

// alloc'ing a node is a matter of alloc'ing file space for the node if none exists
// remember these are mmap'd files
// should it return an obj_t, or just alloc the space?
runic_obj_t runic_alloc_node(runic_t ro);

// - Write a function, given the aforementioned
// - signature or similar, that inserts a root
// - node into file based on the passed handle.
// --- Start with a function that just writes to
// --- the passed handle.

#endif /* runic.h */
