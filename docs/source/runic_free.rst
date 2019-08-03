.. role:: c(code)
   :language: c

====================================
:c:`uint64_t runic_free(runic_t r);`
====================================

The above function signature outlines the operation regarding the access of the free pointer. This will return the value of the next free location in file memory; more specifically, in the file described by :c:`r`.

Arguments:
==========

	:c:`r` – A :c:`runic_t` object that was returned as a result of a successful open operation. Attempting to use any other object is undefined behavior. The program will almost certainly return :c:`0`. Attempting to access the free pointer with invalid :c:`runic_t` objects is not encouraged.

Returns:
========

Following a successful open, this function will return the value of the free pointer as an unsigned 64-bit integer. On an unsuccessful open, this function will return :c:`0`. :c:`0` is not a valid location for the free pointer, so returning :c:`0` is the mechanism the program uses to alert that this function has failed. 

