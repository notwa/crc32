crc32
=====

It computes [crc32s,][crc] and what of it?

[crc]: http://en.wikipedia.org/wiki/Cyclic_redundancy_check

Usage
----
```
crc32 - a 32-bit cyclic rendundancy check calculator

  <files...>        open files as inputs
  -h                display this text
  -s <n>            start cycle with n (default: 0xFFFFFFFF)
  -p <n>            use n as the crc divisor (default: 0x04C11DB7)
  -e                use big endian calculations
  -b                output as binary
  -x                NOT the output
  -r                reverse output's bits

numbers <n> may be entered as hexadecimal or octal with prefixes
```

Notes
-----

*	Does not pad input.
*	Big endian calculations are somewhat untested.

## [Smaller Still](https://gist.github.com/notwa/5689243)
