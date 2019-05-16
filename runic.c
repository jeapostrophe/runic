/****
 * runic.c - API Implementation for .runic 
 * 			 filetype covering opening, closing,
 * 			 and modifying source code files 
 * 			 directly on disk using a tree
 * 			 format.
****/

#include <stdio.h> // perror
#include <stdlib.h> // exit
#include <fcntl.h> // open flags
#include <unistd.h> // close
#include <sys/stat.h> // open, fstat
#include <sys/mman.h> // mmap, munmap
#include "runic.h" // runic

// TODO: Start by building API based on README.md
// Go function by function. First three functions
// listed.

runic_t runic_open(const char* path, int mode)
{
	runic_t ro;

	switch (mode)
	{
	case READONLY: // Read-only
		___runic_open_on_args(&ro, path, O_RDONLY,
			S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH, PROT_READ, MAP_PRIVATE);
		break;
	case READWRITE:
		___runic_open_on_args(&ro, path, O_RDWR,
			S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH, PROT_READ | PROT_WRITE, MAP_SHARED);
		break;
	case CREATEWRITE:
	default:
		___runic_open_on_args(&ro, path, O_RDWR | O_CREAT,
			S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH, PROT_READ | PROT_WRITE, MAP_SHARED);
		break;
	}
	return ro;
}

void ___runic_open_on_args(runic_t* ro, const char* path, int open_flags,
	int permissions_flags, int prot_flags, int map_mode )
{
	if ((ro->fd = open(path, open_flags, permissions_flags)) == -1)
	{
		perror("File open failed.\n");
		exit(1);
	}
	if (fstat(ro->fd, &(ro->sb)) == -1)
	{
		perror("File access corrupted, couldn't get filesize.\n");
		exit(1);
	}
	if ((ro->addr = mmap(NULL, (ro->sb.st_size ? ro->sb.st_size : _SC_PAGESIZE),
		prot_flags, map_mode, ro->fd, 0)) == MAP_FAILED)
	{
		perror("Mmap failed.\n");
		exit(1);
	}
	if (open_flags & O_CREAT)
	{
		/* read the magic number, ensure file is correct */
	}
	else
	{
		/* create the magic number */
	}
}

void runic_close(runic_t ro)
{
	munmap(ro.addr, ro.sb.st_size);
	close(ro.fd);
}

// alloc'ing a node is really a matter of alloc'ing file space for the node if none exists
// remember these are mmap'd files
// should it return an obj_t, or just alloc the space?
runic_obj_t* runic_alloc_node(runic_t ro)
{
	runic_obj_t* rn = 0;

	return rn;
}



// - Write a function, given the aforementioned
// - signature or similar, that inserts a root
// - node into runic_file based on the passed handle.
// --- Start with a function that just writes to
// --- the passed handle.
