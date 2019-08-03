.. role:: c(code)
   :language: c

=======================================
:c:`runic_obj_t runic_root(runic_t r);`
=======================================

The above function signature outlines the operation regarding the access of the root object.

Arguments:
==========

	:c:`r` – A :c:`runic_t` object that was returned as a result of a successful open operation. Attempting to use any other object is undefined behavior. The program will almost certainly return an invalid :c:`runic_obj_t` object. Attempting to access root objects with invalid :c:`runic_t` objects is not encouraged.

Returns:
========

Following a successful open, the program will return a :c:`runic_obj_t` with initialized values enabling further use of the file: 

:c:`typedef struct runic_obj {`
	:c:`uint8_t* base;`		– A pointer to the first address of a contiguous block of memory where the file exists in heap space. (This is the same value as :c:`runic_t`).

	:c:`uint64_t offset;`	– This object’s address, as an offset relative to base.

:c:`} runic_obj_t;`

This function will specifically return the root object – the object which is referenced by the base pointer of the file header. On an unsuccessful open, or otherwise invalid :c:`runic_t`, this function will return an invalid :c:`runic_obj_t`. If there is no root object to return (such as a brand-new file), the program will return an invalid :c:`runic_obj_t`, and print an error to the console alerting the user that no root exists for this file.

Similar in purpose to an invalid :c:`runic_t`, the invalid :c:`runic_obj_t` is a way to signal to the user that this operation has failed. As a result, in the invalid :c:`runic_obj_t`, the base pointer is set to :c:`NULL`, and the offset is set to :c:`0`—an illegal offset location. Offsets of :c:`0` would in effect overwrite the file header, which is not allowed. All functions which manipulate the file or objects within the file will perform checks in order to prevent use invalid runic object for this reason.
