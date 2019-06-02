/****
 * runic.h - API for .runic filetype
 * 			 covering opening, closing, and 
 * 			 modifying source code files 
 * 			 directly on disk using a tree
 * 			 format.
****/

#include <stdbool.h> // bool
#include <sys/stat.h> // struct stat

#ifndef RUNIC_H
#define RUNIC_H

#ifdef __cplusplus
	extern "C" {
#endif

#define ROOT 0x15
#define OFFSET 0x05
#define NODE_SIZE 0x04
#define NODE_TAG 0x00
#define NODE_LEFT_OFFSET 0x04
#define NODE_RIGHT_OFFSET 0x08

typedef struct runic_core
{
	int fd;
	struct stat sb;
	uint8_t* base;
} runic_core_t;

enum runic_file_modes
{
	READONLY, READWRITE, CREATEWRITE
};

typedef struct runic_file
{
	uint64_t root; // 8 bytes, value is always 21
	uint64_t free; // 8 bytes, value is the address of free
} runic_file_t;

typedef struct runic_obj_atom
{
	uint16_t tag; // 2 bytes, value is n < 65,536
	char* value; // 1*n bytes
} runic_obj_atom_t;

typedef struct runic_obj_node
{
	uint16_t tag; // 2 bytes, value is 0
	uint8_t left_child_offset;  // 1 byte, value is a 4 byte offset (next node)
	uint8_t right_child_offset; // 1 byte, value is an 8 byte offset (next node, atom, or null)
} runic_obj_node_t;
	
runic_core_t runic_open(const char* path, int mode);
void ___runic_open_on_args(runic_core_t* ro, const char* path, int open_flags, int share_flags, int prot_flags, int map_mode );

void runic_close(runic_core_t runic_file);

runic_obj_node_t* runic_alloc_node(runic_core_t* ro);
bool ___calc_remaing_space(runic_core_t rn);


// - Write a function, given the aforementioned
// - signature or similar, that inserts a root
// - node into file based on the passed handle.
// --- Start with a function that just writes to
// --- the passed handle.

#ifdef __cplusplus
	}
#endif

#endif /* runic.h */
