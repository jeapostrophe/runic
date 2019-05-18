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

typedef uint8_t /*pointer to a node, not a struct*/ runic_obj_t;

enum runic_file_modes
{
	READONLY, READWRITE, CREATEWRITE
};

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
