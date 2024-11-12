#ifndef INTARITH_H
#define INTARITH_H

#include <type_traits>

template<typename T, typename U>
constexpr inline std::make_unsigned_t<std::common_type_t<T, U>> div_round_up(T n, U d) {
  return (n + d - 1) / d;
}

template<typename T, typename U>
constexpr inline std::make_unsigned_t<std::common_type_t<T, U>> round_up_to(T n, U d) {
  return (n % d ? (n + d - n % d) : n);
}

template<typename T, typename U>
constexpr inline std::make_unsigned_t<std::common_type_t<T, U>> shift_round_up(T x, U y) {
  return (x + (1 << y) - 1) >> y;
}

/*
 * Wrapper to determine if value is a power of 2
 */
template<typename T>
constexpr inline bool isp2(T x) {
  return (x & (x - 1)) == 0;
}

/*
 * Wrappers for various sorts of alignment and rounding.  The "align" must
 * be a power of 2.  Often times it is a block, sector, or page.
 */

/*
 * return x rounded down to an align boundary
 * eg, p2align(1200, 1024) == 1024 (1*align)
 * eg, p2align(1024, 1024) == 1024 (1*align)
 * eg, p2align(0x1234, 0x100) == 0x1200 (0x12*align)
 * eg, p2align(0x5600, 0x100) == 0x5600 (0x56*align)
 *
 * -align 表示 align 的补码，即 ~align+1 ，如果 align 是 2 的幂，则等于 ~(align-1)
 */
template<typename T>
constexpr inline T p2align(T x, T align) {
  return x & -align;
}

/*
 * return x % (mod) align
 * eg, p2phase(0x1234, 0x100) == 0x34 (x-0x12*align)
 * eg, p2phase(0x5600, 0x100) == 0x00 (x-0x56*align)
 */
template<typename T>
constexpr inline T p2phase(T x, T align) {
  return x & (align - 1);
}

/*
 * return how much space is left in this block (but if it's perfectly
 * aligned, return 0).
 * eg, p2nphase(0x1234, 0x100) == 0xcc (0x13*align-x)
 * eg, p2nphase(0x5600, 0x100) == 0x00 (0x56*align-x)
 */
template<typename T>
constexpr inline T p2nphase(T x, T align) {
  return -x & (align - 1);
}

/*
 * return x rounded up to an align boundary
 * eg, p2roundup(0x1234, 0x100) == 0x1300 (0x13*align)
 * eg, p2roundup(0x5600, 0x100) == 0x5600 (0x56*align)
 */
template<typename T>
constexpr inline T p2roundup(T x, T align) {
  return -(-x & -align);
}

// count trailing zeros.
// NOTE: the builtin is nondeterministic on 0 input
template<class T>
inline typename std::enable_if<
    (std::is_integral<T>::value && sizeof(T) <= sizeof(unsigned)),
     unsigned>::type ctz(T v) {
  if (v == 0)
    return sizeof(v) * 8;
  return __builtin_ctz(v);
}

template<class T>
inline typename std::enable_if<
    (std::is_integral<T>::value && sizeof(T) > sizeof(unsigned int) &&
     sizeof(T) <= sizeof(unsigned long)), unsigned>::type ctz(T v) {
  if (v == 0)
    return sizeof(v) * 8;
  return __builtin_ctzl(v);
}

template<class T>
inline typename std::enable_if<
    (std::is_integral<T>::value && sizeof(T) > sizeof(unsigned long) &&
     sizeof(T) <= sizeof(unsigned long long)), unsigned>::type ctz(T v) {
  if (v == 0)
    return sizeof(v) * 8;
  return __builtin_ctzll(v);
}

// count leading zeros
// NOTE: the builtin is nondeterministic on 0 input
template<class T>
inline typename std::enable_if<
    (std::is_integral<T>::value && sizeof(T) <= sizeof(unsigned)),
     unsigned>::type clz(T v) {
  if (v == 0)
    return sizeof(v) * 8;
  return __builtin_clz(v);
}

template<class T>
inline typename std::enable_if<
    (std::is_integral<T>::value && sizeof(T) > sizeof(unsigned int) &&
     sizeof(T) <= sizeof(unsigned long)), unsigned>::type clz(T v) {
  if (v == 0)
    return sizeof(v) * 8;
  return __builtin_clzl(v);
}

template<class T>
inline typename std::enable_if<
    (std::is_integral<T>::value && sizeof(T) > sizeof(unsigned long) &&
     sizeof(T) <= sizeof(unsigned long long)), unsigned>::type clz(T v) {
  if (v == 0)
    return sizeof(v) * 8;
  return __builtin_clzll(v);
}

// count bits (set + any 0's that follow)
template<class T>
inline typename std::enable_if<
    (std::is_integral<T>::value && sizeof(T) <= sizeof(unsigned)),
     unsigned>::type cbits(T v) {
  if (v == 0)
    return 0;
  return (sizeof(v) * 8) - __builtin_clz(v);
}

template<class T>
inline typename std::enable_if<
    (std::is_integral<T>::value && sizeof(T) > sizeof(unsigned int) &&
     sizeof(T) <= sizeof(unsigned long)), unsigned>::type cbits(T v) {
  if (v == 0)
    return 0;
  return (sizeof(v) * 8) - __builtin_clzl(v);
}

template<class T>
inline typename std::enable_if<
    (std::is_integral<T>::value && sizeof(T) > sizeof(unsigned long) &&
     sizeof(T) <= sizeof(unsigned long long)), unsigned>::type cbits(T v) {
  if (v == 0)
    return 0;
  return (sizeof(v) * 8) - __builtin_clzll(v);
}

// count the bits set to 1, a.k.a. population count
template<class T>
unsigned common_popcount(T v) {
  static_assert(sizeof(T) <= sizeof(unsigned long long), "type too large");
  if constexpr (sizeof(T) <= sizeof(unsigned int)) {
    return __builtin_popcount(v);
  } else if constexpr (sizeof(T) <= sizeof(unsigned long)) {
    return __builtin_popcountl(v);
  } else {
    return __builtin_popcountll(v);
  }
}

#endif // INTARITH_H
