#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "crc32.c"
#include "args.c"

typedef int bool;

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 4096
#endif
char buff[BUFFER_SIZE];

#define MAX_INPUTS 256

static char *inputs[MAX_INPUTS];
static unsigned int input_n = 0;

static uint32_t starting = 0xFFFFFFFF;
static bool big_endian = 0;
static uint32_t polynomial = 0x04C11DB7;
static bool print_binary = 0;
static bool xor_output = 1;
static bool reflect_output = 0;

static const char help[] = "\
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
numbers <n> may be in hexadecimal or octal using proper prefixes\n\
";

static char
*check_next(char flag, char *next) {
	if (!next) {
		fprintf(stderr, "-%c requires another argument\n", flag);
		exit(1);
	}
	return next;
}

static void
handle_flag(char flag, char *(*nextarg)())
{
	char *next;
	switch (flag) {
	case 'h': {
		printf(help);
	} exit(0);
	case 'e': {
		big_endian = 1;
	} break;
	case 'b': {
		print_binary = 1;
	} break;
	case 'x': {
		xor_output = 0;
	} break;
	case 'r': {
		reflect_output = 1;
	} break;
	case 's': {
		next = check_next(flag, nextarg());
		starting = strtoul(next, NULL, 0);
	} break;
	case 'p': {
		next = check_next(flag, nextarg());
		polynomial = strtoul(next, NULL, 0);
	} break;
	default: {
		fprintf(stderr, "Unknown flag: -%c\n", flag);
	} exit(1);
	}
}

static void
add_input(char *arg)
{
	if (input_n >= MAX_INPUTS) {
		fprintf(stderr, "Too many inputs specified\n");
		exit(1);
	}
	inputs[input_n++] = arg;
}

static FILE *
open_stream(char *filename)
{
	FILE *stream = NULL;
	stream = fopen(filename, "rb");
	if (stream == NULL) {
		perror(filename);
		exit(1);
	}
	return stream;
}

static inline size_t
buff_read(FILE *stream)
{
	size_t len = fread(buff, 1, BUFFER_SIZE, stream);
	if (ferror(stream)) {
		perror(NULL);
		exit(1);
	}
	return len;
}

static uint32_t
cycle_file(FILE *stream)
{
	uint32_t remainder = starting;
	uint32_t table[CRC_TABLE_SIZE];

	if (big_endian)
		crc_be_fill_table(table, polynomial);
	else
		crc_le_fill_table(table, polynomial);

	if (big_endian) do {
		size_t len = buff_read(stream);
		for (size_t i = 0; i < len; i++)
			crc_be_cycle(table, &remainder, buff[i]);
	} while (!feof(stream)); else do {
		size_t len = buff_read(stream);
		for (size_t i = 0; i < len; i++)
			crc_le_cycle(table, &remainder, buff[i]);
	} while (!feof(stream));

	if (xor_output)
		remainder ^= 0xFFFFFFFF;
	if (reflect_output)
		remainder = crc_reflect(remainder);
	return remainder;
}

static void
print_crc(uint32_t remainder)
{
	if (print_binary)
		fwrite(&remainder, sizeof(remainder), 1, stdout);
	else
		printf("%08X\n", (int) remainder);
}

int
main(int argc, char **argv)
{
	args_parse(argc, argv, handle_flag, add_input);

	if (!input_n) {
		freopen(NULL, "rb", stdin);
		print_crc(cycle_file(stdin));
	}
	for (int i = 0; i < input_n; i++) {
		FILE *stream = open_stream(inputs[i]);
		print_crc(cycle_file(stream));
		fclose(stream);
	}
	return 0;
}
