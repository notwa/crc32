/* Copyright (C) 2012 Connor Olding
 *
 * This program is licensed under the terms of the MIT License, and
 * is distributed without any warranty.  You should have received a
 * copy of the license along with this program; see the file LICENSE.
 */

#include <stdlib.h>

typedef unsigned long ulong;

#include "crc32.h"

int crc_big_endian = 0;
ulong crc_polynomial = 0x04C11DB7;

enum {
	TABLE_SIZE = 0x100
};
static ulong crc_be_table[TABLE_SIZE]; /* big endian */
static ulong crc_le_table[TABLE_SIZE]; /* little endian */

ulong crc_reflect(ulong input)
{
	ulong reflected = 0;
	int i;
	for (i = 0; i < 4 * 8; i++) {
		reflected <<= 1;
		reflected |= input & 1;
		input >>= 1;
	}
	return reflected;
}

static void crc_fill_table(int big)
{
	ulong lsb = (big) ? 1 << 31 : 1; /* least significant bit */
	ulong poly = (big) ? crc_polynomial : crc_reflect(crc_polynomial);
	ulong *tc = (big) ? crc_be_table : crc_le_table;
	int c, i;

	for (c = 0; c < TABLE_SIZE; c++, tc++) {
		*tc = (big) ? c << 24 : c;
		for (i = 0; i < 8; i++) {
			if (*tc & lsb) {
				*tc = (big) ? *tc << 1 : *tc >> 1;
				*tc ^= poly;
			} else {
				*tc = (big) ? *tc << 1 : *tc >> 1;
			}
			*tc &= 0xFFFFFFFF;
		}
	}
}

void crc_be_cycle(ulong *remainder, char c)
{
	static char filled = 0;
	ulong byte;
	if (!filled) {
		crc_fill_table(1);
		filled = 1;
	}
	byte = crc_be_table[((*remainder) >> 24) ^ c];
	*remainder = (((*remainder) << 8) ^ byte) & 0xFFFFFFFF;
}

void crc_le_cycle(ulong *remainder, char c)
{
	static char filled = 0;
	ulong byte;
	if (!filled) {
		crc_fill_table(0);
		filled = 1;
	}
	byte = crc_le_table[((*remainder) ^ c) & 0xFF];
	*remainder = ((*remainder) >> 8) ^ byte;
}
