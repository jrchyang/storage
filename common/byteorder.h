#ifndef BYTEORDER_H
#define BYTEORDER_H

#include <linux/types.h>
#include <boost/endian/conversion.hpp>

template<typename T>
inline T swab(T val) {
  return boost::endian::endian_reverse(val);
}

template<typename T>
struct common_le {
private:
  T v;
public:
  common_le() = default;
  explicit common_le(T nv)
    : v{boost::endian::native_to_little(nv)}
  {}
  common_le<T>& operator=(T nv) {
    v = boost::endian::native_to_little(nv);
    return *this;
  }
  operator T() const { return boost::endian::little_to_native(v); }
  friend inline bool operator==(common_le a, common_le b) {
    return a.v == b.v;
  }
} __attribute__ ((packed));

using common_le64 = common_le<__u64>;
using common_le32 = common_le<__u32>;
using common_le16 = common_le<__u16>;

using common_les64 = common_le<__s64>;
using common_les32 = common_le<__s32>;
using common_les16 = common_le<__s16>;

inline common_les64 init_les64(__s64 x) {
  common_les64 v;
  v = x;
  return v;
}
inline common_les32 init_les32(__s32 x) {
  common_les32 v;
  v = x;
  return v;
}
inline common_les16 init_les16(__s16 x) {
  common_les16 v;
  v = x;
  return v;
}

#endif // BYTEORDER_H
