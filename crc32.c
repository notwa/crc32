enum { CRC_TABLE_SIZE = 0x100 };

ulong
crc_reflect(ulong input)
{
	ulong reflected = 0;
	for (int i = 0; i < 4 * 8; i++) {
		reflected <<= 1;
		reflected |= input & 1;
		input >>= 1;
	}
	return reflected;
}

void
crc_fill_table(ulong *table, int big, ulong polynomial)
{
	ulong lsb = (big) ? 1 << 31 : 1; /* least significant bit */
	ulong poly = (big) ? polynomial : crc_reflect(polynomial);

	for (int c = 0; c < CRC_TABLE_SIZE; c++, table++) {
		*table = (big) ? c << 24 : c;
		for (int i = 0; i < 8; i++) {
			if (*table & lsb) {
				*table = (big) ? *table << 1 : *table >> 1;
				*table ^= poly;
			} else {
				*table = (big) ? *table << 1 : *table >> 1;
			}
			*table &= 0xFFFFFFFF;
		}
	}
}

void
crc_be_cycle(ulong *table, ulong *remainder, char c)
{
	ulong byte = table[(((*remainder) >> 24) ^ c) & 0xff];
	*remainder = (((*remainder) << 8) ^ byte) & 0xFFFFFFFF;
}

void
crc_le_cycle(ulong *table, ulong *remainder, char c)
{
	ulong byte = table[((*remainder) ^ c) & 0xFF];
	*remainder = ((*remainder) >> 8) ^ byte;
}
