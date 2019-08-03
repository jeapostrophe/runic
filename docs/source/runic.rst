.. role:: c(code)
   :language: c

================
:c:`runic.h`
================

.. code-block:: c
	:linenos:

	/****
	* runic.h - API for .runic filetype
	*           covering opening, closing, and 
	*           modifying source code files 
	*           directly on disk using a tree
	*           format.
	****/

	// dependencies
	#include <stddef.h> // size_t
	#include <stdbool.h> // bool
	#include <stdint.h> // uintXX_t
	#include <sys/stat.h> // struct stat, open, fstat
	// see documentation for other necessary dependencies

	// preprocessor statements
	#ifndef RUNIC_H
	#define RUNIC_H
	#ifdef __cplusplus
		extern "C" {
	#endif

	// constants
	#define DEFAULT_ROOT 0x15
	#define HEADER_SIZE 0x05
	#define NODE_TAG_VALUE 0x00

	// enums
	enum runic_file_modes {
		READONLY, READWRITE, CREATEWRITE
	};

	typedef enum runic_obj_ty {
		NODE, ATOM
	} runic_obj_ty_t;

	// structs
	typedef struct runic {
		const char* path;
		int fd;
		struct stat sb;
		int mode;
		uint8_t* base;
	} runic_t;

	typedef struct runic_obj {
		uint8_t* base;
		uint64_t offset;
	} runic_obj_t;
		
	// accessors
	//// file
	runic_t runic_open(const char* path, int mode);
	bool runic_close(runic_t r);
	runic_obj_t runic_root(runic_t r);
	uint64_t runic_free(runic_t r);

	//// node
	runic_obj_ty_t runic_obj_ty(runic_obj_t ro);
	runic_obj_t runic_node_left(runic_obj_t ro);
	runic_obj_t runic_node_right(runic_obj_t ro);

	//// atom
	size_t runic_atom_size(runic_obj_t ro);
	bool runic_atom_read(runic_obj_t ro, char* c);

	// mutators
	//// file
	bool runic_set_root(runic_t* r, runic_obj_t ro);
	runic_obj_t runic_alloc_node(runic_t* r);
	runic_obj_t runic_alloc_atom(runic_t* r, size_t sz);
	runic_obj_t runic_alloc_atom_str(runic_t* r, const char* value);

	//// node
	bool runic_node_set_left(runic_obj_t* parent, runic_obj_t child);
	bool runic_node_set_right(runic_obj_t* parent, runic_obj_t child);

	//// atom
	bool runic_atom_write(runic_obj_t* ro, const char* val);

	// closing statements
	#ifdef __cplusplus
		}
	#endif
	#endif /* runic.h */

