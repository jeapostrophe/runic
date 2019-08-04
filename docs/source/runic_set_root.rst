.. role:: c(code)
   :language: c

=====================================================
:c:`bool runic_set_root(runic_t* r, runic_obj_t ro);`
=====================================================

The above function signature outlines the operation regarding pointer assignment of the root node. The function takes the offset of :c:`ro` and assigns it as the root object of :c:`r`. A subsequent call to :c:`runic_root()` with the same :c:`r` argument will return a copy of :c:`ro`.

Arguments:
==========

	**r** – A :c:`runic_t` object that was returned as a result of a successful open operation. Attempting to use any invalid or malformed object is an invalid operation. The program will almost certainly return :c:`false`. Attempting to assign the root pointer with such an object is not encouraged.

	**ro** – A :c:`runic_obj_t` that was returned as a result of a successful runic_alloc, runic_root, node_left or node_right operation. Meaning, this object will have an initialized :c:`base` pointer and an :c:`offset` greater than or equal to :c:`DEFAULT_ROOT` as defined in dependencies. Using an uninitialized :c:`ro` is an invalid operation and the program will likely return an invalid :c:`runic_obj_t` as described in :c:`runic_root()`.

Returns:
========

Following a successful assignment of ro as the root object of :c:`r`, the function will return :c:`true`. On an unsuccessful assignment (e.g.: invalid object use), the function will return :c:`false`.
