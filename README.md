crc32
=====

A [crc32][crc] calculator written in C and licensed under the [GPLv3 license.][gpl]

[crc]: http://en.wikipedia.org/wiki/Cyclic_redundancy_check
[gpl]: http://www.gnu.org/licenses/gpl.txt

Compilation
-----------

	gcc -ansi -pedantic -Wall -Werror -O3 main.c -o crc32

or for Visual Studio users

	cl /O2 /Ot /W2 /WX /Fecrc32 main.c

Help
----

	  -h, --help             display this text
	  -i, --input f          open file f for reading (default: stdin)
	  -s, --start-at n       start cycle with n (default: 0xFFFFFFFF)
	  -p, --polynomial n     use n for crc calculations (default: 0x04C11DB7)
	  -e, --big-endian       use big endian calculations (default: little)
	  -b, --binary           output as binary (default: hex with newline)
	  -x, --xor              xor the output by 0xFFFFFFFF
	  -r, --reflect          reverse the bits of the output
	
	numbers (n) can be entered as hexadecimal or octal with prefixes

Notes
-----

*	Does not pad input.
*	Big endian calculations may not be accurate. I'll look into this.
