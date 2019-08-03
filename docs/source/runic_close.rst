.. role:: c(code)
   :language: c

=================================
:c:`bool runic_close(runic_t r);`
=================================

The above function signature outlines the operation regarding the closing of a runic file.

Arguments:
==========

	:c:`r` – A :c:`runic_t` object that was returned as a result of a successful open operation. Attempting to close any other object is undefined behavior. The program will almost certainly return :c:`false`. Attempting to close invalid :c:`runic_t` objects is not encouraged.

Returns:
========

Following a successful close, the function will return :c:`true`. On an unsuccessful close (e.g.: invalid object use), the function will return :c:`false`.
