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
#pragma pack(push)
#pragma pack(1) // file should be byte-aligned by 1 byte (no padding)

// constants
#define DEFAULT_ROOT 0x15
#define HEADER_SIZE 0x05
#define NODE_SIZE 0x11
#define TAG_SIZE 0x01
#define POINTER_SIZE 0x08
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
void runic_close(runic_t r); // closes the file
runic_obj_t runic_root(runic_t r); // returns root node

//// node
runic_obj_ty_t runic_obj_ty(runic_obj_t ro); // returns type of target object
runic_obj_t runic_node_left(runic_obj_t ro);
runic_obj_t runic_node_right(runic_obj_t ro);

//// atom
size_t runic_atom_size(runic_obj_t ro); // returns the size of atom
const char* runic_atom_read(runic_obj_t ro); // returns atom value

// mutators
//// file
void runic_set_root(runic_t* r, runic_obj_t* ro);  // returns false on failure
runic_obj_t runic_alloc_node(runic_t* r); // returns null on failure, otherwise, addr
runic_obj_t runic_alloc_atom(runic_t* r, size_t sz);  // returns null on failure

//// node
void runic_node_set_left(runic_obj_t parent, runic_obj_t child);
void runic_node_set_right(runic_obj_t parent, runic_obj_t child);

//// atom
void runic_atom_write(runic_obj_t* ro, const char* value);

// closing statements
#pragma pack(pop)
#ifdef __cplusplus
	}
#endif
#endif /* runic.h */
