.. role:: c(code)
   :language: c

==================================================
:c:`runic_obj_t runic_node_right(runic_obj_t ro);`
==================================================

The above function signature outlines the operation regarding the access of the right child of a :c:`NODE`.

Arguments:
==========

	**ro** – A :c:`runic_obj_t` of type :c:`NODE` that was returned as a result of a successful runic_alloc, runic_root, node_left or node_right operation. Meaning, this object will have an initialized :c:`base` pointer and an :c:`offset` greater than or equal to :c:`DEFAULT_ROOT` as defined in dependencies. Using an uninitialized :c:`ro` is an invalid operation and the program will likely return an invalid :c:`runic_obj_t` as described in :c:`runic_root()`.


Returns:
========

On a successful operation, this function will return the child :c:`runic_obj_t` from the object :c:`ro`, on its right subtree, if any exist. If none exist, it will print a warning to console, return a :c:`runic_obj_t` with a :c:`NULL` pointer and an :c:`offset` of :c:`0` (returning an invalid :c:`runic_obj_t`). If an invalid :c:`runic_obj_t` is supplied in the argument, an invalid :c:`runic_obj_t` is returned.