#ifndef CRC32_INTEL_FAST_H
#define CRC32_INTEL_FAST_H

#include <stdint.h>

/* is the fast version compiled in */
extern int crc32_intel_fast_exists(void);

#ifdef __x86_64__
extern uint32_t
crc32_intel_fast(uint32_t crc, unsigned char const *buffer, unsigned len);
#else
static inline uint32_t
crc32_intel_fast(uint32_t crc, unsigned char const *buffer, unsigned len) {
  return 0;
}
#endif

#endif // CRC32_INTEL_FAST_H
