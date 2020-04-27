/****
 * runic.c - API Implementation for .runic 
 * 			 filetype covering opening, closing,
 * 			 and modifying source code files 
 * 			 directly on disk using a tree
 * 			 format.
****/

// dependencies
#include "runic.h"
#include "runic_hidden.h" // hidden functions
// additional dependencies can be found in runic.h

// accessors
//// file

///Opens the file using a known path string and a designated file mode
///path: A known file path
///mode: A value from the runic_file_modes enum
runic_t runic_open(const char* path, runic_file_modes_t mode) {
	runic_t r, r_NULL;
	r_NULL.base = NULL;
	r.path = path;
	r.mode = mode;
	switch (mode) {
		case READONLY:
			if (!__runic_open_existing(&r, path, O_RDONLY,
				S_IRUSR | S_IRGRP | S_IROTH, PROT_READ, MAP_PRIVATE)) {
					// R--R--R-- (444)
				return r_NULL;
			}
			break;
		case READWRITE:
			if (!__runic_open_existing(&r, path, O_RDWR,
				S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH, PROT_READ | PROT_WRITE, MAP_SHARED)) {
					// RW-R--R-- (644)
				return r_NULL;
			}
			break;
		case CREATEWRITE:
		default:
			// this case WILL destroy any existing data at this filepath!!!
			if (!__runic_open_new(&r, path, O_RDWR | O_CREAT | O_TRUNC,
				S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH, PROT_READ | PROT_WRITE, MAP_SHARED)) {
					// RW-R--R-- (644)
				return r_NULL;
			}
			break;
	}
	return r;
}

///Closes a runic file associated with the runic_t
///r: An open runic_t file
bool runic_close(runic_t r) {
	if (r.base == NULL) {
		return false;
	}

	if (munmap(r.base, r.sb.st_size) == 0
		&& close(r.fd) == 0)
	{
		return true;
	}

	// if the function fails to properly unmap or close the file, the program must exit.
	exit(1);
}

runic_obj_t runic_root(runic_t r) {
	runic_file_t* file_ref = (runic_file_t*)r.base;
	runic_obj_t ro, ro_NULL;
	if (r.base == NULL || file_ref->root == (uint64_t)NULL) {
		ro_NULL.base = NULL;
		ro_NULL.offset = (uint64_t)NULL;
		return ro_NULL;
	} else {  // file seems legit
		ro.offset = file_ref->root;
		ro.base = r.base;
		return ro;
	}
}

uint64_t runic_free(runic_t r) {
	runic_file_t* file_ref = (runic_file_t*)r.base;
	if (r.base == NULL) {
		return 0;
	} else {
		return file_ref->free;
	}
}

uint64_t runic_remaining(runic_t r, bool silent){
	runic_file_t* file_ref = (runic_file_t*)r.base;
	int64_t bytes_remain;
	double percentage_used;
	if (r.base == NULL || fstat(r.fd, &r.sb) == OP_FAIL_CODE) {
		// bad object
		return 0;
	}

	bytes_remain = (r.sb.st_size - file_ref->free);
	percentage_used = ((double)(file_ref->free))/r.sb.st_size;
	if(!silent) {
		printf("This file has %lld total bytes, with %lld bytes free.\n", r.sb.st_size, bytes_remain);
		printf("Totaling %.2F %c used.\n", percentage_used, (37)); // max file size by stat is 8k PiB, 37 is % symbol
	}
	return bytes_remain;
}

//// node
runic_obj_ty_t runic_obj_ty(runic_obj_t ro) {
	runic_obj_fwdptr_t* fptr_ref = (runic_obj_fwdptr_t*)(ro.base + ro.offset);
	runic_obj_atom_t* atom_ref = (runic_obj_atom_t*)(ro.base + ro.offset);
	runic_obj_node_t* node_ref = (runic_obj_node_t*)(ro.base + ro.offset);
	if (ro.base == NULL || ro.offset < DEFAULT_ROOT) {
		return OP_FAIL_CODE;
	}
	if (fptr_ref->tag == FPTR_TAG_VALUE) {
		return FWD_PTR;
	}
	if (atom_ref->tag) {
		return ATOM;
	}
	if (!(node_ref->tag)) {
		return NODE;
	}

	// if none of the if statements return, the program must exit
	exit(1);
}

runic_obj_t runic_node_left(runic_obj_t ro) {
	runic_obj_node_t* obj_ref = (runic_obj_node_t*)(ro.base + ro.offset);
	runic_obj_t ret;
	if (ro.base == NULL || ro.offset < DEFAULT_ROOT) {
		ret.base = NULL;
		ret.offset = (uint64_t)NULL;
		return ret;
	} else { // object is a valid object
		ret.offset = obj_ref->left;
		ret.base = ro.base;
	}
	
	// assign over if failed (nothing at this memory location)
	if (obj_ref->left == (uint64_t)NULL) {
		ret.base = NULL;
		ret.offset = (uint64_t)NULL;
	}
	return ret;
}

runic_obj_t runic_node_right(runic_obj_t ro) {
	runic_obj_node_t* obj_ref = (runic_obj_node_t*)(ro.base + ro.offset);
	runic_obj_t ret;
	if (ro.base == NULL || ro.offset < DEFAULT_ROOT) {
		// bad input
		ret.base = NULL;
		ret.offset = (uint64_t)NULL;
		return ret;
	} else {
		ret.offset = obj_ref->right;
		ret.base = ro.base;
	}
	
	if (obj_ref->right == (uint64_t)NULL) {
		// nothing at this memory location
		ret.base = NULL;
		ret.offset = (uint64_t)NULL;
	}
	return ret;
}

//// atom
size_t runic_atom_size(runic_obj_t ro) { // returns the size of atom
	runic_obj_atom_t* obj_ref = (runic_obj_atom_t*)(ro.base + ro.offset);
	int sz = obj_ref->tag;
	if (ro.base == NULL || ro.offset < DEFAULT_ROOT) {
		return (uint64_t)NULL;
	}

	// atoms store a minimum of 8 bytes so they can be used in other operations
	// for values less than 8, a strlen is used to get the actual size
	if (sz <= 8) {
		sz = strlen(&(obj_ref->value));
		if (sz < 0) {
			// this should never execute
			// c standard does not have an error code for strlen
			// but we should test for this anyway
			return OP_FAIL_CODE;
		}
	}
	return sz;
}

bool runic_atom_read(runic_obj_t ro, char* c) { // returns atom value
	size_t sz = runic_atom_size(ro);
	runic_obj_atom_t* obj_ref = (runic_obj_atom_t*)(ro.base + ro.offset);
	if (c == NULL || ro.base == NULL || ro.offset < DEFAULT_ROOT) {
		return false;
	} else if (memcpy(c, &obj_ref->value, sz) != NULL) {
		return true; 
	} else {
		// this should never execute
		// c standard does not have an error code for memcpy
		// but we should test for this anyway
		return false;
	}
}

// mutators
//// file
bool runic_set_root(runic_t* r, runic_obj_t ro) { // returns false on failure
	runic_file_t* file_ref = (runic_file_t*)r->base;
	if (r->base == NULL || ro.base == NULL || ro.offset < DEFAULT_ROOT) {
		return false;
	} else { // if the objects are real, write to disk. atomic operations cant fail
		file_ref->root = ro.offset;
		return true;
	}
	
}

runic_t runic_shrink(runic_t* r) {
	r->base = NULL;
	return *r;
}

runic_obj_t runic_alloc_node(runic_t* r) {
	runic_obj_t ro;
	ro.base = NULL;
	return ro;
}

runic_obj_t runic_alloc_atom(runic_t* r, size_t sz) {
	runic_obj_t ro;
	ro.base = NULL;
	return ro;

}

runic_obj_t runic_alloc_atom_str(runic_t* r, const char* value) {
	size_t sz;
	runic_obj_t ro, ro_NULL;
	if ((sz = strlen(value)) < 0) {
		// this should never execute
		// c standard does not have an error code for strlen
		// but we should test for it anyway
		ro_NULL.base = NULL;
		ro_NULL.offset = (uint64_t)NULL;
		return ro_NULL;
	}
	
	ro = runic_alloc_atom(r, sz);
	if (!runic_atom_write(&ro, value)) {
		ro_NULL.base = NULL;
		ro_NULL.offset = (uint64_t)NULL;
		return ro_NULL;
	}
	
	return ro;
}

//// node
bool runic_node_set_left(runic_obj_t* parent, runic_obj_t child) {
	runic_obj_node_t* parent_ref = (runic_obj_node_t*)(parent->base + parent->offset);
	if (parent->base == NULL || parent->offset < DEFAULT_ROOT
		|| child.base == NULL || child.offset < DEFAULT_ROOT)
	{   // both objects need to be real to write to disk
		return false;
	} else {
		parent_ref->left = child.offset;
		return true;
	}
	
}
bool runic_node_set_right(runic_obj_t* parent, runic_obj_t child) {
	runic_obj_node_t* parent_ref = (runic_obj_node_t*)(parent->base + parent->offset);
	if (parent->base == NULL || parent->offset < DEFAULT_ROOT
		|| child.base == NULL || child.offset < DEFAULT_ROOT)
	{
		return false;
	} else {
		parent_ref->right = child.offset;
		return true;
	}
}

//// atom
bool runic_atom_write(runic_obj_t* ro, const char* val) {
	runic_obj_atom_t* obj_ref = (runic_obj_atom_t*)(ro->base + ro->offset);
	size_t sz;
	if (ro->base == NULL || ro->offset < DEFAULT_ROOT || val == NULL) {
		return false;
	}
	sz = runic_atom_size(*ro);
	return (sz >= strlen(val) && memcpy(&obj_ref->value, val, sz) != NULL);
	// should return a success if both operations succeed.
}
