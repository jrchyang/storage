#ifndef CRC32_INTEL_BASELINE_H
#define CRC32_INTEL_BASELINE_H

#include <stdint.h>

extern uint32_t
crc32_intel_baseline(uint32_t crc, unsigned char const *buffer, unsigned len);

#endif // CRC32_INTEL_BASELINE_H
