/****
 * runic.c - API Implementation for .runic 
 * 			 filetype covering opening, closing,
 * 			 and modifying source code files 
 * 			 directly on disk using a tree
 * 			 format.
****/

#include <stdio.h> // perror
#include <stdlib.h> // exit
#include <stddef.h> // ptrdiff_t
#include <stdbool.h> // bool
#include <string.h> // strcmp
#include <fcntl.h> // open flags
#include <unistd.h> // close, sysconf
#include <sys/stat.h> // open, fstat
#include <sys/mman.h> // mmap, munmap
#include "runic.h" // runic

runic_core_t runic_open(const char* path, int mode)
{
	runic_core_t ro;

	switch (mode)
	{
	case READONLY:
		___runic_open_on_args(&ro, path, O_RDONLY,
			S_IRUSR | S_IRGRP | S_IROTH, PROT_READ, MAP_PRIVATE);
			// Permissions: r-- r-- r--
		break;
	case READWRITE:
		___runic_open_on_args(&ro, path, O_RDWR,
			S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH, PROT_READ | PROT_WRITE, MAP_SHARED);
			// Permissions: rw- r-- r--
		break;
	case CREATEWRITE:
	default:
		___runic_open_on_args(&ro, path, O_RDWR | O_CREAT,
			S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH, PROT_READ | PROT_WRITE, MAP_SHARED);
			// Permissions: rw- r-- r-- (Create)
		break;
	}
	return ro;
}

void ___runic_open_on_args(runic_core_t* ro, const char* path, int open_flags,
	int share_flags, int prot_flags, int map_mode)
{
	if ((ro->fd = open(path, open_flags, share_flags)) == -1)
	{
		perror("File open failed.\n");
		exit(1);
	}
	if (fstat(ro->fd, &(ro->sb)) == -1)
	{
		close(ro->fd);
		perror("File access corrupted, couldn't get filesize.\n");
		exit(1);
	}
	if ((ro->base = mmap(NULL, (ro->sb.st_size ? ro->sb.st_size : 1), // arg 2 is any nonzero value
		prot_flags, map_mode, ro->fd, 0)) == MAP_FAILED) // creates the file in disk (if necessary) and generates a map
	{
		close(ro->fd);
		perror("Mmap failed.\n");
		exit(1);
	}
	if (open_flags & O_CREAT) // generates file space
	{
		runic_close(*ro); // close mmap and file
		if ((ro->fd = open(path, open_flags, share_flags)) == -1) // reopen file
		{
			perror("File open failed.\n");
			exit(1);
		}
		write(ro->fd, "\0", sysconf(_SC_PAGESIZE)); // write into file (4K)
		if ((ro->base = mmap(NULL, sysconf(_SC_PAGESIZE),
			prot_flags, map_mode, ro->fd, 0)) == MAP_FAILED) // mmap file (4K)
		{
			close(ro->fd);
			perror("Mmap failed.\n");
			exit(1);
		}
		fstat(ro->fd, &(ro->sb)); // stat file (should be 4K)
		strcpy((char*)(ro->base), "RUNIC"); // insert magic number and return
		((runic_file_t*)(ro->base))->root = DEFAULT_ROOT; // set the first node in the tree
		((runic_file_t*)(ro->base))->free = DEFAULT_ROOT; // start of free
	}
	else
	{
		if (strcmp((char*)ro->base, "RUNIC") != 0)
		{
			runic_close(*ro);
			perror("File is not a runic file.\n");
			exit(1);
		}
	}

	// TODO: handle errors better than exit(1)
}

void runic_close(runic_core_t ro)
{
	munmap(ro.base, ro.sb.st_size);
	close(ro.fd);
}

runic_obj_node_t* runic_alloc_node(runic_core_t* ro)
{
	runic_obj_node_t* rn = NULL;

	if (___calc_remaing_space(*ro))
	{
		rn = (runic_obj_node_t*)(ro->base + ((uint64_t)((runic_file_t*)(ro->base))->free));
		((runic_file_t*)(ro->base))->free += NODE_SIZE;
		rn->tag = NODE_TAG;
		rn->left_child_offset = (uint64_t)NULL;
		rn->right_child_offset = (uint64_t)NULL;

		return rn;
	}
	else
	{
		perror("Not enough space.\n");
		exit(1);
		// TODO: Change this to garbage collect, attempt again.
	}
}

bool ___calc_remaing_space(runic_core_t ro)
{
	bool status = false;
	uint64_t free, file_size;

	free = ((runic_file_t*)(ro.base))->free;
	file_size = ro.sb.st_size;

	if ((free + NODE_SIZE) < file_size)
	{
		status = true;
	}

	return status;
}

runic_obj_atom_t* runic_alloc_atom(runic_core_t* ro, size_t size)
{
	runic_obj_atom_t* rn = NULL;

	if (___calc_remaing_space_atom(*ro, size))
	{
		rn = (runic_obj_atom_t*)(ro->base + ((uint64_t)((runic_file_t*)(ro->base))->free));
		((runic_file_t*)(ro->base))->free += size + ATOM_TAG_SIZE;
		rn->tag = size;

		return rn;
	}
    else
    {
        perror("Not enough space.\n");
        exit(1);
		// TODO: Change this to garbage collect, attempt again.
    }
}

bool ___calc_remaing_space_atom(runic_core_t ro, size_t size)
{
	bool status = false;
	uint64_t free, file_size;

	free = ((runic_file_t*)(ro.base))->free;
	file_size = ro.sb.st_size;

	if ((free + size + ATOM_TAG_SIZE) < file_size)
	{
		status = true;
	}

	return status;
}

bool runic_set_root(runic_core_t* ro, runic_obj_node_t* rn)
{
	bool status = false;

	// do some error checking here, null ptrs, etc.

	uint64_t dist = (uint64_t)(rn - (uint64_t)(ro->base + DEFAULT_ROOT));
	((runic_file_t*)(ro->base))->root = dist;

	return status;
}
