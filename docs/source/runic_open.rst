.. role:: c(code)
   :language: c

====================================================
:c:`runic_t runic_open(const char* path, int mode);`
====================================================


The above function signature outlines the operation regarding the opening of a runic file.

Arguments:
==========

	**path** – A string literal or character pointer which references a c-string that contains the file path to the file a user wishes to open. This address must be a valid UNIX compatible path. The format may either be path-relative to the location of the executable (e.g.: :c:`"../../Documents/sample.runic"`), or an absolute path (e.g. :c:`"/usr/admin/Documents/sample.runic"`). Relative pathing from the home directory (e.g.: :c:`"~/Documents/sample.runic"`) is invalid.

	Specifying an invalid path will cause the function to return, providing a :c:`runic_t` which has a base pointer equal to :c:`NULL` and no other initialized data elements.

	**mode** – any of the following:


	.. code-block:: c

		enum runic_file_modes {
			READONLY, READWRITE, CREATEWRITE
		};

	:c:`READONLY` – Opening a file in this mode will prevent any changes to disk, but allow access to read the contents of the file. All accessor functions will work in this mode. If mutator functions are used, this is considered undefined behavior. The program may allow changes in the working memory of the file, but no changes will be reflected on disk. Using mutators on files opened in this mode is not encouraged.

	:c:`READWRITE` – Opening a file in this mode will allow changes to be written to disk. :c:`runic_t` does not make use of any file buffers or similar mechanisms, so all changes made from as a result of a mutator function are immediately reflected on disk. As a result, there is no “save” operation necessary. If the file does not already exist on disk, the program will return an invalid :c:`runic_t` object.

	:c:`CREATEWRITE` – Opening a file in this mode will create a file on disk and allow changes to be made to it. If the file already exists on disk, it will be destroyed and overwritten by a new file with the same name. If the user attempts to specify a directory as a runic file (e.g.: :c:`runic_open("~/Documents", CREATEWRITE)`) the operation will fail and an invalid :c:`runic_t` will be returned. If a user attempts to enter another file mode (e.g.: :c:`4`, :c:`19`, etc.), open will default to :c:`CREATEWRITE`. 

Returns:
========

Following a successful open, the program will return a :c:`runic_t` with initialized values enabling further use of the file: 

:c:`typedef struct runic {`
	:c:`const char* path;`	– This value will be the same as the argument used to open the file.

	:c:`int fd;`			– A unique integer representing the file descriptor for this file.

	:c:`struct stat sb;`	– A status buffer with information about the file, including size, etc.

	:c:`int mode;`			– This value will be the same as the argument used to open the file.

	:c:`uint8_t* base;`		– A pointer to the first address of a contiguous block of memory where the file exists in heap space.

:c:`} runic_t;`

Following an unsuccessful open for any of the reasons described above, the program will return a :c:`runic_t` where all values will be uninitialized, except base, which will be initialized with :c:`NULL`. This object is invalid, and all other functions will recognize it as such.
