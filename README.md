crc32
=====

It computes [crc32s,][crc] and what of it?

[crc]: http://en.wikipedia.org/wiki/Cyclic_redundancy_check

Usage
----
```
crc32 - 32-bit cyclic redundancy check calculator

  -h, --help             display this text
      --license          show copyright & license information
  -i, --input f          open file f for reading (default: stdin)
  -s, --start-at n       start cycle with n (default: 0xFFFFFFFF)
  -p, --polynomial n     use n as the crc divisor (default: 0x04C11DB7)
  -e, --big-endian       use big endian calculations (default: little)
  -b, --binary           output as binary (default: hex with newline)
  -x, --xor              xor the output by 0xFFFFFFFF
  -r, --reflect          reverse the bits of the output

numbers (n) may be entered as hexadecimal or octal with prefixes
```

Notes
-----

*	Does not pad input.
*	Big endian calculations are somewhat untested.

