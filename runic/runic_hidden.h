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


bool __runic_open_on_args(runic_t* r, const char* path, int open_flags,
	int share_flags, int prot_flags, int map_mode)
{
	runic_file_t* file_ref;
	bool stat = false;
	if ((r->fd = open(path, open_flags, share_flags)) == OP_FAIL_CODE)
		return stat;
	if (fstat(r->fd, &r->sb) == OP_FAIL_CODE) {
		close(r->fd);
		return stat;
	}
	if ((r->base = mmap(NULL, (r->sb.st_size ? r->sb.st_size : 1), // arg 2 != 0
		prot_flags, map_mode, r->fd, 0)) == MAP_FAILED) // creates file & map
	{
		close(r->fd);
		return stat;
	}
	if ((open_flags & O_CREAT) && (r->sb.st_size < sysconf(_SC_PAGESIZE))) { // generates file space
		runic_close(*r); // close mmap and file
		if ((r->fd = open(path, open_flags, share_flags)) == OP_FAIL_CODE) // reopen
			return stat;
		truncate(r->path, sysconf(_SC_PAGESIZE)); // write into file (4K)
		if ((r->base = mmap(NULL, sysconf(_SC_PAGESIZE), // mmap file (4K)
			prot_flags, map_mode, r->fd, 0)) == MAP_FAILED) {
			close(r->fd);
			return stat;
		}
		file_ref = (runic_file_t*)r->base;
		if (fstat(r->fd, &r->sb) == OP_FAIL_CODE) {
			close(r->fd);
			return stat;
		}
		memcpy((char*)r->base, "RUNIC", HEADER_SIZE); // insert magic number and return
		file_ref->root = (uint64_t)NULL; // set first node
		file_ref->free = DEFAULT_ROOT; // start of free
	} else {
		file_ref = (runic_file_t*)r->base;
		if (memcmp((char*)r->base, "RUNIC", HEADER_SIZE) != 0) {
			runic_close(*r);
			return stat;
		}
	}
	return stat = true;
}

runic_obj_t __runic_alloc_fwdptr(runic_obj_t ro, uint64_t addr) {
	runic_obj_fwdptr_t* fptr_ref = (runic_obj_fwdptr_t*)(ro.base+ro.offset);
	fptr_ref->tag = FPTR_TAG_VALUE;
	fptr_ref->fwdaddr = addr;
	return ro;
}

uint64_t __runic_fwd_addr(runic_obj_t ro) {
	runic_obj_fwdptr_t* fptr_ref = (runic_obj_fwdptr_t*)(ro.base+ro.offset);
	return fptr_ref->fwdaddr;
}

runic_obj_t __runic_lookback_left(runic_t* r, runic_t* rn, runic_obj_t ro) {
	runic_obj_node_t* node_ref = (runic_obj_node_t*)(rn->base + ro.offset);
	runic_obj_t rlo, r_null; 
	rlo.base = r->base;
	rlo.offset = node_ref->left;
	if (rlo.base != NULL && rlo.offset >= DEFAULT_ROOT)
		return rlo;
	else {
		r_null.base = NULL;
		r_null.offset = 0;
		return r_null;
	}
}

bool __runic_obj_node_cpy(runic_t src, runic_t dest, uint64_t src_offset) {
    runic_file_t* source, *destination;
    source = (runic_file_t*) src.base;
    destination= (runic_file_t*) dest.base;
    if (memcpy(dest.base+(destination->free)-sizeof(runic_obj_node_t), src.base+src_offset, sizeof(runic_obj_node_t)) != NULL)
        return true;
	else
        return false;
}

runic_obj_t __runic_lookback_right(runic_t* r, runic_t* rn, runic_obj_t ro) {
	runic_obj_node_t* node_ref = (runic_obj_node_t*)(rn->base + ro.offset);
	runic_obj_t rro, r_null; 
	rro.base = r->base;
	rro.offset = node_ref->right;
	if (rro.base != NULL && rro.offset >= DEFAULT_ROOT)
		return rro;
	else {
		r_null.base = NULL;
		r_null.offset = 0;
		return r_null;
	}
}

uint64_t __runic_move(runic_t* r, runic_t* rn, runic_obj_t ro) {
	char c[255];
	uint64_t out;
	runic_obj_t rno, ret;
	runic_obj_ty_t type = runic_obj_ty(ro);
	if (ro.base == NULL || ro.offset < DEFAULT_ROOT) {
		ret.base = NULL;
		ret.offset = (uint64_t)NULL;
		return ret.offset;
	}
	if (type == FWD_PTR)
		return __runic_fwd_addr(ro);
	else if (type == ATOM) {
		if (runic_atom_read(ro, c))
			rno = runic_alloc_atom_str(rn, c);
		else {
			out = (uint64_t)NULL;
			return out;
		}
    } else { // is node
    	rno = runic_alloc_node(rn);
  	    if (!__runic_obj_node_cpy(*r, *rn, ro.offset)) {
  	         ret.base = NULL;
  	         ret.offset = (uint64_t)NULL;
   	         return ret.offset;
   	    }
    }
	// if the alloc fails in some way
	if (rno.base == NULL || rno.offset < DEFAULT_ROOT) {
		ret.base = NULL;
		ret.offset = (uint64_t)NULL;
		return ret.offset;
	}
	// alloc the fwdptr
	__runic_alloc_fwdptr(ro, rno.offset);
	return rno.offset;
}

bool __runic_move_children(runic_t* r, runic_t* rn, runic_obj_t ro) {
	uint64_t off1, off2;
	runic_obj_t rl, rr;
	// get the children
	rl = __runic_lookback_left(r, rn, ro);
	rr = __runic_lookback_right(r, rn, ro);
	
	if (rl.base != NULL && rl.offset > DEFAULT_ROOT) {
		if ((off1 = __runic_move(r, rn, rl)) > DEFAULT_ROOT) {
			rl.offset = off1;
			rl.base = rn->base;
			if (!runic_node_set_left(&ro, rl)) {
				return false;
			}
		} else {
			return false;
		}
	}
	if (rr.base != NULL && rr.offset > DEFAULT_ROOT) {
		if ((off2 = __runic_move(r, rn, rr)) > DEFAULT_ROOT) {
			rr.offset = off2;
			rr.base = rn->base;
			if (!runic_node_set_right(&ro, rr)) {
				return false;
			}
		} else {
			return false;
		}
	}

	// return a success!~
	return true;
}

int __runic_doscan(runic_t* r, runic_t* rn) {
	int sz, freed = -1;
	uint64_t offset, free;
	runic_obj_ty_t type;
	runic_obj_t ro = runic_root(*r);
	if (ro.base == NULL || ro.offset < DEFAULT_ROOT)
		return freed;
	if ((offset = __runic_move(r, rn, ro)) < DEFAULT_ROOT)
		return freed;
    else {
        ro.offset = offset;
        ro.base = rn->base;
        runic_set_root(rn, ro);
	}
	do {
		if (ro.base != NULL && ro.offset >= DEFAULT_ROOT) {
            ro.offset = offset;
            ro.base = rn->base;
			// keep doing the thing as long as the above is true
			type = runic_obj_ty(ro);
            if (type == NODE) {
				__runic_move_children(r, rn, ro);
				offset += sizeof(runic_obj_node_t);
			} else {
				ro.offset = offset;
				ro.base = rn->base;
				if (runic_atom_size(ro) < sizeof(uint64_t)) {
					sz = sizeof(uint64_t);
				} else {
					sz = runic_atom_size(ro);
				}
				offset += sizeof(uint8_t) + sz;
			}
            free = runic_free(*rn);
		}
		else
			exit(1);
	} while (offset < free);
	if (runic_root(*rn).base != NULL) {
		freed = runic_free(*r) - (runic_free(*rn));
		return freed;
	}
	return freed;
}

bool __calc_remaing_space(runic_t r) {
	bool stat = false;
	uint64_t free, file_size;
	runic_file_t* file_ref = (runic_file_t*)r.base;
	free = file_ref->free;
	file_size = r.sb.st_size;
	if ((free + sizeof(runic_obj_node_t)) < file_size) {
		stat = true;
	}
	return stat;
}

int __runic_compact(runic_t* r) {
    runic_file_t* file_ref;
	int open_flags, share_flags, prot_flags, map_mode, output;
	runic_t rn;
    rn.base = NULL;
	rn.path = "/tmp/tmp2.runic";
	rn.mode = CREATEWRITE;
	open_flags =  O_RDWR | O_CREAT; // create this file in read-write
	share_flags = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	prot_flags = PROT_READ | PROT_WRITE;
	map_mode = MAP_SHARED;
	if ((rn.fd = open(rn.path, open_flags, share_flags)) == OP_FAIL_CODE) { // reopen
		return 0;
	}
	truncate(rn.path, r->sb.st_size); // new files are 0 sized, so write to file with space equal runic free
	if ((rn.base = mmap(NULL, r->sb.st_size, // mmap file with new, proper size
		prot_flags, map_mode, rn.fd, 0)) == MAP_FAILED) 
	{
		close(rn.fd);
		return 0;
	}
    if (fstat(rn.fd, &rn.sb) == OP_FAIL_CODE) {
        close(rn.fd);
        return 0;
    }
    file_ref = (runic_file_t*)rn.base;
    memcpy((char*)rn.base, "RUNIC", HEADER_SIZE); // insert magic number and return
    file_ref->root = (uint64_t)NULL; // set first node
    file_ref->free = DEFAULT_ROOT; // start of free
    output = __runic_doscan(r, &rn);
    if (output < 0)
		return -1;
	runic_close(*r);
	if (remove(r->path) == OP_FAIL_CODE) // remove old file
		return 0;
	if (rename(rn.path, r->path) == OP_FAIL_CODE) // rename and replace new file into old path
		return 0;
	rn.mode = r->mode;
	rn.path = r->path; // path should now properly reflect the path
	runic_close(rn);
	return output;
}

bool __expand_file(runic_t* r) {
	bool stat = false;
	runic_file_t* file_ref;
	int open_flags, share_flags, prot_flags, map_mode;
	runic_close(*r); // close mmap and file
	open_flags = O_RDWR | O_APPEND; // append to this file in read-write
	share_flags = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	prot_flags = PROT_READ | PROT_WRITE;
	map_mode = MAP_SHARED;
	if ((r->fd = open(r->path, open_flags, share_flags)) == -1) { // reopen
		return stat;
	}
	truncate(r->path, r->sb.st_size * 2); // write into file its current size (this will double capacity)
	if ((r->base = mmap(NULL, r->sb.st_size * 2, // mmap file with new, doubled size
		prot_flags, map_mode, r->fd, 0)) == MAP_FAILED) 
	{
		close(r->fd);
		return stat;
	}
	file_ref = (runic_file_t*)r->base;
	if (fstat(r->fd, &r->sb) == -1) {
		close(r->fd);
	 	return stat;
	}
	return stat = true;
}

bool __calc_remaing_space_atom(runic_t r, size_t size) {
	bool stat = false;
	size_t tsz = size; // true size
	uint64_t free, file_size;
	runic_file_t* file_ref = (runic_file_t*)r.base;
	free = file_ref->free;
	file_size = r.sb.st_size;
	if (tsz < sizeof(uint64_t))
		tsz = sizeof(uint64_t);
	if ((free + tsz + sizeof(uint8_t)) < file_size) {
		stat = true;
	}
	return stat;
}

// closing statements
#pragma pack(pop)
#endif /* runic_hidden.h */
