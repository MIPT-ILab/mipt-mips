/**
 * types.h - Implementation of types that are independent of host system
 * The types are used in algorithms affected by size of a variable
 *
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012-2018 MIPT-MIPS project
 */

// protection from multi-include
#ifndef COMMON__TYPES_H
#define COMMON__TYPES_H

// C++11 fixed width integer types
#include <cstdint>

// Do not use std::byte as Apple Xcode does not have it
enum class Byte : uint8_t { };

/* Signed 8-bit integer type */
using int8 = int8_t;
/* Signed 16-bit integer type */
using int16 = int16_t;
/* Signed 32-bit integer type */
using int32 = int32_t;
/* Signed 64-bit integer type */
using int64 = int64_t;
/* Unsigned 8-bit integer type */
using uint8 = uint8_t;
/* Unsigned 16-bit integer type */
using uint16 = uint16_t;
/* Unsigned 32-bit integer type */
using uint32 = uint32_t;
/* Unsigned 64-bit integer type */
using uint64 = uint64_t;
/* Single precision 32-bit float type */
using float32 = float;
/* Double precision 64-bit float type */
using float64 = double;

// Use native GCC type if available, as Boost <= 1.60 + GCC 7 generate a bug
#if defined(__GNUC__) && defined(__SIZEOF_INT128__)

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

/* Unsigned 128-bit integer type */
using uint128 = unsigned __int128;

/* Signed 128-bit integer type */
using int128 = __int128;

#pragma GCC diagnostic pop

#else // __SIZEOF_INT128__

#include <boost/multiprecision/cpp_int.hpp>

/* Unsigned 128-bit integer type */
using uint128 = boost::multiprecision::uint128_t;

/* Unsigned 128-bit integer type */
using int128 = boost::multiprecision::int128_t;

#endif // __SIZEOF_INT128__

/* Convert signed type to unsigned type */
template<typename> struct unsign;
template<> struct unsign<int8>    { using type = uint8; };
template<> struct unsign<int16>   { using type = uint16; };
template<> struct unsign<int32>   { using type = uint32; };
template<> struct unsign<int64>   { using type = uint64; };
template<> struct unsign<int128>  { using type = uint128; };
template<> struct unsign<uint8>   { using type = uint8; };
template<> struct unsign<uint16>  { using type = uint16; };
template<> struct unsign<uint32>  { using type = uint32; };
template<> struct unsign<uint64>  { using type = uint64; };
template<> struct unsign<uint128> { using type = uint128; };

template<typename T> using unsign_t = typename unsign<T>::type;

/* Convert unsigned type to signed type */
template<typename> struct sign;
template<> struct sign<uint8>   { using type = int8; };
template<> struct sign<uint16>  { using type = int16; };
template<> struct sign<uint32>  { using type = int32; };
template<> struct sign<uint64>  { using type = int64; };
template<> struct sign<uint128> { using type = int128; };

template<typename T> using sign_t = typename sign<T>::type;

/* Convert type to 2x type */
template<typename> struct doubled;
template<> struct doubled<uint8>  { using type = uint16; };
template<> struct doubled<uint16> { using type = uint32; };
template<> struct doubled<uint32> { using type = uint64; };
template<> struct doubled<uint64> { using type = uint128; };

template<> struct doubled<int8>   { using type = int16;  };
template<> struct doubled<int16>  { using type = int32;  };
template<> struct doubled<int32>  { using type = int64;  };
template<> struct doubled<int64>  { using type = int128;  };

template<typename T> using doubled_t = typename doubled<T>::type;

using Addr = uint64;

// The following consts are used to initialize some variables
// as something that has invalid value.
// Using such values most likely will leads to segmenation fault,
// which is easy to debug than access by an uninitialize pointer
// or array index.
static const uint8  NO_VAL8  = 0xBA;
static const uint16 NO_VAL16 = 0xf00d;
static const uint32 NO_VAL32 = 0xdead'beeful;
static const uint64 NO_VAL64 = 0xfeed'face'cafe'beafull;

static const uint8  MAX_VAL8  = UINT8_MAX;
static const uint16 MAX_VAL16 = UINT16_MAX;
static const uint32 MAX_VAL32 = UINT32_MAX;
static const uint64 MAX_VAL64 = UINT64_MAX;

#endif // #ifndef COMMON__TYPES_H
