.. role:: c(code)
   :language: c

Runic: A file format for encoding Abstract Syntax Trees.
=========================================================


Abstract:
---------
A file format for trees that can encode programs directly as ASTs. For example, the program :c:`int f ( int x ) { return 2 * x + 3; }` is normally stored as a string, but in our format ("Runic"), it will be stored as a tree: :c:`(function int f ([int x]) (return (+ (* 2 x) 3)))` where ()s indicate nodes as below:

.. code-block:: #

        FUNCTION
   /    /    \      \
 int    f    args     return   
             |         \
            arg         +
            / \        / \
          int  x      *   3
                     / \
                    2   x


But the tree is NOT stored linearized, instead it is stored as a binary encoding. For example, it might be stored like this:

.. code-block:: racket

	RUNIC
	0
	0: "function", 4, 1, 2, 3, 4
	1: "int", 0
	2: "f", 0
	3: "args", 1, 5
	4: "return", 1, 8
	5: "arg", 2, 6, 7
	6: "int", 0
	7: "x", 0
	8: "+", 2, 9, 10
	9: "*", 2, 11, 12
	10: "3", 0
	11: "2", 0
	12: "x", 0

This is just a sketch! The file starts with a magic number. Next, is the address of the first object (the top of the tree.) Then objects are encoded as the text data, then the number of children, then the children. In the real format, the pointers number, would be the offset in the file. So for example, if the first node ("0") stored pointers as 64-bits, then the second node ("1") would actually be stored at address 48, because there are 8 bytes in "function" and then 5 64-bit numbers. That is, the file would look like this…

.. code-block:: #

	RUNIC\0\0\0\0\0\0\0\0function\0\0\0\0\0\0\0\4\0\0\0\0\0\0\0\48....


There are many details in this sketch that need to worked out. For example, are offsets from the start of the file or from the start of the first object? If so, then everything in the above example would be offset by another 13 bytes because "RUNIC" is 5 bytes and then the first object pointer is another 8.

The file format will allow instantaneous (i.e. no parsing required) reading of the file with modifications (i.e. you can create new nodes, rewiring old nodes, and so on.) As the file grows and the interior pointers change, some nodes may become dead (i.e. nothing is pointing to them.) This means that the library will have to support garbage collection to free space inside files. The easiest way to do this is whenever the file grows from 2^N bytes to 2^N+1 bytes, do a "Stop & Copy"-style collection where the "From Space" is the old file and the "To Space" is the new, bigger file.

Tasks
------
- Define an API to use the library
- Write documentation for API
- Define the file format
- Write some example files by hand
- Write test cases for the API
- Implement the "writing" part of the API
- Implement the "reading" part of the API
- Implement the GC part of the API
- Write an interesting program that uses the library

API
----
.. code-block:: c

	typedef /* ??? */ runic_t;
	typedef /* ??? */ runic_obj_t;
	enum /* ??? ATOM or NODE or NULL */ runic_obj_ty_t;

	// IMPL: This will mmap the file
	runic_t runic_open( PATH );
	void runic_close( runic_t );

	runic_obj_t runic_root( runic_t ); 
	void runic_set_root( runic_t, runic_obj_t );

	runic_obj_t runic_alloc_atom( runic_t, size_t );
	runic_obj_t runic_alloc_node( runic_t );

	runic_obj_ty_t runic_obj_ty( runic_obj_t );

	size_t runic_atom_size( runic_obj_t );
	const char *runic_atom_read( runic_obj_t );
	void runic_atom_write( runic_obj_t, const char * );
	// Even though we may mmap, we don't allow those pointers to be exposed, because GC might happen thus invalidating them.

	runic_obj_t runic_node_left( runic_obj_t );
	runic_obj_t runic_node_right( runic_obj_t );
	void runic_node_set_left( runic_obj_t, runic_obj_t );
	void runic_node_set_right( runic_obj_t, runic_obj_t );


Refined Example:
-----------------
.. code-block:: c++

	runic_obj_t runic_alloc_atom_str ( runic_t ro, const char* c ) {
	auto ra = runic_alloc_node( ro, strlen(c) );
	runic_atom_write( ra, c );
	return ra; }

	/*
	root = ("function" . ("int" . ("f" . ((("int" . "x") . NULL) . ("return" . ("+" . (("*" . ("2" . "x")) . ("3" . NULL))))))))
	*/

	auto ro = runic_open("example.runic");
	auto rn = runic_alloc_node( ro );
	runic_set_root( ro, rn );

	runic_node_set_left( rn, runic_alloc_atom_str("function") );
	runic_node_set_right( rn, runic_alloc_node( ro ) );
	rn = runic_node_right( rn );

	runic_node_set_left( rn, runic_alloc_atom_str("int") );
	runic_node_set_right( rn, runic_alloc_node( ro ) );
	rn = runic_node_right( rn );

	runic_node_set_left( rn, runic_alloc_atom_str("f") );
	runic_node_set_right( rn, runic_alloc_node( ro ) );
	rn = runic_node_right( rn );

	auto args = runic_alloc_node( ro );
	runic_node_set_left( args, runic_alloc_node( ro ) );
	auto arg = runic_node_right( args );
	runic_node_set_left( arg, runic_alloc_atom_str("int") );
	runic_node_set_right( arg, runic_alloc_atom_str("x") );

	runic_node_set_left( rn, args );
	runic_node_set_right( rn, runic_alloc_node( ro ) );
	rn = runic_node_right( rn );
	runic_node_set_left( rn, runic_alloc_node( ro ) );
	rn = runic_node_left( rn );

	runic_node_set_left( rn, runic_alloc_atom_str("return") );
	runic_node_set_right( rn, runic_alloc_node( ro ) );
	rn = runic_node_right( rn );

	runic_node_set_left( rn, runic_alloc_atom_str("+") );
	runic_node_set_right( rn, runic_alloc_node( ro ) );
	rn = runic_node_right( rn );

	// XXX

	runic_close(ro);


File Format Details
--------------------
.. code-block:: c

	// FILE = MAGIC NUMBER, OFFSET of ROOT, OFFSET of FREE
	struct {
	char tag;
	uint32_t root;
	uint32_t free;
	} runic_ff_file_t;

	// NODE = MAGIC NUMBER, OFFSET of LEFT, OFFSET of RIGHT
	struct {
	char tag;
	uint32_t left;
	uint32_t right;
	} runic_ff_node_t;

	// ATOM = MAGIC NUMBER, SIZE of ATOM, ATOM BYTES
	struct {
	char tag;
	uint32_t size;
	} runic_ff_atom_t;

	// NULL = 0


Implementation Sketch
----------------------
.. code-block:: c

	void runic_node_set_left( runic_obj_t ro, runic_obj_t nl ) {
	runic_t r = ro->file;
	runic_ff_node_t rn = (r->base + ro->offset);
	rn->left = nl->offset;
	return; }
