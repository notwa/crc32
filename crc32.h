/* Copyright (C) 2012 Connor Olding
 *
 * This program is licensed under the terms of the MIT License, and
 * is distributed without any warranty.  You should have received a
 * copy of the license along with this program; see the file LICENSE.
 */

ulong crc_polynomial;

void crc_be_cycle(ulong *remainder, char c);
void crc_le_cycle(ulong *remainder, char c);
ulong crc_reflect(ulong input);
