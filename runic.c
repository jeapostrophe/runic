/****
 * runic.c - API Implementation for .runic 
 * 			 filetype covering opening, closing,
 * 			 and modifying source code files 
 * 			 directly on disk using a tree
 * 			 format.
****/

#include <stdio.h> // perror
#include <stdlib.h> // exit
#include <string.h> // strcmp
#include <fcntl.h> // open flags
#include <unistd.h> // close, sysconf
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
		close(ro->fd);
		perror("File access corrupted, couldn't get filesize.\n");
		exit(1);
	}
    if ((ro->addr = mmap(NULL, (ro->sb.st_size ? ro->sb.st_size : 1), // arg 2 is a nonzero value, even on create
		prot_flags, map_mode, ro->fd, 0)) == MAP_FAILED) // creates the file in disk (if necessary) and generates a map
	{
		close(ro->fd);
		perror("Mmap failed.\n");
		exit(1);
	}
	if (open_flags & O_CREAT) // generates file space
	{
		runic_close(*ro); // close mmap and file
		if ((ro->fd = open(path, open_flags, permissions_flags)) == -1) // reopen file
		{
			perror("File open failed.\n");
			exit(1);
		}
		write(ro->fd, "\0", sysconf(_SC_PAGESIZE)); // write into file (4K)
		if ((ro->addr = mmap(NULL, (sysconf(_SC_PAGESIZE)),
		prot_flags, map_mode, ro->fd, 0)) == MAP_FAILED) // mmap file (4K)
		{
			close(ro->fd);
			perror("Mmap failed.\n");
			exit(1);
		}
		fstat(ro->fd, &(ro->sb)); // stat file (should be 4K)
		strcpy((char*)ro->addr, "RUNIC"); // insert magic number and return
	}
	else
	{
		if (strcmp((char*)ro->addr, "RUNIC") != 0)
		{
			runic_close(*ro);
			perror("File is not a runic file.\n");
			exit(1);
		}
	}
}

void runic_close(runic_t ro)
{
	munmap(ro.addr, ro.sb.st_size);
	close(ro.fd);
}

runic_obj_t* runic_alloc_node(runic_t ro)
{
	runic_obj_t* rn = 0;

	// access the file size
	// navigate the node tree (to the deepest point) where children == null
	// get address of the navigated node
	// determine if there is space in the file for atleast 1 more node
	// --- current address depth + node size <= file size
	// (if necesary) create the space
	// --- create a new runic_t, run createwrite @ 2x size of file
	// --- perform stop and copy
	// create the node
	// --- 
	// return the address of the node
	// ---

	return rn;
}


// - Write a function, given the aforementioned
// - signature or similar, that inserts a root
// - node into runic_file based on the passed handle.
// --- Start with a function that just writes to
// --- the passed handle.



//   const or atom  == runic_obj_t
//        const   ==  enum'd runic_obj_ty_t
//    /          					 \
// atom ==  runic_obj_ty_t		 const or null
