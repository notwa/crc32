/* Copyright (C) 2012 Connor Olding
 *
 * This program is licensed under the terms of the MIT License, and
 * is distributed without any warranty.  You should have received a
 * copy of the license along with this program; see the file LICENSE.
 */

#include <stdio.h>
#include <stdlib.h>

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

char args_info[] = "crc32 - 32-bit cyclic redundancy check calculator\n";
char args_usage[] = "Usage: %s [-i f] [-s n] [-p n] [-e] [-x] [-r] [-b]\n";
void args_print_help_suffix()
{
	puts("\n\
numbers (n) can be entered as hexadecimal or octal with prefixes");
}

const int args_switch_count = 9;
char* args_switches[] = {
"-h","--help","            display this text",
"  ","--license","         show copyright & license information",
"-i","--input","f          open file f for reading (default: stdin)",
"-s","--start-at","n       start cycle with n (default: 0xFFFFFFFF)",
"-p","--polynomial","n     use n as the crc divisor (default: 0x04C11DB7)",
"-e","--big-endian","      use big endian calculations (default: little)",
"-b","--binary","          output as binary (default: hex with newline)",
"-x","--xor","             xor the output by 0xFFFFFFFF",
"-r","--reflect","         reverse the bits of the output",
};

void print_license()
{
	puts("crc32  Copyright (C) 2012 Connor Olding\n\
This program comes with ABSOLUTELY NO WARRANTY, and is free software.\n\
See the file \"LICENSE\" or <http://gnu.org/licenses/gpl.txt> for details.");
	exit(0);
}

void start_at()
{
	remainder = strtoul(args_poll(), NULL, 0);
}

void set_input()
{
	input_filename = args_poll();
}

void set_polynomial()
{
	const int p = strtoul(args_poll(), NULL, 0);
	crc_set_polynomial(p);
}

void set_binary()
{
	print_binary = 1;
}

void set_xor()
{
	xor_output = 0;
}

void set_reflect()
{
	reflect_output = 1;
}

void (*args_functions[])() =
	{args_print_help, print_license, set_input, start_at, set_polynomial,
	crc_set_big_endian, set_binary, set_xor, set_reflect};

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
	args_handle(argc, argv);
	cycle_input();
	print_crc();
	return 0;
}
