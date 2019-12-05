.. runic documentation master file, created by
   sphinx-quickstart on Mon Jul 29 19:04:15 2019.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to runic's documentation!
=================================

.. code-block:: c

	/****
	 * runic.h - API for .runic filetype
	 *           covering opening, closing, and 
	 *           modifying source code files 
	 *           directly on disk using a tree
	 *           format.
	****/

The following Documentation covers the runic API and it’s uses – This includes all functions, data structures, and enumerated types. This Documentation also covers API dependencies, constants and preprocessor directives, ultimately serving as a complete guide to integrate the runic API in your application.

**Contents:**

.. toctree::
   :maxdepth: 1

   runic
   intro
   deps_and_refs
   runic_t
   runic_obj_t
   runic_open
   runic_close
   runic_root
   runic_free
   runic_remaining
   runic_set_root
   runic_shrink
   runic_alloc_node
   runic_alloc_atom
   runic_alloc_atom_str
   runic_obj_ty
   runic_node_left
   runic_node_right
   runic_node_set_left
   runic_node_set_right
   runic_atom_size
   runic_atom_read
   runic_atom_write

Indices and tables
==================

* :ref:`search`

