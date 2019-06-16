/****
 * runic.h - API for .runic filetype
 * 			 covering opening, closing, and 
 * 			 modifying source code files 
 * 			 directly on disk using a tree
 * 			 format.
****/

// dependencies
#include <stdbool.h> // bool
#include <sys/stat.h> // struct stat

// preprocessor statements
#ifndef RUNIC_H
#define RUNIC_H
#ifdef __cplusplus
	extern "C" {
#endif

// constants
#define DEFAULT_ROOT 0x15
#define HEADER_SIZE 0x05
#define NODE_TAG_VALUE 0x00

// enums
enum runic_file_modes
{
	READONLY, READWRITE, CREATEWRITE
};

typedef enum runic_obj_ty
{
	NODE, ATOM
} runic_obj_ty_t;

// structs
typedef struct runic
{
	int fd;
	struct stat sb;
	uint8_t* base;
} runic_t;

typedef struct runic_obj
{
	uint8_t* base;
	uint64_t offset;
} runic_obj_t;

// accessors
//// file
runic_t runic_open(const char* path, int mode); // returns null on failure, otherwise returns runic
bool runic_close(runic_t r); // closes the file
runic_obj_t runic_root(runic_t r); // returns root node

//// node
runic_obj_ty_t runic_obj_ty(runic_obj_t ro); // returns type of target object
runic_obj_t runic_node_left(runic_obj_t ro); // returns obj
runic_obj_t runic_node_right(runic_obj_t ro); // returns obj

//// atom
size_t runic_atom_size(runic_obj_t ro); // returns the size of atom
bool runic_atom_read(runic_obj_t ro, char* c); // returns atom value

// mutators
//// file
bool runic_set_root(runic_t* r, runic_obj_t ro);  // returns false on failure
runic_obj_t runic_alloc_node(runic_t* r); // returns null on failure, otherwise, addr
runic_obj_t runic_alloc_atom(runic_t* r, size_t sz);  // returns null on failure
runic_obj_t runic_alloc_atom_str(runic_t* r, const char* value); // allocs and writes

//// node
bool runic_node_set_left(runic_obj_t* parent, runic_obj_t child); // sets parent->left = child
bool runic_node_set_right(runic_obj_t* parent, runic_obj_t child); // sets parent->right = child

//// atom
bool runic_atom_write(runic_obj_t* ro, const char* val); // sets atom's value

// closing statements
#ifdef __cplusplus
	}
#endif
#endif /* runic.h */
