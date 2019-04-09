/****
 * runic.c - API Implementation for .runic 
 * 			 filetype covering opening, closing,
 * 			 and modifying source code files 
 * 			 directly on disk using a tree
 * 			 format.
****/

#include <stdio.h>
#include "runic.h"

// TODO: Start by building API based on README.md
// Go function by function. First three functions
// listed.
//
// - Write a function, given the aforementioned
// - signature or similar, that opens a .runic
// - file with it's contents memory-mapped for
// - use in the application.
// --- Start with a function that just opens a file
// --- using the mmap protocol.
// --- Argument for options should be written? One 
// --- each read_existing/write_existing/create_new?
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