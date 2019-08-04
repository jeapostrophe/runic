.. role:: c(code)
   :language: c

=============================================================
:c:`bool runic_atom_write(runic_obj_t* ro, const char* val);`
=============================================================

The above function signature outlines the operation regarding changing the value of write.

Arguments:
==========

	**ro** – A :c:`runic_obj_t` object which is of type :c:`ATOM` that was returned as a result of a successful :c:`runic_root()` or a runic_alloc operation. Attempting to use any other object is an invalid operation. The program will almost certainly return :c:`false` and an error will be displayed to console.

	**val** – A null-terminated c-string which is of size less than or equal to the value returned by :c:`runic_atom_size()`. Using a string of a larger size will cause this operation to fail, print to console, and return :c:`false`.

Returns:
========

On a success, :c:`val` is assigned into memory for :c:`ro`, and the function returns :c:`true`. On a failure, the function returns :c:`false`.
