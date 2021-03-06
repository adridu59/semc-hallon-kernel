/*
Copyright 2011 Google Inc. All Rights Reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

    * Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
copyright notice, this list of conditions and the following disclaimer
in the documentation and/or other materials provided with the
distribution.
    * Neither the name of Google Inc. nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Various stubs for the open-source version of Snappy.

File modified by
Zeev Tarantov <zeev.tarantov@gmail.com>
*/

#ifndef CSNAPPY_INTERNAL_USERSPACE_H_
#define CSNAPPY_INTERNAL_USERSPACE_H_

#if defined(_MSC_VER) && (_MSC_VER <= 1300)
typedef unsigned __int8  uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
#else
#include <stdint.h>
#endif

#ifdef _GNU_SOURCE
#define min(x, y) (__extension__ ({		\
	typeof(x) _min1 = (x);			\
	typeof(y) _min2 = (y);			\
	(void) (&_min1 == &_min2);		\
	_min1 < _min2 ? _min1 : _min2; }))
#else
#define min(x, y) (((x) < (y)) ? (x) : (y))
#endif

/* Static prediction hints. */
#define likely(x)	__builtin_expect(!!(x), 1)
#define unlikely(x)	__builtin_expect(!!(x), 0)


#ifdef DEBUG
#include <assert.h>
#define DCHECK(cond)	assert(cond)
#else
#define DCHECK(cond)
#endif

/*
Uses code from http://code.google.com/p/exfat/source/browse/trunk/libexfat/byteorder.h
with 3-clause BSD license instead of GPL, with permission from:
Andrew Nayenko
Albert Lee
*/
#if defined(_MSC_VER)

#include <stdlib.h>
#define bswap_16(x) _byteswap_ushort(x)
#define bswap_32(x) _byteswap_ulong(x)
#define bswap_64(x) _byteswap_uint64(x)

#elif defined(__GLIBC__) || defined(__ANDROID__)

#include <endian.h>
#include <byteswap.h>

#elif defined(__APPLE__)

#include <machine/endian.h>
#include <libkern/OSByteOrder.h>
#define bswap_16(x) OSSwapInt16(x)
#define bswap_32(x) OSSwapInt32(x)
#define bswap_64(x) OSSwapInt64(x)
#define __BYTE_ORDER BYTE_ORDER
#define __LITTLE_ENDIAN LITTLE_ENDIAN
#define __BIG_ENDIAN BIG_ENDIAN

#elif defined(__FreeBSD__) || defined(__DragonFlyBSD__) || defined(__NetBSD__)

#include <sys/endian.h>
#define bswap_16(x) bswap16(x)
#define bswap_32(x) bswap32(x)
#define bswap_64(x) bswap64(x)
#define __BYTE_ORDER _BYTE_ORDER
#define __LITTLE_ENDIAN _LITTLE_ENDIAN
#define __BIG_ENDIAN _BIG_ENDIAN

#elif defined(__OpenBSD__)

#include <machine/endian.h>
#define bswap_16(x) swap16(x)
#define bswap_32(x) swap32(x)
#define bswap_64(x) swap64(x)
#define __BYTE_ORDER _BYTE_ORDER
#define __LITTLE_ENDIAN _LITTLE_ENDIAN
#define __BIG_ENDIAN _BIG_ENDIAN

#elif defined(__sun)

#include <sys/byteorder.h>
#define bswap_16(x) BSWAP_16(x)
#define bswap_32(x) BSWAP_32(x)
#define bswap_64(x) BSWAP_64(x)
#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN 4321
#ifdef _LITTLE_ENDIAN
#define __BYTE_ORDER __LITTLE_ENDIAN
#else
#define __BYTE_ORDER __BIG_ENDIAN
#endif

#else 
#error No byte order macros available for your platform
#endif


/* Potentially unaligned loads and stores. */

#if defined(__i386__) || defined(__x86_64__) || defined(__powerpc__)

#define UNALIGNED_LOAD16(_p) (*(const uint16_t*)(_p))
#define UNALIGNED_LOAD32(_p) (*(const uint32_t*)(_p))
#define UNALIGNED_LOAD64(_p) (*(const uint64_t*)(_p))

#define UNALIGNED_STORE16(_p, _val) (*(uint16_t*)(_p) = (_val))
#define UNALIGNED_STORE32(_p, _val) (*(uint32_t*)(_p) = (_val))
#define UNALIGNED_STORE64(_p, _val) (*(uint64_t*)(_p) = (_val))

#elif defined(__arm__) && \
	!defined(__ARM_ARCH_4__) && \
	!defined(__ARM_ARCH_4T__) && /* http://wiki.debian.org/ArmEabiPort#Choice_of_minimum_CPU */ \
	!defined(__MARM_ARMV4__) && \
	!defined(_ARMV4I_) && \
	!defined(__ARM_ARCH_5__) && \
	!defined(__ARM_ARCH_5T__) && \
	!defined(__ARM_ARCH_5E__) && \
	!defined(__ARM_ARCH_5TE__) && \
	!defined(__ARM_ARCH_5TEJ__) && \
	!defined(__MARM_ARMV5__) && \
	!defined(__ARM_ARCH_6__) && /* ARMv6 fakes unaligned access */ \
	!defined(__ARM_ARCH_6J__) && \
	!defined(__ARM_ARCH_6K__) && \
	!defined(__ARM_ARCH_6Z__) && \
	!defined(__ARM_ARCH_6ZK__) && \
	!defined(__ARM_ARCH_6T2__) && \
	!defined(__ARMV6__)

#define UNALIGNED_LOAD16(_p) (*(const uint16_t*)(_p))
#define UNALIGNED_LOAD32(_p) (*(const uint32_t*)(_p))
#define UNALIGNED_STORE16(_p, _val) (*(uint16_t*)(_p) = (_val))
#define UNALIGNED_STORE32(_p, _val) (*(uint32_t*)(_p) = (_val))

#pragma pack(1)
struct una_u64 { uint64_t x; };
#pragma pack()

static inline uint64_t UNALIGNED_LOAD64(const void *p)
{
	const struct una_u64 *ptr = (const struct una_u64 *)p;
	return ptr->x;
}

static inline void UNALIGNED_STORE64(void *p, uint64_t v)
{
	struct una_u64 *ptr = (struct una_u64 *)p;
	ptr->x = v;
}

#else /* !(x86 || powerpc) && !(arm && !(old arm architectures)) */

#pragma pack(1)
struct una_u16 { uint16_t x; };
struct una_u32 { uint32_t x; };
struct una_u64 { uint64_t x; };
#pragma pack()

static inline uint16_t UNALIGNED_LOAD16(const void *p)
{
	const struct una_u16 *ptr = (const struct una_u16 *)p;
	return ptr->x;
}

static inline uint32_t UNALIGNED_LOAD32(const void *p)
{
	const struct una_u32 *ptr = (const struct una_u32 *)p;
	return ptr->x;
}

static inline uint64_t UNALIGNED_LOAD64(const void *p)
{
	const struct una_u64 *ptr = (const struct una_u64 *)p;
	return ptr->x;
}

static inline void UNALIGNED_STORE16(void *p, uint16_t v)
{
	struct una_u16 *ptr = (struct una_u16 *)p;
	ptr->x = v;
}

static inline void UNALIGNED_STORE32(void *p, uint32_t v)
{
	struct una_u32 *ptr = (struct una_u32 *)p;
	ptr->x = v;
}

static inline void UNALIGNED_STORE64(void *p, uint64_t v)
{
	struct una_u64 *ptr = (struct una_u64 *)p;
	ptr->x = v;
}

#endif /* !(x86 || powerpc) && !(arm && !armv5 && !armv6) */


#if __BYTE_ORDER == __LITTLE_ENDIAN
#define get_unaligned_le32(p)           UNALIGNED_LOAD32(p)
#define put_unaligned_le16(v, p)        UNALIGNED_STORE16(p, v)
#elif __BYTE_ORDER == __BIG_ENDIAN
static inline uint32_t get_unaligned_le32(const void *p)
{
  return bswap_32(UNALIGNED_LOAD32(p));
}
static inline void put_unaligned_le16(uint16_t val, void *p)
{
  UNALIGNED_STORE16(p, bswap_16(val));
}
#else
#error __BYTE_ORDER must be either __LITTLE_ENDIAN or __BIG_ENDIAN
#endif


#if defined(HAVE_BUILTIN_CTZ)

static inline int FindLSBSetNonZero(uint32_t n)
{
	return __builtin_ctz(n);
}

static inline int FindLSBSetNonZero64(uint64_t n)
{
	return __builtin_ctzll(n);
}

#else /* Portable versions. */

static inline int FindLSBSetNonZero(uint32_t n)
{
	int rc = 31, i, shift;
	uint32_t x;
	for (i = 4, shift = 1 << 4; i >= 0; --i) {
		x = n << shift;
		if (x != 0) {
			n = x;
			rc -= shift;
		}
		shift >>= 1;
	}
	return rc;
}

/* FindLSBSetNonZero64() is defined in terms of FindLSBSetNonZero(). */
static inline int FindLSBSetNonZero64(uint64_t n)
{
	const uint32_t bottombits = (uint32_t)n;
	if (bottombits == 0) {
		/* Bottom bits are zero, so scan in top bits */
		return 32 + FindLSBSetNonZero((uint32_t)(n >> 32));
	} else {
		return FindLSBSetNonZero(bottombits);
	}
}

#endif /* End portable versions. */

#endif  /* CSNAPPY_INTERNAL_USERSPACE_H_ */
