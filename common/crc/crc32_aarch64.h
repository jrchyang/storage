#ifndef CRC32_AARCH64_H
#define CRC32_AARCH64_H

#include <stdint.h>

#ifdef HAVE_ARMV8_CRC
extern uint32_t
crc32_aarch64(uint32_t crc, unsigned char const *buffer, unsigned len);
#else
static inline uint32_t
crc32_aarch64(uint32_t crc, unsigned char const *buffer, unsigned len) {
  return 0;
}
#endif

#endif // CRC32_AARCH64_H
