crc32
-----

A [crc32][crc] calculator written in C and licensed under GPLv3.

[crc]: http://en.wikipedia.org/wiki/Cyclic_redundancy_check "cycle redundancy check on wikipedia"

Compilation
===========

	gcc -ansi -pedantic -Wall -Werror -O3 main.c -o crc32

or for Visual Studio users

	cl /O2 /Ot /W2 /WX /Fecrc32 main.c

Help
====



Notes
=====

*	Defaults to comply to IEEE-802.3 specs.
*	Big endian calculations may not be accurate. I'll look into this.
