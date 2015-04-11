enum { CRC_TABLE_SIZE = 0x100 };

static inline uint32_t
crc_reflect(uint32_t input)
{
	uint32_t reflected = 0;
	for (int i = 0; i < 4 * 8; i++) {
		reflected <<= 1;
		reflected |= input & 1;
		input >>= 1;
	}
	return reflected;
}

static inline void
crc_fill_table(uint32_t *table, int big, uint32_t polynomial)
{
	uint32_t lsb = (big) ? 1 << 31 : 1; /* least significant bit */
	uint32_t poly = (big) ? polynomial : crc_reflect(polynomial);

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

static inline void
crc_be_cycle(uint32_t *table, uint32_t *remainder, char c)
{
	uint32_t byte = table[(((*remainder) >> 24) ^ c) & 0xff];
	*remainder = (((*remainder) << 8) ^ byte) & 0xFFFFFFFF;
}

static inline void
crc_le_cycle(uint32_t *table, uint32_t *remainder, char c)
{
	uint32_t byte = table[((*remainder) ^ c) & 0xFF];
	*remainder = ((*remainder) >> 8) ^ byte;
}
