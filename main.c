/* Copyright (C) 2012 Connor Olding
 *
 * This program is licensed under the terms of the MIT License, and
 * is distributed without any warranty.  You should have received a
 * copy of the license along with this program; see the file LICENSE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "crc32.h"
#include "args.h"

#ifdef _MSC_VER
	#define FREOPEN_BLANK ("")
#else
	#define FREOPEN_BLANK (NULL)
#endif

static ulong remainder = 0xFFFFFFFF;

static FILE* input_stream = NULL;
static char* input_filename = "-";
static char print_binary = 0;
static char xor_output = 1;
static char reflect_output = 0;

const char help1[] = "\
crc32 - a 32-bit cyclic rendundancy check calculator\n\
\n\
  -h                display this text\n\
  -i <file>         open file for reading (default: stdin)\n\
  -s <n>            start cycle with n (default: 0xFFFFFFFF)\n\
  -p <n>            use n as the crc divisor (default: 0x04C11DB7)\n\
  -e                use big endian calculations\n\
  -b                output as binary\n\
  -x                xor the output by 0xFFFFFFFF\n\
  -r                reverse the bits of the output\n\
\n\
";
const char help2[] = "\
numbers <n> may be entered as hexadecimal or octal with prefixes\n\
";

void handle_flag(char flag, char* (*nextarg)()) {
	/* TODO: check for NULL on nextarg */
	switch (flag) {
	case 'h':
		printf(help1);
		printf(help2);
		exit(0);
	case 'i':
		input_filename = nextarg();
		break;
	case 's':
		remainder = strtoul(nextarg(), NULL, 0);
		break;
	case 'p':
		crc_set_polynomial(strtoul(nextarg(), NULL, 0));
		break;
	case 'e':
		crc_set_big_endian();
		break;
	case 'b':
		print_binary = 1;
		break;
	case 'x':
		xor_output = 0;
		break;
	case 'r':
		reflect_output = 1;
		break;
	default:
		exit(1);
	}
}

static void open_stream()
{
	const char mode[] = "rb";
	if (!strcmp(input_filename, "-")) {
		freopen(FREOPEN_BLANK, mode, stdin);
		input_stream = stdin;
	} else {
		input_stream = fopen(input_filename, mode);
	}
	if (input_stream == NULL) {
		fprintf(stderr, "Could not open file for reading: %s\n",
			input_filename);
		exit(1);
	}
}

static void close_stream()
{
	fclose(input_stream);
}

static void cycle_input()
{
	int c;
	open_stream();
	while ((c = getc(input_stream)) != EOF)
		crc_cycle(&remainder, c);
	close_stream();
	
	if (xor_output)
		remainder ^= 0xFFFFFFFF;
	if (reflect_output)
		remainder = crc_reflect(remainder);
}

static void print_crc()
{
	if (print_binary)
		fwrite(&remainder, sizeof(remainder), 1, stdout);
	else
		printf("%08X\n", (int) remainder);
}

int main(int argc, char** argv)
{
	crc_set_little_endian();
	args_parse(argc, argv, handle_flag, NULL);
	cycle_input();
	print_crc();
	return 0;
}
