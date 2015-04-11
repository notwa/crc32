/* Copyright (C) 2012 Connor Olding
 *
 * This program is licensed under the terms of the MIT License, and
 * is distributed without any warranty.  You should have received a
 * copy of the license along with this program; see the file LICENSE.
 */

void args_parse(int argc, char **argv,
    void flagfn(char, char*()), void plainfn(char*));
