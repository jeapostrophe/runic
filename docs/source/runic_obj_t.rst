.. role:: c(code)
   :language: c

===================
:c:`// runic_obj_t`
===================

The other critical element of file manipulation is handled through the :c:`runic_obj_t` artifact. The file contains many :c:`runic_obj_t` derivatives in memory, and they are broken down into two major categories: :c:`NODE` and :c:`ATOM`. As previously mentioned in the :c:`runic_t` section, intelligent use of the two object types allow manipulation and creation of abstract syntax trees. Each object (:c:`NODE` and :c:`ATOM`) contains a tag. This tag is either assigned :c:`0` (as defined by :c:`NODE_TAG_VALUE`) or a positive integer equal to the size in bytes of the respective :c:`ATOM` string.

:c:`// node`
================
Beyond the tag, each :c:`NODE` contains two data elements (a left and right child) which will be used to reference other :c:`NODE` or :c:`ATOM`. The overall size of a node is 17 bytes. 

The following operations can be performed on a :c:`NODE`: 

.. code-block:: c

	// accessors 
	runic_obj_ty_t runic_obj_ty(runic_obj_t ro);
	runic_obj_t runic_node_left(runic_obj_t ro);
	runic_obj_t runic_node_right(runic_obj_t ro); 

	// mutators
	bool runic_node_set_left(runic_obj_t* parent, runic_obj_t child);
	bool runic_node_set_right(runic_obj_t* parent, runic_obj_t child);

:c:`// atom`
================
As mentioned, an :c:`ATOM` contains the size of value (an ascii string) in bytes in the tag field, as well as the value itself. The overall size of an :c:`ATOM` is therefore variable length, equal to the ascii string + 1(the tag). 

The following operations can be performed on an :c:`ATOM`: 

.. code-block:: c

	// accessors
	runic_obj_ty_t runic_obj_ty(runic_obj_t ro); 
	size_t runic_atom_size(runic_obj_t ro);
	bool runic_atom_read(runic_obj_t ro, char* c);

	// mutators
	bool runic_atom_write(runic_obj_t* ro, const char* val);
