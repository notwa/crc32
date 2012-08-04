/* Copyright (C) 2012 Connor Olding
 *
 * This program is licensed under the terms of the MIT License, and
 * is distributed without any warranty.  You should have received a
 * copy of the license along with this program; see the file LICENSE.
 */

#ifndef CRC32_H_
#define CRC32_H_

#include <stdlib.h>

/* interface */

typedef unsigned long ulong;

void crc_cycle(ulong *remainder, char c);
ulong crc_reflect(ulong input);

void crc_set_big_endian();
void crc_set_little_endian();
void crc_set_polynomial(ulong p);

/* implementation */

static char crc_big_endian = 0;
static const int crc_table_size = 0x100;
static ulong *crc_be_table = NULL; /* big endian */
static ulong *crc_le_table = NULL; /* little endian */
static ulong crc_polynomial = 0x04C11DB7;

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

static ulong *crc_alloc(size_t size)
{
	ulong *p = (ulong*) malloc(size * sizeof(ulong));
	if (p == NULL)
		exit(1);
	return p;
}

static void crc_allocate_tables_once()
{
	static char allocated = 0;
	if (crc_big_endian && !(allocated & 1)) {
		crc_be_table = crc_alloc(crc_table_size);
		allocated |= 1;
	} else if (!crc_big_endian && !(allocated & 2)) {
		crc_le_table = crc_alloc(crc_table_size);
		allocated |= 2;
	}
}

static void crc_fill_big_endian_table()
{
	const ulong least_significant_bit = 1 << 31;
	ulong c;
	int i;
	
	crc_allocate_tables_once();
	
	for (c = 0; c < crc_table_size; c++) {
		crc_be_table[c] = c << 24;
		for (i = 0; i < 8; i++) {
			if (crc_be_table[c] & least_significant_bit) {
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
	const ulong least_significant_bit = 1;
	const ulong reflected_polynomial = crc_reflect(crc_polynomial);
	ulong c;
	int i;
	
	crc_allocate_tables_once();
	/* printf("p: %08X\n", (int)reflected_polynomial); */
	
	for (c = 0; c < crc_table_size; c++) {
		crc_le_table[c] = c;
		for (i = 0; i < 8; i++) {
			if (crc_le_table[c] & least_significant_bit) {
				crc_le_table[c] >>= 1;
				crc_le_table[c] ^= reflected_polynomial;
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

void crc_set_big_endian()
{
	crc_big_endian = 1;
}

void crc_set_little_endian()
{
	crc_big_endian = 0;
}

void crc_set_polynomial(ulong p)
{
	crc_polynomial = p;
}

void crc_cycle(ulong *remainder, char c)
{
	crc_fill_tables_once();
	if (crc_big_endian) {
		const ulong newByte = crc_be_table[((*remainder) >> 24) ^ c];
		/* printf("%08X00 ^ %08X =\n",
			(int)(*remainder), (int)newByte); */
		*remainder = ((*remainder) << 8) ^ newByte;
		*remainder &= 0xFFFFFFFF;
	} else {
		const ulong newByte = crc_le_table[((*remainder) ^ c) & 0xFF];
		/* printf("  %08X ^ %08X =\n",
			(int)((*remainder) >> 8), (int)newByte); */
		*remainder = ((*remainder) >> 8) ^ newByte;
	}
}

#endif
