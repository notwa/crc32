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

static void crc_fill_big_endian_table()
{
	const ulong lsb = 1 << 31; /* least significant bit */
	ulong c;
	int i;

	for (c = 0; c < TABLE_SIZE; c++) {
		crc_be_table[c] = c << 24;
		for (i = 0; i < 8; i++) {
			if (crc_be_table[c] & lsb) {
				crc_be_table[c] <<= 1;
				crc_be_table[c] ^= crc_polynomial;
			} else {
				crc_be_table[c] <<= 1;
			}
			crc_be_table[c] &= 0xFFFFFFFF;
		}
	}
}

static void crc_fill_little_endian_table()
{
	const ulong lsb = 1; /* least significant bit */
	const ulong reflected = crc_reflect(crc_polynomial);
	ulong c;
	int i;
	
	for (c = 0; c < TABLE_SIZE; c++) {
		crc_le_table[c] = c;
		for (i = 0; i < 8; i++) {
			if (crc_le_table[c] & lsb) {
				crc_le_table[c] >>= 1;
				crc_le_table[c] ^= reflected;
			} else {
				crc_le_table[c] >>= 1;
			}
		}
	}
}

static void crc_fill_tables_once()
{
	static char filled = 0;
	if (crc_big_endian && !(filled & 1)) {
		crc_fill_big_endian_table();
		filled |= 1;
	} else if (!crc_big_endian && !(filled & 2)) {
		crc_fill_little_endian_table();
		filled |= 2;
	}
}

void crc_cycle(ulong *remainder, char c)
{
	crc_fill_tables_once();
	if (crc_big_endian) {
		const ulong byte = crc_be_table[((*remainder) >> 24) ^ c];
		*remainder = ((*remainder) << 8) ^ byte;
		*remainder &= 0xFFFFFFFF;
	} else {
		const ulong byte = crc_le_table[((*remainder) ^ c) & 0xFF];
		*remainder = ((*remainder) >> 8) ^ byte;
	}
}
