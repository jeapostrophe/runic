.. role:: c(code)
   :language: c

================
:c:`runic_obj_t runic_alloc_node(runic_t* r);`
================

The above function signature outlines the operation regarding allocating a :c:`NODE`.

Arguments:
==========

	**r** – A :c:`runic_t` object that was returned as a result of a successful open operation. Attempting to use any other object is undefined behavior. The program will almost certainly return an invalid :c:`runic_obj_t`. Attempting to allocate objects with invalid :c:`runic_t` objects is not encouraged.


Returns:
========

A :c:`runic_obj_t` object which is of type :c:`NODE`. In the event that file memory is full, no objects can be garbage collected, and there is no additional disk space, this operation may fail. If so, the :c:`runic_obj_t` returned will have an offset of :c:`0` and a base pointer equal to :c:`NULL`.