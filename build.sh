#!/bin/sh
####
# build.sh - An automated tool for building
#			 all makefiles in this repository.
#
#			 This keeps makefiles clean and
#			 divided based on subdirectory,
#			 allowing easy management of all
#			 files.
#
#			 All executables will be deposited
#			 into the root directory.
####


build()
{
	cd runic/
	make
	cd ../test/
	make
	cd ../wordlist/v1/
	make
	cd ../v2/
	make
	cd ../../
	make
}

clean()
{
	cd runic/
	make clean
	cd ../test/
	make clean
	cd ../wordlist/v1/
	make clean
	cd ../v2/
	make clean
	cd ../../
	make clean
}

if [ "$#" -gt "0" ]
then
	if [ "$1" = "build" ]
	then
		echo "Building Makefiles...\n"
		build
	fi

	if [ "$1" = "clean" ]
	then
		echo "Cleaning directories...\n"
		clean
	fi
else
	echo "Use arguments 'build' or 'clean' for execution.\n"
fi
