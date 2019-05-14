/****
 * runic.c - API Implementation for .runic 
 * 			 filetype covering opening, closing,
 * 			 and modifying source code files 
 * 			 directly on disk using a tree
 * 			 format.
****/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "runic.h"

// TODO: Start by building API based on README.md
// Go function by function. First three functions
// listed.
//

// TODO: Research file offset and address. These mmap arguments are not correct or final!
runic_t runic_open(const char* path, int mode)
{
	runic_t runic_file;

	switch (mode)
	{
	case 1: // Read-only
		runic_file.fd = open(path, O_RDONLY);
		if (fstat(runic_file.fd, &(runic_file.sb)) == -1)
		{
			perror("File access corrupted, couldn't get filesize.\n");
			exit(1);
		}
		runic_file.addr = mmap(NULL, runic_file.sb.st_size, PROT_READ , MAP_PRIVATE, runic_file.fd, 0);
		break;
	
	default: // Read-write, Create new
		runic_file.fd = open(path, O_RDWR | O_CREAT | O_APPEND);
		if (fstat(runic_file.fd, &(runic_file.sb)) == -1)
		{
			perror("File access corrupted, couldn't get filesize.\n");
			exit(1);
		}
		runic_file.addr = mmap(NULL, runic_file.sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, runic_file.fd, 0);
		break;
	}	

	return runic_file;
}


// - Write a function, given the aforementioned
// - signature or similar, that closes a runic_file.
// --- Start with a function that just closes a
// --- runic_file using the mmap protocol.
//
// - Write a function, given the aforementioned
// - signature or similar, that inserts a root
// - node into runic_file based on the passed handle.
// --- Start with a function that just writes to
// --- the passed handle.
