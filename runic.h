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

typedef struct runic
{
	int fd;
	struct stat sb;
	uint8_t* addr;
} runic_t;

enum runic_file_modes
{
	READONLY, READWRITE, CREATEWRITE
};

typedef enum runic_obj_ty
{
	NODE, ATOM
} runic_obj_ty_t;

typedef struct runic_obj  // Do not use
{
	runic_obj_ty_t node;
	struct runic_obj* left_child;  // ALWAYS a node
	struct runic_obj* right_child; // node, atom or NULL
} runic_obj_t;

typedef struct runic_obj_node
{
	runic_obj_ty_t node;
	struct runic_obj_node* left_child;  // ALWAYS a node
	struct runic_obj* right_child; // node, atom or NULL
} runic_obj_node_t;

typedef struct runic_obj_atom
{
	runic_obj_ty_t node;
	char* value;  // ALWAYS a node
	struct runic_obj* right_child; // node, atom or NULL
} runic_obj_atom_t;
	
runic_t runic_open(const char* path, int mode);
void ___runic_open_on_args(runic_t* ro, const char* path, int open_flags, int permissions_flags, int prot_flags, int map_mode );

void runic_close(runic_t runic_file);

runic_obj_t* runic_alloc_node(runic_t ro);

// - Write a function, given the aforementioned
// - signature or similar, that inserts a root
// - node into file based on the passed handle.
// --- Start with a function that just writes to
// --- the passed handle.

#ifdef __cplusplus
	}
#endif

#endif /* runic.h */
