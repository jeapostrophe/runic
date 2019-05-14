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
	u_int8_t* addr;
} runic_t;

// TODO: Start by building API based on README.md
// Go function by function. First three functions
// listed.
//
runic_t runic_open(const char* path, int mode);
//
// - Write a function, given the aforementioned
// - signature or similar, that closes a file.
// --- Start with a function that just closes a
// --- file using the mmap protocol.
//
// - Write a function, given the aforementioned
// - signature or similar, that inserts a root
// - node into file based on the passed handle.
// --- Start with a function that just writes to
// --- the passed handle.

#endif /* runic.h */
