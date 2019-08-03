.. role:: c(code)
   :language: c

===========================
Dependencies and References
===========================

:c:`// dependencies`
====================

The following dependencies are a complete list of the components required to use the runic API. Runic assumes a UNIX compatible operating system (e.g.: Linux, BSD, macOS). At this time, there is no support for Windows. Attempting to run programs linked with runic without these dependencies is considered undefined behavior and will most likely result in a compiler error.

The list is as follows:

- :c:`stdio.h`		- Console I/O
- :c:`stdlib.h`		– System-level (entry/exit) operations
- :c:`stddef.h`		– Data type compatibility
- :c:`stdbool.h`	– Data type compatibility
- :c:`string.h`		– String and data manipulation
- :c:`fcntl.h`		– File control
- :c:`unistd.h`		– Accessing system configuration
- :c:`sys/stat.h`	– File status access
- :c:`sys/mman.h`	– Memory mapping

:c:`// preprocessor statements`
===============================

The following preprocessor statements allow code execution in a C++ environment and prevent redeclaration/duplication of symbols.

.. code-block:: c

	#ifndef RUNIC_H
	#define RUNIC_H
	#ifdef __cplusplus 
		extern "C" {
	#endif 

The following statements are required to “bookend” the opening statements at the top of the header file.

.. code-block:: c

	// closing statements
	#ifndef __cplusplus
		}
	#endif 
	#endif /* runic.h */

:c:`// constants`
=================

The following constants define values used throughout the operation of this library. :c:`DEFAULT_ROOT` defines the first legal memory location for data after the file header, :c:`HEADER_SIZE` defines the size of the magic number used by the file, and :c:`NODE_TAG_VALUE` defines how the API identifies :c:`NODE` from :c:`ATOM`. To learn more about :c:`NODE` and :c:`ATOM`, please navigate to their respective sections in the documentation.


.. code-block:: c

	#define DEFAULT_ROOT 0x15
	#define HEADER_SIZE 0x05
	#define NODE_TAG_VALUE 0x00
