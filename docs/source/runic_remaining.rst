.. role:: c(code)
   :language: c

=======================================================
:c:`uint64_t runic_remaining(runic_t r, bool silent);`
=======================================================

The above function signature outlines the operation regarding the access of the remaining space in a file, in bytes. This will return the value of the total file size, minus the next free location in file memory; more specifically, in the file described by :c:`r`.

Arguments:
==========

	**r** – A :c:`runic_t` object that was returned as a result of a successful open operation. Attempting to use any invalid or malformed object is an invalid operation. The program will almost certainly return :c:`0`. Attempting return a value with such an object is not encouraged.

	**silent** – A value, set to :c:`true` or :c:`false`, which specifies if information regarding the file size is printed to :c:`stdout`.

Returns:
========

Following a successful open, this function will subsequently return the amount of the remaining free memory, in bytes, as an unsigned 64-bit integer. On an unsuccessful open, or otherwise invalid :c:`runic_t`, this function will return :c:`0`. If :c:`silent` is set to :c:`false`, similar information will be printed to :c:`stdout`.