#ifndef DATATYPES_H
#define DATATYPES_H
#pragma once

typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
typedef signed int int32_t;
typedef unsigned int uint32_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed int ssize_t;

#ifndef INT8_MAX
# define INT8_MAX 127
#endif
#ifndef INT8_MIN
# define INT8_MIN -128
#endif
#ifndef UINT8_MAX
# define UINT8_MAX 255
#endif
#ifndef INT16_MAX
# define INT16_MAX 32767
#endif
#ifndef INT16_MIN
# define INT16_MIN -32768
#endif
#ifndef UINT16_MAX
# define UINT16_MAX 65535
#endif
#ifndef INT32_MAX
# define INT32_MAX 2147483647
#endif
#ifndef INT32_MIN
/* MSC treats -2147483648 as -(2147483648U). */
# define INT32_MIN (-INT32_MAX - 1)
#endif
#ifndef UINT32_MAX
# define UINT32_MAX 4294967295U
#endif

#ifndef PRId64
# define PRId64 "I64d"
#endif
#ifndef PRIi64
# define PRIi64 "I64i"
#endif
#ifndef PRIo64
# define PRIo64 "I64o"
#endif
#ifndef PRIu64
# define PRIu64 "I64u"
#endif
#ifndef PRIx64
# define PRIx64 "I64x"
#endif
#ifndef PRIX64
# define PRIX64 "I64X"
#endif

inline int64_t make_int64(uint32_t high, uint32_t low)
{
  int64_t v = (int64_t)high;
  v <<= 32;
  v  |= low;
  return v;
}

inline uint64_t make_uint64(uint32_t high, uint32_t low)
{
  uint64_t v = (uint64_t)high;
  v <<= 32;
  v  |= low;
  return v;
}
#endif