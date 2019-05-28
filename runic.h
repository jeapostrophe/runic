/****
 * runic.h - API for .runic filetype
 * 			 covering opening, closing, and 
 * 			 modifying source code files 
 * 			 directly on disk using a tree
 * 			 format.
****/

#include <sys/stat.h> // struct stat

#ifndef RUNIC_H
#define RUNIC_H

#ifdef __cplusplus
	extern "C" {
#endif

#define OFFSET 0x05
#define NODE_SIZE 0x00

typedef struct runic
{
	int fd;
	struct stat sb;
	uint8_t* mmap_addr; //addr in memory
	uint64_t* last_addr; // addr in file
} runic_t;

enum runic_file_modes
{
	READONLY, READWRITE, CREATEWRITE
};

typedef struct runic_obj_atom
{
	char* value;  // ALWAYS a node
} runic_obj_atom_t;

typedef struct runic_obj_node
{
	char* value;
	struct runic_obj_node* left_child;  // ALWAYS a node
	runic_obj_atom_t* right_child; // node, atom or NULL
} runic_obj_node_t;
	
runic_t runic_open(const char* path, int mode);
void ___runic_open_on_args(runic_t* ro, const char* path, int open_flags, int share_flags, int prot_flags, int map_mode );

void runic_close(runic_t runic_file);

runic_obj_node_t* runic_alloc_node(runic_t* ro);
uint64_t* ___runic_search_all_nodes(runic_obj_node_t* rn);

// - Write a function, given the aforementioned
// - signature or similar, that inserts a root
// - node into file based on the passed handle.
// --- Start with a function that just writes to
// --- the passed handle.

#ifdef __cplusplus
	}
#endif

#endif /* runic.h */
