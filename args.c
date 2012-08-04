/* Copyright (C) 2012 Connor Olding
 *
 * This program is licensed under the terms of the MIT License, and
 * is distributed without any warranty.  You should have received a
 * copy of the license along with this program; see the file LICENSE.
 */

#include "args.h"

#include <stddef.h>

static int argc, argi;
static char **argv, *flag;

static char *nextarg()
{
	char *temp = flag;
	flag = NULL;
	if (temp[1])
		return temp + 1;
	if (++argi == argc)
		return NULL;
	return argv[argi];
}

void args_parse(int argc_, char **argv_,
    void flagfn(char, char*()), void plainfn(char*))
{
	argc = argc_;
	argv = argv_;
	for (argi = 1; argi < argc; argi++) {
		char *arg = argv[argi];
		if (!arg[0])
			continue;

		/* if ... || arg == "-" */
		if (arg[0] != '-' || !arg[1]) {
			if (plainfn)
				plainfn(arg);
			continue;
		}
		/* if arg == "--" */
		if (arg[1] == '-' && !arg[2])
			break;

		for (flag = arg + 1; *flag; flag++) {
			flagfn(*flag, nextarg);
			if (!flag)
				break;
		}
	}
	for (; argi < argc; argi++)
		if (plainfn)
			plainfn(argv[argi]);
}

