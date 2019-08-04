.. role:: c(code)
   :language: c

============================================
:c:`size_t runic_atom_size(runic_obj_t ro);`
============================================

The above function signature outlines the operation for retrieving the size of an :c:`ATOM`.

Arguments:
==========

	**ro** – A :c:`runic_obj_t` object which is of type :c:`ATOM` that was returned as a result of a successful :c:`runic_root()` or any runic_alloc operation. Attempting to use any other object is an invalid operation. The program will almost certainly return :c:`0` and an error will be displayed to console.


Returns:
========

On a successful operation, the function returns a value, :c:`0 < size_t < 255`, corresponding to the size of the :c:`ATOM` string. On an unsuccessful operation, the function returns :c:`0`, an invalid size marker for :c:`ATOM`.