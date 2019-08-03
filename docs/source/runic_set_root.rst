.. role:: c(code)
   :language: c

=====================================================
:c:`bool runic_set_root(runic_t* r, runic_obj_t ro);`
=====================================================

The above function signature outlines the operation regarding pointer assignment of the root node. The function takes the offset of :c:`ro` and assigns it as the root object of :c:`r`. A subsequent call to :c:`runic_root()` with the same :c:`r` argument will return a copy of :c:`ro`.

Arguments:
==========

	:c:`r` – A :c:`runic_t` object that was returned as a result of a successful open operation. Attempting to use any other object is undefined behavior. The program will almost certainly return :c:`false`. Attempting to assign the root pointer of an invalid :c:`runic_t` object is not encouraged.

	:c:`ro` – A :c:`runic_obj_t` object that was returned as a result of a successful :c:`runic_root()` or any runic_alloc operation. Attempting to use any other object is undefined behavior. The program will almost certainly return :c:`false`. Attempting to assign the root pointer of a :c:`runic_t` to invalid :c:`runic_obj_t` objects is not encouraged.

Returns:
========

Following a successful assignment of ro as the root object of :c:`r`, the function will return :c:`true`. On an unsuccessful assignment (e.g.: invalid object use), the function will return :c:`false`.
