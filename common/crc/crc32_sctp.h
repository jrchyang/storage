#ifndef CRC32_SCTP_H
#define CRC32_SCTP_H

#include <stdint.h>

extern uint32_t
crc32_sctp(uint32_t crc, unsigned char const *data, unsigned length);

#endif // CRC32_SCTP_H
