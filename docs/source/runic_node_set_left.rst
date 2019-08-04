.. role:: c(code)
   :language: c

================================================================================
:c:`bool runic_node_set_left(runic_obj_t* parent, runic_obj_t child);`
================================================================================

The above function signature outlines the operation regarding pointer assignment of the root node. The function takes the offset of :c:`child` and assigns it as the left element of :c:`parent`. A subsequent call to :c:`runic_node_left()` with the same :c:`parent` argument will return a copy of :c:`child`.

Arguments:
==========

	**parent** – A :c:`runic_obj_t` of type :c:`NODE` that was returned as a result of a successful runic_alloc, runic_root, node_left or node_right operation. Meaning, this object will have an initialized :c:`base` pointer and an :c:`offset` greater than or equal to :c:`DEFAULT_ROOT` as defined in dependencies. Using an uninitialized :c:`parent` is an invalid operation and the program will likely return :c:`false`.

	**child** – A :c:`runic_obj_t` that was returned as a result of a successful runic_alloc, runic_root, node_left or node_right operation. Meaning, this object will have an initialized :c:`base` pointer and an :c:`offset` greater than or equal to :c:`DEFAULT_ROOT` as defined in dependencies. Using an uninitialized :c:`child` is an invalid operation and the program will likely return :c:`false`.


Returns:
========

On a success, :c:`child` is assigned as the left element for :c:`parent`, and the function returns :c:`true`. On a failure, the function returns :c:`false`.
