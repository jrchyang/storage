#ifndef INLINE_MEMORY_H
#define INLINE_MEMORY_H

#include <string.h>
#include <cstddef>
#include <cstdint>

#include "global_definition.h"

#if defined(__GNUC__)

static inline void *
maybe_inline_memcpy(void *dest, const void *src, size_t l, size_t inline_len)
  __attribute__((always_inline));

void *maybe_inline_memcpy(void *dest, const void *src, size_t l, size_t inline_len)
{
  if (l > inline_len) {
    return memcpy(dest, src, l);
  }
  switch (l) {
  case 8:
    return __builtin_memcpy(dest, src, 8);
  case 4:
    return __builtin_memcpy(dest, src, 4);
  case 3:
    return __builtin_memcpy(dest, src, 3);
  case 2:
    return __builtin_memcpy(dest, src, 2);
  case 1:
    return __builtin_memcpy(dest, src, 1);
  default:
    int cursor = 0;
    while (l >= sizeof(uint64_t)) {
      __builtin_memcpy((char*)dest + cursor, (char*)src + cursor, sizeof(uint64_t));
      cursor += sizeof(uint64_t);
      l -= sizeof(uint64_t);
    }
    while (l >= sizeof(uint32_t)) {
      __builtin_memcpy((char*)dest + cursor, (char*)src + cursor, sizeof(uint32_t));
      cursor += sizeof(uint32_t);
      l -= sizeof(uint32_t);
    }
    while (l > 0) {
      *((char*)dest + cursor) = *((char*)src + cursor);
      cursor++;
      l--;
    }
  }
  return dest;
}

#else

#define maybe_inline_memcpy(d, s, l, x) memcpy(d, s, l)

#endif

#if defined(__GNUC__) && defined(__x86_64__)

static inline bool
mem_is_zero(const char *data, size_t len) __attribute__((always_inline));

bool mem_is_zero(const char *data, size_t len)
{
  // we do have XMM registers in x86-64, so if we need to check at least
  // 16 bytes, make use of them
  if (len / sizeof(uint128_t) > 0) {
    // align data pointer to 16 bytes, otherwise it'll segfault due to bug
    // in (at least some) GCC versions (using MOVAPS instead of MOVUPS).
    // check up to 15 first bytes while at it.
    while (((unsigned long long)data) & 15) {
      if (*(uint8_t*)data != 0) {
        return false;
      }
      data += sizeof(uint8_t);
      --len;
    }

    const char* data_start = data;
    const char* max128 = data + (len / sizeof(uint128_t))*sizeof(uint128_t);

    while (data < max128) {
      if (*(uint128_t*)data != 0) {
        return false;
      }
      data += sizeof(uint128_t);
    }
    len -= (data - data_start);
  }

  const char* max = data + len;
  const char* max32 = data + (len / sizeof(uint32_t))*sizeof(uint32_t);
  while (data < max32) {
    if (*(uint32_t*)data != 0) {
      return false;
    }
    data += sizeof(uint32_t);
  }
  while (data < max) {
    if (*(uint8_t*)data != 0) {
      return false;
    }
    data += sizeof(uint8_t);
  }
  return true;
}

#else

static inline bool mem_is_zero(const char *data, size_t len) {
  const char *end = data + len;
  const char* end64 = data + (len / sizeof(uint64_t))*sizeof(uint64_t);

  while (data < end64) {
    if (*(uint64_t*)data != 0) {
      return false;
    }
    data += sizeof(uint64_t);
  }

  while (data < end) {
    if (*data != 0) {
      return false;
    }
    ++data;
  }
  return true;
}

#endif

#endif // INLINE_MEMORY_H
