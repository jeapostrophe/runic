.. role:: c(code)
   :language: c

=====================================================
:c:`runic_t runic_shrink(runic_t* r);`
=====================================================

The above function outlines the operation of a shrink operation. The shrink operation performs garbage collection on the targeted file, attempting to reclaim any available memory in the file. It first defragments the tree, removing unwanted nodes, and then truncates the file to :c:`1` byte past the free pointer. As a result, a subsequent alloc operation will cause the file to expand.


Arguments:
==========

	**r** – A :c:`runic_t` object that was returned as a result of a successful shrink operation. Attempting to use any invalid or malformed object is an invalid operation. The program will almost certainly return a null :c:`runic_t`. Attempting shrink a file with such an object is not encouraged.

Returns:
========

The file will return the new, shrinked file as output. 