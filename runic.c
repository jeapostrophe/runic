/****
 * runic.c - API Implementation for .runic 
 * 			 filetype covering opening, closing,
 * 			 and modifying source code files 
 * 			 directly on disk using a tree
 * 			 format.
****/

// dependencies
#include <stdio.h> // perror
#include <stdlib.h> // exit
// #include <stddef.h> // ptrdiff_t
#include <stdbool.h> // bool
#include <string.h> // strcmp
#include <fcntl.h> // open flags
#include <unistd.h> // close, sysconf
#include <sys/stat.h> // open, fstat
#include <sys/mman.h> // mmap, munmap
#include "runic.h" // runic

// preprocessor statements
#pragma pack(push)
#pragma pack(1) // file should be byte-aligned by 1 byte (no padding)

// structs
typedef struct runic_file {
	char header[HEADER_SIZE]; // 5 bytes, is the magic number ("RUNIC")
	uint64_t root; // 8 bytes, min is 21 (5+8+8), default is null
	uint64_t free; // 8 bytes, value is the address after last node/atom
} runic_file_t;

typedef struct runic_obj_node {
	uint8_t tag; // 1 byte, value is 0
	uint64_t left;  // 8 bytes, default is null
	uint64_t right; // 8 bytes, default is null
} runic_obj_node_t;

typedef struct runic_obj_atom {
	uint8_t tag; // 1 byte, value is n < 255
	char* value; // 1 * n bytes
} runic_obj_atom_t;

// accessors
//// file
void ___runic_open_on_args(runic_t* r, const char* path, int open_flags,
	int share_flags, int prot_flags, int map_mode)
{
	if ((r->fd = open(path, open_flags, share_flags)) == -1) {
		perror("File open failed.\n");
		exit(1);
	}
	if (fstat(r->fd, &(r->sb)) == -1) {
		close(r->fd);
		perror("File access corrupted, couldn't get filesize.\n");
		exit(1);
	}
	if ((r->base = mmap(NULL, (r->sb.st_size ? r->sb.st_size : 1), // arg 2 != 0
		prot_flags, map_mode, r->fd, 0)) == MAP_FAILED) // creates file & map
	{
		close(r->fd);
		perror("Mmap failed.\n");
		exit(1);
	}
	if (open_flags & O_CREAT) { // generates file space
		runic_close(*r); // close mmap and file
		if ((r->fd = open(path, open_flags, share_flags)) == -1) { // reopen
			perror("File open failed.\n");
			exit(1);
		}
		write(r->fd, "\0", sysconf(_SC_PAGESIZE)); // write into file (4K)
		if ((r->base = mmap(NULL, sysconf(_SC_PAGESIZE), // mmap file (4K)
			prot_flags, map_mode, r->fd, 0)) == MAP_FAILED) 
		{
			close(r->fd);
			perror("Mmap failed.\n");
			exit(1);
		}
		fstat(r->fd, &(r->sb)); // stat file (should be 4K)
		strcpy((char*)(r->base), "RUNIC"); // insert magic number and return
		((runic_file_t*)r->base)->root = (uint64_t)NULL; // set first node
		((runic_file_t*)r->base)->free = DEFAULT_ROOT; // start of free
	} else {
		if (strcmp((char*)r->base, "RUNIC") != 0) {
			runic_close(*r);
			perror("File is not a runic file.\n");
			exit(1);
		}
	}
	// TODO: handle errors better than exit(1)
}

runic_t runic_open(const char* path, int mode) { // returns null on failure, otherwise returns the runic
	runic_t r;
	// do some checks for invalid path or mode
	switch (mode) {
	case READONLY:
		___runic_open_on_args(&r, path, O_RDONLY,
			S_IRUSR | S_IRGRP | S_IROTH, PROT_READ, MAP_PRIVATE);
			// Permissions: r-- r-- r--
		break;
	case READWRITE:
		___runic_open_on_args(&r, path, O_RDWR,
			S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH, PROT_READ | PROT_WRITE, MAP_SHARED);
			// Permissions: rw- r-- r--
		break;
	case CREATEWRITE:
	default:
		___runic_open_on_args(&r, path, O_RDWR | O_CREAT,
			S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH, PROT_READ | PROT_WRITE, MAP_SHARED);
			// Permissions: rw- r-- r-- (Create)
		break;
	}
	return r;
}

void runic_close(runic_t r) { // closes the file
	// bool stat = false;
	// do some checks for invalid object
	munmap(r.base, r.sb.st_size);
	close(r.fd);
	return;
}

runic_obj_t runic_root(runic_t r) { // returns root node
	// check for errors
	runic_obj_t ro;
	ro.offset = ((runic_file_t*)r.base)->root;
	ro.base = r.base;
	return ro;
}

//// node
runic_obj_ty_t runic_obj_ty(runic_obj_t ro) { // returns type of target object
	// check for errors
	if (((runic_obj_atom_t*)ro.base + ro.offset)->tag) {
		return ATOM;
	}
	if (!((runic_obj_node_t*)ro.base + ro.offset)->tag) {
		return NODE;
	}
	return (runic_obj_ty_t)NULL;
}

runic_obj_t runic_node_left(runic_obj_t ro) {
	// check for errors
	runic_obj_t ret;
	ret.offset = ((runic_obj_node_t*)ro.base + ro.offset)->left;
	ret.base = ro.base;
	return ret;
}

runic_obj_t runic_node_right(runic_obj_t ro) {
	// check for errors
	runic_obj_t ret;
	ret.offset = ((runic_obj_node_t*)ro.base + ro.offset)->right;
	ret.base = ro.base;
	return ret;
}

//// atom
size_t runic_atom_size(runic_obj_t ro) { // returns the size of atom
	// check for errors
	return ((runic_obj_atom_t*)ro.base + ro.offset)->tag;
}

const char* runic_atom_read(runic_obj_t ro) { // returns atom value
	// check for errors
	return ((runic_obj_atom_t*)ro.base + ro.offset)->value;
}

// mutators
//// file
void runic_set_root(runic_t* r, runic_obj_t* ro) { // returns false on failure
	// bool stat = false;
	// check for errors
	((runic_file_t*)r->base)->root = ro->offset;
	return;
}

bool ___calc_remaing_space(runic_t r) {
	bool stat = false;
	uint64_t free, file_size;
	free = ((runic_file_t*)r.base)->free;
	file_size = r.sb.st_size;
	if ((free + NODE_SIZE) < file_size) {
		stat = true;
	}
	return stat;
}

runic_obj_t runic_alloc_node(runic_t* r) { // returns null on failure, otherwise addr
	runic_obj_t ro;
	// do some checks for invalid object
	if (___calc_remaing_space(*r)) {
		ro.base = r->base;
		ro.offset = (uint64_t)(r->base + (uint64_t)((runic_file_t*)r->base)->free);
		((runic_file_t*)r->base)->free += NODE_SIZE;
		((runic_obj_node_t*)ro.base + ro.offset)->tag = NODE_TAG_VALUE;
		((runic_obj_node_t*)ro.base + ro.offset)->left = (uint64_t)NULL;
		((runic_obj_node_t*)ro.base + ro.offset)->right = (uint64_t)NULL;
		return ro;
	} else {
		perror("Not enough space.\n");
		exit(1);
		// TODO: Change this to garbage collect, attempt again.
		// If gc fails to create enough space, then un-map, add space, remap.
	}
}

bool ___calc_remaing_space_atom(runic_t r, size_t size) {
	bool stat = false;
	uint64_t free, file_size;
	free = ((runic_file_t*)r.base)->free;
	file_size = r.sb.st_size;
	if ((free + size + TAG_SIZE) < file_size) {
		stat = true;
	}
	return stat;
}

runic_obj_t runic_alloc_atom(runic_t* r, size_t sz) {  // returns null on fail
	runic_obj_t ro;
	// do some checks for invalid size (negative, greater than 255)
	// do some checks for invalid object
	if (___calc_remaing_space_atom(*r, sz))
	{
		ro.base = r->base;
		ro.offset = (uint64_t)(r->base + (uint64_t)((runic_file_t*)r->base)->free);
		((runic_file_t*)r->base)->free += sz + TAG_SIZE;
		((runic_obj_atom_t*)ro.base + ro.offset)->tag = sz;
		return ro;
	}
    else
    {
        perror("Not enough space.\n");
        exit(1);
		// TODO: Change this to garbage collect, attempt again.
		// If gc fails to create enough space, then un-map, add space, remap.
	}
}

//// node
void runic_node_set_left(runic_obj_t parent, runic_obj_t child) {
	// do some checks
	((runic_obj_node_t*)parent.base + parent.offset)->left =
		child.offset;
}
void runic_node_set_right(runic_obj_t parent, runic_obj_t child) {
	// do some checks
	((runic_obj_node_t*)parent.base + parent.offset)->right =
		child.offset;
}

//// atom
void runic_atom_write(runic_obj_t* ro, const char* value) {
	// do some checks for value size, as we dont want to overwrite
	((runic_obj_atom_t*)ro->base + ro->offset)->value = (char*)value;
	return;
}

// closing statements
#pragma pack(pop)
