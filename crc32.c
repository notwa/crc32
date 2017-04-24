enum { CRC_TABLE_SIZE = 0x100 };

static inline uint32_t
crc_reflect(uint32_t input)
{
	uint32_t reflected = 0;
	for (int i = 0; i < 32; i++) {
		reflected <<= 1;
		reflected |= input & 1;
		input >>= 1;
	}
	return reflected;
}

static inline void
crc_be_fill_table(uint32_t *table, uint32_t polynomial)
{
	const uint32_t lsb = 1 << 31;
	uint32_t poly = polynomial;

	for (int c = 0; c < CRC_TABLE_SIZE; c++) {
		uint32_t v = c << 24;
		for (int i = 0; i < 8; i++) {
			if (v & lsb) {
				v <<= 1;
				v ^= poly;
			} else {
				v <<= 1;
			}
		}
		*table++ = v;
	}
}

static inline void
crc_le_fill_table(uint32_t *table, uint32_t polynomial)
{
	const uint32_t lsb = 1;
	uint32_t poly = crc_reflect(polynomial);

	for (int c = 0; c < CRC_TABLE_SIZE; c++) {
		uint32_t v = c;
		for (int i = 0; i < 8; i++) {
			if (v & lsb) {
				v >>= 1;
				v ^= poly;
			} else {
				v >>= 1;
			}
		}
		*table++ = v;
	}
}

static inline void
crc_be_cycle(uint32_t *table, uint32_t *remainder, uint8_t c)
{
	const uint8_t i = (*remainder >> 24) ^ c;
	*remainder = (*remainder << 8) ^ table[i];
}

static inline void
crc_le_cycle(uint32_t *table, uint32_t *remainder, uint8_t c)
{
	const uint8_t i = (*remainder & 0xFF) ^ c;
	*remainder = (*remainder >> 8) ^ table[i];
}
