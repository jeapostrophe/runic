.. role:: c(code)
   :language: c

================
:c:`// runic_t`
================

Arguably the most critical element of the runic file API is how the file is represented in the program. In implementation, the file is represented to the user through the :c:`runic_t` artifact. This structure includes all necessary references needed to open, close, expand, and modify the file and its contents. 

.. code-block:: c

	typedef struct runic {
		const char* path;
		int fd;
		struct stat sb;
		int mode;
		uint8_t* base;
	} runic_t; 

Runic files contain a header; this includes a magic number, that uniquely identifies runic files from other file types (e.g.: .PDF, .DOC, etc.). The header also includes information to access the first object in the file, as well as the first available free space in file memory.

Runic files can contain 2 types of objects, :c:`ATOM` and :c:`NODE`. Each :c:`NODE` is an object which points to two children: a left and right child. With multiple :c:`NODE` a user can create a tree. Each :c:`ATOM` contains a value, an ascii string which provides data that gives the tree context, allowing for the construction of an abstract syntax tree, the primary use case for this API. This data is used to help traverse the tree and add new nodes.

The following operations can be performed on a file:

.. code-block:: c

	// accessors
	runic_t runic_open(const char* path, int mode);
	bool runic_close(runic_t r);
	runic_obj_t runic_root(runic_t r);
	uint64_t runic_free(runic_t r); 

	// mutators
	bool runic_set_root(runic_t* r, runic_obj_t ro);
	runic_obj_t runic_alloc_node(runic_t* r);
	runic_obj_t runic_alloc_atom(runic_t* r, size_t sz);
	runic_obj_t runic_alloc_atom_str(runic_t* r, const char* value);
 
