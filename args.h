/*	args.h - argument handling
	Copyright (C) 2012 Connor Olding

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ARGS_H_
#define ARGS_H_

#include <stdio.h>
#include <string.h>

/* interface */

extern char args_info[];
extern char args_usage[];
extern char args_help_suffix[];

extern const int args_switch_count;
extern char* args_switches[];
extern void (*args_functions[])();

void args_handle(int argc, char* argv[]);
char* args_poll();
void args_print_help();

/* implemenation */

static const int args_columns = 3;

static char* args_program_name = NULL;

static int args_argc = 0;
static char** args_argv = NULL;

static int args_current = 0;
static int args_previous = 0;

static char args_is_blank(char* s)
{
	if (s == NULL || s[0] == '\0' || s[0] == ' ')
		return 1;
	return 0;
}

static char args_is_short(char *s)
{
	if (!args_is_blank(s) && s[0] == '-' && s[1] != '\0' && s[1] != '-')
		return 1;
	return 0;
}

static void args_print_info()
{
	printf(args_info);
}

static void args_print_usage()
{
	printf(args_usage, args_program_name);
}

static void args_print_switches()
{
	int i;
	for (i = 0; i < args_switch_count * args_columns; i += args_columns) {
		printf("  ");
		if (args_is_blank(args_switches[i])) {
			printf("   "); 
		} else {
			printf("%s,", args_switches[i]);
		}
		printf(" %s %s\n",
			args_switches[i + 1],
			args_switches[i + 2]);
	}
	printf(args_help_suffix);
}

void args_print_help()
{
	args_print_info();
	puts("");
	args_print_switches();
	exit(0);
}

static int args_get_index(char* name)
{
	int i;
	for (i = 0; i < args_switch_count * args_columns; i++) {
		if (i % args_columns == args_columns - 1)
			continue; /* skip checking the description column */
		if (args_is_blank(args_switches[i]))
			continue;
		if (!strcmp(args_switches[i], name)) {
			return i / args_columns;
		}
	}
	return -1;
}

static char* args__poll(char wants_switch)
{
	static char short_[3] = "-\0";
	static int pos = 0;
	char* arg = NULL;

	if (args_current >= args_argc) {
		if (args_current - 1 == args_previous)
			fprintf(stderr, "%s requires an argument.\n",
				args_argv[args_previous]);
		else
			fprintf(stderr, "%s requires another argument.\n",
				args_argv[args_previous]);
		args_print_usage();
		exit(1);

	arg = args_argv[args_current];
	if (!wants_switch) {
		args_current++;
		return arg + pos * sizeof(char);
	}
	if (args_is_short(arg)) {
		if (pos == 0)
			pos++;
		short_[1] = arg[pos];
		pos++;
		if (arg[pos] == '\0') {
			pos = 0;
			args_current++;
		}
		return short_;
	} else {
		args_current++;
	}
	return arg;
}

static char* args_poll_for_switch()
{
	return args__poll(1);
}

char* args_poll()
{
	return args__poll(0);
}

void args_handle(int argc, char** argv)
{
	args_argc = argc;
	args_argv = argv;
	
	args_current = 0;
	args_program_name = args_poll();
	
	while (args_current < args_argc) {
		char* name;
		int index;
		
		args_previous = args_current;
		name = args_poll_for_switch();
		index = args_get_index(name);
		if (index < 0) {
			fprintf(stderr, "Unknown option: %s\n", name);
			args_print_usage();
			exit(1);
		}
		args_functions[index]();
	}
}

#endif

