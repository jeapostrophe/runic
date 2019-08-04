.. role:: c(code)
   :language: c

==================================================
:c:`runic_obj_ty_t runic_obj_ty(runic_obj_t ro);`
==================================================

The above function signature outlines the operation regarding the determination of a :c:`runic_obj_t` type.

Arguments:
==========

	**ro** – A :c:`runic_obj_t` that was returned as a result of a successful runic_alloc, runic_root, node_left or node_right operation. Meaning, this object will have an initialized :c:`base` pointer and an :c:`offset` greater than or equal to :c:`DEFAULT_ROOT` as defined in dependencies. Using an uninitialized :c:`ro` is an invalid operation and the program will likely return an invalid :c:`runic_obj_t` as described in :c:`runic_root()`.


Returns:
========

Any of the following:

	.. code-block:: c

		typedef enum runic_obj_ty {
			NODE, ATOM
		} runic_obj_ty_t;

Each :c:`runic_obj_t` contains a tag identifiying it as either a :c:`NODE` or :c:`ATOM`, in addition to its children or value, respectively. That tag data is evaluated and converted to the type corresponding to it. The type is returned. In the event that a malformed or otherwise invalid :c:`runic_obj_t` is used, either a :c:`-1` is returned or the program will exit. Using an invalid :c:`runic_obj_t` is not encouraged.