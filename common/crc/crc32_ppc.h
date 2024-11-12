#ifndef CRC32_PPC_H
#define CRC32_PPC_H

#include <stdint.h>

extern uint32_t
crc32_ppc(uint32_t crc, unsigned char const *buffer, unsigned len);

#endif // CRC32_PPC_H
