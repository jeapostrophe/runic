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

// TODO: Research file offset and address. These mmap arguments are not correct or final!
// File offset should be 0 if I am starting at the beginning of the file on every open. Not sure if it should be different but 0 for now.
// Addr still not 100% on.
runic_t runic_open(const char* path, int mode)
{
	runic_t ro;

	switch (mode)
	{
	case readOnly: // Read-only
		ro.fd = open(path, O_RDONLY);
		safe_fstat(&ro);
		ro.addr = mmap(NULL, ro.sb.st_size, PROT_READ , MAP_PRIVATE, ro.fd, 0);
		mmap_failed(ro);
		break;

	case readWrite: // Read-write, Create new (default)
	default:
		ro.fd = open(path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
			// args: path, Read-write and create if new; permissions flags(necesary for create): rwx,r-x,r-x
		safe_fstat(&ro);
		ro.addr = mmap(NULL, ro.sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, ro.fd, 0);
		mmap_failed(ro);
		break;
	}	

	return ro;
}
void mmap_failed(runic_t ro)
{
	if(ro.addr == MAP_FAILED)
	{
		perror("Mmap failed.");
		exit(1);
	}
}
void safe_fstat(runic_t* ro)  // if there's a better naming convention i should use, just let me know
{
	if (fstat(ro->fd, &(ro->sb)) == -1)
	{
		perror("File access corrupted, couldn't get filesize.\n");
		exit(1);
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
runic_obj_t runic_alloc_node(runic_t ro)
{
	runic_obj_t rn;

	rn.temp = 0; // change this later

	return rn;
}



// - Write a function, given the aforementioned
// - signature or similar, that inserts a root
// - node into runic_file based on the passed handle.
// --- Start with a function that just writes to
// --- the passed handle.
