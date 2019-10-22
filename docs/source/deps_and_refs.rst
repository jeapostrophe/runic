.. role:: c(code)
   :language: c

===========================
Dependencies and References
===========================

:c:`// dependencies`
====================

The following dependencies are a complete list of the components required to use the runic API. Runic assumes a UNIX compatible operating system (e.g.: Linux, BSD, macOS). At this time, there is no support for Windows. Attempting to run programs linked with runic without these dependencies is considered undefined behavior and will most likely result in a compiler error.

The list is as follows:

- :c:`stdio.h`		- Console and System I/O
- :c:`stdlib.h`		– System-level (entry/exit) operations
- :c:`stdbool.h`	– Boolean type compatibility
- :c:`stddef.h`		– Data type compatibility
- :c:`stdint.h`		– Integer type compatibility
- :c:`string.h`		– String and memory manipulation
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

There is one constant exposed to the user which can be used to perform safety checks. :c:`DEFAULT_ROOT` defines the first legal memory location for any data after the file header. :c:`runic_obj_t`'s which have :c:`offset`s with values less than :c:`DEFAULT_ROOT` are effectively :c:`NULL` and should be treated as such. Writing information below this memory location will corrupt the file.

.. code-block:: c

	#define DEFAULT_ROOT 0x15
