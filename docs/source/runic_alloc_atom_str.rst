.. role:: c(code)
   :language: c

=====================================================================
:c:`runic_obj_t runic_alloc_atom_str(runic_t* r, const char* value);`
=====================================================================

The above function signature outlines the operation regarding allocating an :c:`ATOM` with a length equal to the string passed in argument :c:`value`. The argument is then written to the atom. This is a compound function which uses :c:`runic_alloc_atom()` and :c:`runic_atom_write()`.

Arguments:
==========

	**r** – A :c:`runic_t` object that was returned as a result of a successful open operation. Attempting to use any other object is an invalid operation. The program will almost certainly return an invalid :c:`runic_obj_t`. Attempting to allocate objects with invalid :c:`runic_t` objects is not encouraged.

	**value** – A null-terminated c-string which is less than 255 bytes long. Using a string of a larger size will cause this operation to fail, print to console, and return an invalid :c:`runic_obj_t`.


Returns:
========

A :c:`runic_obj_t` object which is of type :c:`ATOM` and contains the string described in :c:`value`. In the event that file memory is full, no objects can be garbage collected, and there is no additional disk space, this operation may fail. If so, the :c:`runic_obj_t` returned will have an offset of :c:`0` and a base pointer equal to :c:`NULL`.