/****
 * runic_hidden.h - Hidden operations for runic
 *					that are never user exposed
 * 
****/

// dependencies
#include "runic.h"  // runic header
// additional dependencies can be found in runic.h

// preprocessor statements
#ifndef RUNIC_HIDDEN_H
#define RUNIC_HIDDEN_H

// constants
#define HEADER_SIZE 0x05
#define NODE_TAG_VALUE 0x00
#define FPTR_TAG_VALUE 0xFF
#define MAX_ATOM_SIZE 0xFE
#define OP_FAIL_CODE (-1)
#define FWD_PTR 0x03

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
	char value; // 1 * n bytes
} runic_obj_atom_t;

typedef struct runic_obj_fwdptr {
	uint8_t tag;
	uint64_t fwdaddr;
} runic_obj_fwdptr_t;

bool __runic_open_existing(runic_t* r, const char* path, int open_flags,
	int share_flags, int prot_flags, int map_mode)
{
	if ((r->fd = open(path, open_flags, share_flags)) != OP_FAIL_CODE) {
		if (fstat(r->fd, &(r->sb)) != OP_FAIL_CODE &&
			(r->base = mmap(r->base, r->sb.st_size, prot_flags, map_mode, r->fd, 0)) != NULL)
		{
			// file, fstat, and mmap all worked
			return true;
		} else {
			// file opened, but fstat or mmap failed
			close(r->fd);
			return false;
		}
	} else {
		// file did not open
		return false;
	}
}

bool __runic_open_new(runic_t* r, const char* path, int open_flags,
	int share_flags, int prot_flags, int map_mode)
{
	if ((r->fd = open(path, open_flags)) != OP_FAIL_CODE) {
		if (fstat(r->fd, &(r->sb)) != OP_FAIL_CODE &&
			(r->base = mmap(r->base, r->sb.st_size ? 1 : r->sb.st_size, prot_flags, map_mode, r->fd, 0)) != NULL)
		{
			// do stuff here to make sure file is atleast 4kb when loaded
			return true;
		} else {
			close(r->fd);
			return false;
		}	
	} else {
		return false;
	}
}

// "allocs" a fptr. really this just writes over a known memory location.
// there is no "undo" for this, use with caution.
runic_obj_t __runic_alloc_fwdptr(runic_obj_t ro, uint64_t addr) {
	runic_obj_fwdptr_t* fptr_ref = (runic_obj_fwdptr_t*)(ro.base+ro.offset);
	fptr_ref->tag = FPTR_TAG_VALUE;
	fptr_ref->fwdaddr = addr;
	return ro;
}

// returns a fptr addr when given a runic obj
uint64_t __runic_fwd_addr(runic_obj_t ro) {
	runic_obj_fwdptr_t* fptr_ref = (runic_obj_fwdptr_t*)(ro.base+ro.offset);
	return fptr_ref->fwdaddr;
}

runic_obj_t __runic_lookback_left(runic_t* r, runic_t* rn, runic_obj_t ro) {
	// gets the child node ptr from the old file using the new file & obj
	runic_obj_t ro_1;
	ro_1.base = NULL;
	return ro_1;
}

bool __runic_obj_node_cpy(runic_t src, runic_t dest, uint64_t src_offset) {
	// cpys a node over to new file
	return false;
}

runic_obj_t __runic_lookback_right(runic_t* r, runic_t* rn, runic_obj_t ro) {
	// gets the child node ptr from the old file using the new file & obj
	runic_obj_t ro_1;
	ro_1.base = NULL;
	return ro_1;
}

uint64_t __runic_move(runic_t* r, runic_t* rn, runic_obj_t ro) {
	// moves a node or atom over
	return 0;
}

bool __runic_move_children(runic_t* r, runic_t* rn, runic_obj_t ro) {
	// moves a pair of children from the old file, called by move
	return false;
}

int __runic_doscan(runic_t* r, runic_t* rn) {
	// runs the whole algorithm
	return 0;
}

// takes a runic file and determines if enough space exists to add atleast 1 NODE
bool __calc_remaing_space(runic_t r) {
	runic_file_t* file_ref = (runic_file_t*)r.base;
	return ((file_ref->free + sizeof(runic_obj_node_t)) < r.sb.st_size);
}

int __runic_compact(runic_t* r) {
	// creates a new file, runs doscan
	// and returns freed memory as a new file
	return 0;
}

bool __expand_file(runic_t* r) {
	// garbage collects memory, and then expands file if necessary
	return false;
}

bool __calc_remaing_space_atom(runic_t r, size_t size) {
	// calculates the remaining space in an atom
	return false;
}

// closing statements
#pragma pack(pop)
#endif /* runic_hidden.h */
