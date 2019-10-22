/****
 * runic.c - API Implementation for .runic 
 * 			 filetype covering opening, closing,
 * 			 and modifying source code files 
 * 			 directly on disk using a tree
 * 			 format.
****/

// dependencies
#include <stdio.h> // perror, rename, remove
#include <stdlib.h> // exit
#include <stdbool.h> // bool
#include <stddef.h> // size_t
#include <stdint.h> // uintXX_t
#include <string.h> // memcmp, memcpy, strlen
#include <fcntl.h> // open flags
#include <unistd.h> // close, sysconf
#include <sys/stat.h> // struct stat, open, fstat
#include <sys/mman.h> // mmap, munmap, map flags
#include "runic.h"
#include "runic_hidden.h" // hidden functions
// additional dependencies can be found in runic.h

// accessors
//// file
runic_t runic_open(const char* path, int mode) {
	runic_t r, r_null;
	r_null.base = NULL;
	switch (mode) {
	case READONLY:
		if (!__runic_open_on_args(&r, path, O_RDONLY,
			S_IRUSR | S_IRGRP | S_IROTH, PROT_READ, MAP_PRIVATE)) {
			return r_null;
		}
		break;
	case READWRITE:
		if (!__runic_open_on_args(&r, path, O_RDWR,
			S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH, PROT_READ | PROT_WRITE, MAP_SHARED)) {
			return r_null;
		}
		break;
	case CREATEWRITE:
	default:
		if (!__runic_open_on_args(&r, path, O_RDWR | O_CREAT,
			S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH, PROT_READ | PROT_WRITE, MAP_SHARED)) {
			return r_null;
		}
		break;
	}
	r.path = path;
	r.mode = mode;
	return r;
}

bool runic_close(runic_t r) {
	bool stat = false;
	if (r.base == NULL) {
		perror("Invalid runic_t object.\n");
		return stat;
	}
	munmap(r.base, r.sb.st_size);
	close(r.fd);
	return stat = true;
}

runic_obj_t runic_root(runic_t r) {
	runic_obj_t ro;
	runic_file_t* file_ref = (runic_file_t*)r.base;
	if (r.base == NULL) {
		perror("Invalid runic_t object.\n");
		ro.base = NULL;
		ro.offset = (uint64_t)NULL;
		return ro;
	}
	if (file_ref->root == (uint64_t)NULL) {
		perror("No root exists for this file.\n");
		ro.base = NULL;
		ro.offset = (uint64_t)NULL;
		return ro;
	} else {
		ro.offset = file_ref->root;
		ro.base = r.base;
		return ro;
	}
}

uint64_t runic_free(runic_t r) {
	runic_file_t* file_ref = (runic_file_t*)r.base;
	if (r.base == NULL) {
		perror("Invalid runic_t object.\n");
		return 0;
	} else
		return file_ref->free;
}

uint64_t runic_remaining(runic_t r, bool silent){
	runic_file_t* file_ref = (runic_file_t*)r.base;
	int64_t bytes_remain = (r.sb.st_size - file_ref->free);
	double percentage_used = ((double)(file_ref->free))/r.sb.st_size;
	if (r.base == NULL) {
		perror("Invalid runic_t object.\n");
		return 0;
	}
	if(!silent) {
		printf("This file has %lld total bytes, with %lld bytes free.\n", r.sb.st_size, bytes_remain);
		printf("Totaling %.2F %c used.", percentage_used, (37)); // max file size by stat is 8k PiB, 37 is % symbol
	}
	return bytes_remain;
}

//// node
runic_obj_ty_t runic_obj_ty(runic_obj_t ro) {
	runic_obj_fwdptr_t* fptr_ref;
	runic_obj_atom_t* atom_ref;
	runic_obj_node_t* node_ref;
	if (ro.base == NULL || ro.offset < DEFAULT_ROOT) {
		perror("Invalid runic_obj_t object.\n");
		return OP_FAIL_CODE;
	}
	fptr_ref = (runic_obj_fwdptr_t*)(ro.base + ro.offset);
	atom_ref = (runic_obj_atom_t*)(ro.base + ro.offset);
	node_ref = (runic_obj_node_t*)(ro.base + ro.offset);
	if (fptr_ref->tag == FPTR_TAG_VALUE) {
		return FWD_PTR;
	}
	if (atom_ref->tag) {
		return ATOM;
	}
	if (!(node_ref->tag)) {
		return NODE;
	}
	perror("Operation critically failed, exiting...\n");
	exit(1);
}

runic_obj_t runic_node_left(runic_obj_t ro) {
	runic_obj_t ret;
	runic_obj_node_t* obj_ref;
	if (ro.base == NULL || ro.offset < DEFAULT_ROOT) {
		perror("Invalid runic_obj_t object.\n");
		ret.base = NULL;
		ret.offset = (uint64_t)NULL;
		return ret;
	}
	obj_ref = (runic_obj_node_t*)(ro.base + ro.offset);
	ret.offset = obj_ref->left;
	ret.base = ro.base;
	if (obj_ref->left == (uint64_t)NULL) {
		perror("No left child exists for this object.\n");
		ret.base = NULL;
		ret.offset = (uint64_t)NULL;
	}
	return ret;
}

runic_obj_t runic_node_right(runic_obj_t ro) {
	runic_obj_t ret;
	runic_obj_node_t* obj_ref;
	if (ro.base == NULL || ro.offset < DEFAULT_ROOT) {
		perror("Invalid runic_obj_t object.\n");
		ret.base = NULL;
		ret.offset = (uint64_t)NULL;
		return ret;
	}
	obj_ref = (runic_obj_node_t*)(ro.base + ro.offset);
	ret.offset = obj_ref->right;
	ret.base = ro.base;
	if (obj_ref->right == (uint64_t)NULL) {
		perror("No right child exists for this object.\n");
		ret.base = NULL;
		ret.offset = (uint64_t)NULL;
	}
	return ret;
}

//// atom
size_t runic_atom_size(runic_obj_t ro) { // returns the size of atom
	runic_obj_atom_t* obj_ref;
	if (ro.base == NULL || ro.offset < DEFAULT_ROOT) {
		perror("Invalid runic_obj_t object.\n");
		return (uint64_t)NULL;
	}
	obj_ref = (runic_obj_atom_t*)(ro.base + ro.offset);
	return obj_ref->tag;
}

bool runic_atom_read(runic_obj_t ro, char* c) { // returns atom value
	bool stat = false;
	size_t sz = runic_atom_size(ro);
	runic_obj_atom_t* obj_ref;
	if (ro.base == NULL || ro.offset < DEFAULT_ROOT || c == NULL) {
		perror("Invalid runic_obj_t object or char pointer.\n");
		return stat;
	}
	obj_ref = (runic_obj_atom_t*)(ro.base + ro.offset);
	memcpy(c, &obj_ref->value, sz);
	return stat = true;
}

// mutators
//// file
bool runic_set_root(runic_t* r, runic_obj_t ro) { // returns false on failure
	bool stat = false;
	runic_file_t* file_ref = (runic_file_t*)r->base;
	if (r->base == NULL || ro.base == NULL || ro.offset < DEFAULT_ROOT) {
		perror("Invalid runic_obj_t or runic_t object.\n");
		return stat;
	}
	file_ref->root = ro.offset;
	return stat = true;
}

runic_t runic_shrink(runic_t* r) {
	int open_flags, share_flags, prot_flags, map_mode;
	runic_t rnull, rn = runic_open("/tmp/tmp1.runic", CREATEWRITE); // open runic file (temp)
	rnull.base = NULL; rnull.path = NULL, rnull.fd = 0; rnull.mode = 0; rnull.sb.st_size = 0;
	if (r->sb.st_size > sysconf(_SC_PAGESIZE)) // if r (runic) is greater than 4k expand the rn (runic new)
	{
		if (runic_close(rn)) {
			perror("File close failed.\n");
			return rnull;
		}
		open_flags = O_RDWR | O_APPEND; // append to this file in read-write
		share_flags = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
		prot_flags = PROT_READ | PROT_WRITE;
		map_mode = MAP_SHARED;
		if ((rn.fd = open(rn.path, open_flags, share_flags)) == OP_FAIL_CODE) { // reopen
			perror("File open failed.\n");
			return rnull;
		}
		write(rn.fd, "\0", (r->sb.st_size - sysconf(_SC_PAGESIZE))); // write into file and match size (size - 4k already wrote)
		if ((rn.base = mmap(NULL, rn.sb.st_size * 2, // mmap file with new, proper size
			prot_flags, map_mode, rn.fd, 0)) == MAP_FAILED) 
		{
			close(rn.fd);
			perror("Mmap failed.\n");
		}
	}
	if (!__runic_doscan(r, &rn)) {
		perror("File .\n");
		return rnull;
	}
	if (!runic_close(*r) && !runic_close(rn)) {
		perror("File close failed.\n");
		return rnull;
	}
	if (remove(r->path) == OP_FAIL_CODE) { // remove old file
		perror("File removal failed.\n");
		return rnull;
	}
	if (rename(rn.path, r->path) == OP_FAIL_CODE) { // rename and replace new file into old path
		perror("File rename failed.\n");
		return rnull;
	}
	rn.path = r->path; // path should now properly reflect the path
	if (__runic_compact(&rn))
		return runic_open(rn.path, rn.mode);
	else
		return rnull; // shrink down to free size
}

runic_obj_t runic_alloc_node(runic_t* r) {
	runic_obj_t ro;
	runic_file_t* file_ref = (runic_file_t*)r->base;
	runic_obj_node_t* obj_ref;
	if (r->base == NULL) {
		perror("Invalid runic_t object.\n");
		ro.base = NULL;
		ro.offset = (uint64_t)NULL;
		return ro;
	}
	if (__calc_remaing_space(*r)) {
		ro.base = r->base;
		ro.offset = file_ref->free;
		file_ref->free += sizeof(runic_obj_node_t);
		obj_ref = (runic_obj_node_t*)(r->base + ro.offset);
		obj_ref->tag = NODE_TAG_VALUE;
		obj_ref->left = (uint64_t)NULL;
		obj_ref->right = (uint64_t)NULL;
		return ro;
	} else {
		if (__garbage_collect() < sizeof(runic_obj_node_t) && r->mode != READONLY) {
			if (!__expand_file(r)) {
				perror("Not enough space.\n");
				ro.base = NULL;
				ro.offset = (uint64_t)NULL;
				return ro;
			}
		}
		return ro = runic_alloc_node(r);
	}
}

runic_obj_t runic_alloc_atom(runic_t* r, size_t sz) {
	size_t asz, tsz;
	runic_obj_t ro;
	runic_file_t* file_ref = (runic_file_t*)r->base;
	runic_obj_atom_t* obj_ref;
	if (r->base == NULL || sz < 0 || sz > MAX_ATOM_SIZE) { // 255 reserved for fptr
		perror("Invalid runic_t object or size parameter.\n");
		ro.base = NULL;
		ro.offset = (uint64_t)NULL;
		return ro;
	}
	if (sz < sizeof(uint64_t)) { // less than 8 bytes?? (ptr sz)
		asz = sz;
		tsz = sizeof(uint64_t);
	} // make it 8 bytes
	if (__calc_remaing_space_atom(*r, sz)) {
		ro.base = r->base;
		ro.offset = file_ref->free;
		file_ref->free += (tsz + sizeof(uint8_t));
		obj_ref = (runic_obj_atom_t*)(r->base + ro.offset);
		obj_ref->tag = asz; // accessable size
		return ro;
	} else {
		if (__garbage_collect() < (tsz + sizeof(uint8_t)) && r->mode != READONLY) {
			if (!__expand_file(r)) {
				perror("Not enough space.\n");
				ro.base = NULL;
				ro.offset = (uint64_t)NULL;
				return ro;
			}
		}
		return ro = runic_alloc_atom(r, tsz);
	}
}

runic_obj_t runic_alloc_atom_str(runic_t* r, const char* value) {
	size_t sz;
	runic_obj_t ro;
	sz = strlen(value);
	ro = runic_alloc_atom(r, sz);
	runic_atom_write(&ro, value);
	return ro;
}

//// node
bool runic_node_set_left(runic_obj_t* parent, runic_obj_t child) {
	bool stat = false;
	if (parent->base == NULL || parent->offset < DEFAULT_ROOT
		|| child.base == NULL || child.offset < DEFAULT_ROOT)
	{
		perror("Invalid runic_obj_t object.\n");
		return stat;
	}
	runic_obj_node_t* parent_ref = (runic_obj_node_t*)(parent->base + parent->offset);
	parent_ref->left = child.offset;
	return stat = true;
}
bool runic_node_set_right(runic_obj_t* parent, runic_obj_t child) {
	bool stat = false;
	if (parent->base == NULL || parent->offset < DEFAULT_ROOT
		|| child.base == NULL || child.offset < DEFAULT_ROOT)
	{
		perror("Invalid runic_obj_t object.\n");
		return stat;
	}
	runic_obj_node_t* parent_ref = (runic_obj_node_t*)(parent->base + parent->offset);
	parent_ref->right = child.offset;
	return stat = true;
}

//// atom
bool runic_atom_write(runic_obj_t* ro, const char* val) {
	size_t sz;
	bool stat = false;
	runic_obj_atom_t* obj_ref;
	if (ro->base == NULL || ro->offset < DEFAULT_ROOT || val == NULL) {
		perror("Invalid runic_obj_t object or char pointer.\n");
		return stat;
	}
	obj_ref = (runic_obj_atom_t*)(ro->base + ro->offset);
	sz = runic_atom_size(*ro);
	if (sz <= strlen(val)) {
		memcpy(&obj_ref->value, val, sz);
		return stat = true;
	} else {
		perror("String size exceeds atom size.\n");
		return stat;
	}
}
