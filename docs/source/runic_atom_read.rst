.. role:: c(code)
   :language: c

===================================================
:c:`bool runic_atom_read(runic_obj_t ro, char* c);`
===================================================

The above function signature outlines the operation for reading the value of an :c:`ATOM`.

Arguments:
==========

	**ro** – A :c:`runic_obj_t` object which is of type :c:`ATOM` that was returned as a result of a successful :c:`runic_root()` or any runic_alloc operation. Attempting to use any other object is an invalid operation. The program will almost certainly return :c:`false` and the input buffer will remain unaltered. Attempting read from an invalid :c:`runic_obj_t` is not encouraged.

	**c** – A buffer with space enough to read the contents of :c:`ro`. If the buffer is too small, it is undefined behavior. The program may fail.


Returns:
========

On a success, the :c:`ATOM` value of :c:`ro` is deposited into the buffer :c:`c`, and the function returns :c:`true`. On a failure, the function returns :c:`false`.
