/* Copyright (C) 2012 Connor Olding
 *
 * This program is licensed under the terms of the MIT License, and
 * is distributed without any warranty.  You should have received a
 * copy of the license along with this program; see the file LICENSE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned long ulong;

#include "crc32.h"
#include "args.h"

#ifdef _MSC_VER
	#define FREOPEN_BLANK ("")
#else
	#define FREOPEN_BLANK (NULL)
#endif

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 4096
#endif
char buff[BUFFER_SIZE];

typedef struct string_node_s string_node;
struct string_node_s {
	char *s;
	string_node *next;
};

static string_node *input_node = NULL;

static ulong starting = 0xFFFFFFFF;
static char big_endian = 0;
static ulong polynomial = 0x04C11DB7;
static char print_binary = 0;
static char xor_output = 1;
static char reflect_output = 0;

static const char help1[] = "\
crc32 - a 32-bit cyclic rendundancy check calculator\n\
\n\
  <files...>        open files as inputs\n\
  -h                display this text\n\
  -s <n>            start cycle with n (default: 0xFFFFFFFF)\n\
  -p <n>            use n as the crc divisor (default: 0x04C11DB7)\n\
  -e                use big endian calculations\n\
  -b                output as binary\n\
  -x                NOT the output\n\
  -r                reverse output's bits\n\
\n\
";
static const char help2[] = "\
numbers <n> may be entered as hexadecimal or octal with prefixes\n\
";

static char *check_next(char flag, char *next) {
	if (!next) {
		fprintf(stderr, "-%c requires another argument\n", flag);
		exit(1);
	}
	return next;
}

static void handle_flag(char flag, char *(*nextarg)())
{
	char *next;
	switch (flag) {
	case 'h':
		printf(help1);
		printf(help2);
		exit(0);
	case 'e':
		big_endian = 1;
		return;
	case 'b':
		print_binary = 1;
		return;
	case 'x':
		xor_output = 0;
		return;
	case 'r':
		reflect_output = 1;
		return;
	case 's':
		next = check_next(flag, nextarg());
		starting = strtoul(next, NULL, 0);
		break;
	case 'p':
		next = check_next(flag, nextarg());
		polynomial = strtoul(next, NULL, 0);
		break;
	default:
		fprintf(stderr, "Unknown flag: -%c\n", flag);
		exit(1);
	}
}

static void add_input(char *arg)
{
	static string_node *last = NULL;
	string_node *n = calloc(1, sizeof(string_node));
	if (!n) {
		fprintf(stderr, "calloc failed\n");
		exit(1);
	}

	n->s = arg;
	if (!input_node)
		input_node = n;
	else
		last->next = n;
	last = n;
}

static FILE *open_stream(char *filename)
{
	FILE *stream = NULL;
	stream = fopen(filename, "rb");
	if (stream == NULL) {
		perror(filename);
		exit(1);
	}
	return stream;
}


static ulong cycle_file(FILE *stream)
{
	ulong remainder = starting;
	void (*cycle)(ulong*, ulong*, char) =
	    (big_endian) ? crc_be_cycle : crc_le_cycle;
	ulong table[CRC_TABLE_SIZE];
	int i, len;

	crc_fill_table(table, big_endian, polynomial);
	do {
		len = fread(buff, 1, BUFFER_SIZE, stream);
		if (ferror(stream)) {
			perror(NULL);
			exit(1);
		}

		for (i = 0; i < len; i++)
			cycle(table, &remainder, buff[i]);
	} while (!feof(stream));

	if (xor_output)
		remainder ^= 0xFFFFFFFF;
	if (reflect_output)
		remainder = crc_reflect(remainder);
	return remainder;
}

static void print_crc(ulong remainder)
{
	if (print_binary)
		fwrite(&remainder, sizeof(remainder), 1, stdout);
	else
		printf("%08X\n", (int) remainder);
}

int main(int argc, char **argv)
{
	string_node *n;
	args_parse(argc, argv, handle_flag, add_input);

	if (!input_node) {
		freopen(FREOPEN_BLANK, "rb", stdin);
		print_crc(cycle_file(stdin));
	}
	for (n = input_node; n; n = n->next) {
		FILE *stream = open_stream(n->s);
		print_crc(cycle_file(stream));
		fclose(stream);
		free(n);
	}
	return 0;
}
