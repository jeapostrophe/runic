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
	r.path = path;
	r.mode = mode;
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
	return r;
}

bool runic_close(runic_t r) {
	bool stat = false;
	if (r.base == NULL) {
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
		ro.base = NULL;
		ro.offset = (uint64_t)NULL;
		return ro;
	}
	if (file_ref->root == (uint64_t)NULL) {
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
		return 0;
	} else
		return file_ref->free;
}

uint64_t runic_remaining(runic_t r, bool silent){
	runic_file_t* file_ref = (runic_file_t*)r.base;
	fstat(r.fd, &r.sb);
	int64_t bytes_remain = (r.sb.st_size - file_ref->free);
	double percentage_used = ((double)(file_ref->free))/r.sb.st_size;
	if (r.base == NULL) {
		return 0;
	}
	if(!silent) {
		printf("This file has %lld total bytes, with %lld bytes free.\n", r.sb.st_size, bytes_remain);
		printf("Totaling %.2F %c used.\n", percentage_used, (37)); // max file size by stat is 8k PiB, 37 is % symbol
	}
	return bytes_remain;
}

//// node
runic_obj_ty_t runic_obj_ty(runic_obj_t ro) {
	runic_obj_fwdptr_t* fptr_ref;
	runic_obj_atom_t* atom_ref;
	runic_obj_node_t* node_ref;
	if (ro.base == NULL || ro.offset < DEFAULT_ROOT) {
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
		ret.base = NULL;
		ret.offset = (uint64_t)NULL;
		return ret;
	}
	obj_ref = (runic_obj_node_t*)(ro.base + ro.offset);
	ret.offset = obj_ref->left;
	ret.base = ro.base;
	if (obj_ref->left == (uint64_t)NULL) {
		ret.base = NULL;
		ret.offset = (uint64_t)NULL;
	}
	return ret;
}

runic_obj_t runic_node_right(runic_obj_t ro) {
	runic_obj_t ret;
	runic_obj_node_t* obj_ref;
	if (ro.base == NULL || ro.offset < DEFAULT_ROOT) {
		ret.base = NULL;
		ret.offset = (uint64_t)NULL;
		return ret;
	}
	obj_ref = (runic_obj_node_t*)(ro.base + ro.offset);
	ret.offset = obj_ref->right;
	ret.base = ro.base;
	if (obj_ref->right == (uint64_t)NULL) {
		ret.base = NULL;
		ret.offset = (uint64_t)NULL;
	}
	return ret;
}

//// atom
size_t runic_atom_size(runic_obj_t ro) { // returns the size of atom
	runic_obj_atom_t* obj_ref;
	if (ro.base == NULL || ro.offset < DEFAULT_ROOT) {
		return (uint64_t)NULL;
	}
	obj_ref = (runic_obj_atom_t*)(ro.base + ro.offset);
	if (obj_ref->tag == 8) {
		if (strlen(&(obj_ref->value)) > 0) {
			return strlen(&(obj_ref->value));
		}
	}
	return obj_ref->tag;
}

bool runic_atom_read(runic_obj_t ro, char* c) { // returns atom value
	bool stat = false;
	size_t sz = runic_atom_size(ro);
	runic_obj_atom_t* obj_ref;
	if (ro.base == NULL || ro.offset < DEFAULT_ROOT || c == NULL) {
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
		return stat;
	}
	file_ref->root = ro.offset;
	return stat = true;
}

runic_t runic_shrink(runic_t* r) {
	off_t val;
	if (__runic_compact(r) < 0)
		exit(1);
	(*r) = runic_open(r->path, r->mode);
	val = runic_free(*r);
	runic_close(*r);
	truncate(r->path, val+1);
	return runic_open(r->path, r->mode);
}

runic_obj_t runic_alloc_node(runic_t* r) {
	int out;
	runic_obj_t ro;
	runic_file_t* file_ref = (runic_file_t*)r->base;
	runic_obj_node_t* obj_ref;
	if (r->base == NULL) {
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
        out = __runic_compact(r);
		if (out < 0)
			exit(1);
		if (out < (sizeof(runic_obj_node_t)) && r->mode != READONLY) {
			(*r) = runic_open(r->path, r->mode);
			if (!__expand_file(r)) {
				ro.base = NULL;
				ro.offset = (uint64_t)NULL;
				return ro;
			}
		}
		return ro = runic_alloc_node(r);
	}
}

runic_obj_t runic_alloc_atom(runic_t* r, size_t sz) {
	size_t out, tsz = sz;
	runic_obj_t ro;
	runic_file_t* file_ref = (runic_file_t*)r->base;
	runic_obj_atom_t* obj_ref;
	if (r->base == NULL || sz < 0 || sz > MAX_ATOM_SIZE) { // 255 reserved for fptr
		ro.base = NULL;
		ro.offset = (uint64_t)NULL;
		return ro;
	}
	if (sz < sizeof(uint64_t)) { // less than 8 bytes?? (ptr sz)
		tsz = sizeof(uint64_t); // true size
	} // make it 8 bytes
	if (__calc_remaing_space_atom(*r, sz)) {
		ro.base = r->base;
		ro.offset = file_ref->free;
		file_ref->free += (tsz + sizeof(uint8_t));
		obj_ref = (runic_obj_atom_t*)(r->base + ro.offset);
		obj_ref->tag = tsz; // accessable size
		return ro;
	} else {
        out = __runic_compact(r);
		if (out < 0)
			exit(1);
        if (out < (tsz + sizeof(uint8_t)) && r->mode != READONLY) {
            (*r) = runic_open(r->path, r->mode);
			if (!__expand_file(r)) {
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
		return stat;
	}
	obj_ref = (runic_obj_atom_t*)(ro->base + ro->offset);
	sz = runic_atom_size(*ro);
	if (sz >= strlen(val)) {
		memcpy(&obj_ref->value, val, sz);
		return stat = true;
	} else {
		return stat;
	}
}
