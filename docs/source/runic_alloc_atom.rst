.. role:: c(code)
   :language: c

================
:c:`runic_obj_t runic_alloc_atom(runic_t* r, size_t sz);`
================

The above function signature outlines the operation regarding allocating an :c:`ATOM` with a size equal to 1 byte plus the size as specified by :c:`sz`. :c:`ATOM` cannot be resized, so choose the size wisely, or simply use :c:`runic_obj_t runic_alloc_atom_str()`.

Arguments:
==========

	**r** – A :c:`runic_t` object that was returned as a result of a successful open operation. Attempting to use any other object is undefined behavior. The program will almost certainly return an invalid :c:`runic_obj_t`. Attempting to allocate objects with invalid :c:`runic_t` objects is not encouraged.

	**sz** – A value of less than 255. Using larger value will cause this operation to fail, print to console, and return an invalid :c:`runic_obj_t`.


Returns:
========

A :c:`runic_obj_t` object which is of type :c:`ATOM` and is equal in size to :c:`sz + 1`. In the event that file memory is full, no objects can be garbage collected, and there is no additional disk space, this operation may fail. If so, the :c:`runic_obj_t` returned will have an offset of :c:`0` and a base pointer equal to :c:`NULL`.