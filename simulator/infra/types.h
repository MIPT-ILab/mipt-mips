/**
 * types.h - Implementation of types that are independent of host system
 * The types are used in algorithms affected by size of a variable
 *
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012-2019 MIPT-MIPS project
 */

#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <cstddef>
#include <cstdint>

template <typename To, typename From>
static constexpr To narrow_cast(const From& value)
{
    return static_cast<To>( value);
}

// Signed types
using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;

// Unsigned types
using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

// Float types
using float32 = float;
using float64 = double;

/* Convert signed type to unsigned type */
template<typename> struct unsign;
template<> struct unsign<int8>    { using type = uint8; };
template<> struct unsign<int16>   { using type = uint16; };
template<> struct unsign<int32>   { using type = uint32; };
template<> struct unsign<int64>   { using type = uint64; };
template<> struct unsign<uint8>   { using type = uint8; };
template<> struct unsign<uint16>  { using type = uint16; };
template<> struct unsign<uint32>  { using type = uint32; };
template<> struct unsign<uint64>  { using type = uint64; };

template<typename T> using unsign_t = typename unsign<T>::type;

/* Convert unsigned type to signed type */
template<typename> struct sign;
template<> struct sign<uint8>   { using type = int8; };
template<> struct sign<uint16>  { using type = int16; };
template<> struct sign<uint32>  { using type = int32; };
template<> struct sign<uint64>  { using type = int64; };

template<typename T> using sign_t = typename sign<T>::type;

/* Convert type to 2x type */
template<typename> struct doubled;
template<> struct doubled<uint8>   { using type = uint16; };
template<> struct doubled<uint16>  { using type = uint32; };
template<> struct doubled<uint32>  { using type = uint64; };

template<> struct doubled<int8>    { using type = int16;  };
template<> struct doubled<int16>   { using type = int32;  };
template<> struct doubled<int32>   { using type = int64;  };

template<typename T> using doubled_t = typename doubled<T>::type;

template<size_t N, typename T> struct packed       { using type = doubled_t<typename packed<N / 2, T>::type>; };
template<typename T>           struct packed<1, T> { using type = T; };
template<size_t N, typename T> using packed_t = typename packed<N, T>::type;

/* Convert type to 2x smaller type */
template<typename> struct halved;
template<> struct halved<uint16>  { using type = uint8; };
template<> struct halved<uint32>  { using type = uint16; };
template<> struct halved<uint64>  { using type = uint32; };

template<typename T> using halved_t = typename halved<T>::type;

// Byte casts
static inline std::byte* byte_cast( char* b)
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast) Casting byte to byte is correct
    return reinterpret_cast<std::byte*>( b);
}

static inline const std::byte* byte_cast( const char* b)
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast) Casting byte to byte is correct
    return reinterpret_cast<const std::byte*>( b);
}

static inline std::byte* byte_cast( uint8* b)
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast) Casting byte to byte is correct
    return reinterpret_cast<std::byte*>( b);
}

static inline const std::byte* byte_cast( const uint8* b)
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast) Casting byte to byte is correct
    return reinterpret_cast<const std::byte*>( b);
}

using Addr = uint64;
using AddrDiff = int64;

// The following consts are used to initialize some variables
// as something that has invalid value.
// Using such values most likely will leads to segmenation fault,
// which is easy to debug than access by an uninitialize pointer
// or array index.
static const uint8  NO_VAL8  = 0xBA;
static const uint16 NO_VAL16 = 0xf00d;
static const uint32 NO_VAL32 = 0xdead'beefUL;
static const uint64 NO_VAL64 = 0xfeed'face'cafe'beafULL;

static const uint8  MAX_VAL8  = UINT8_MAX;
static const uint16 MAX_VAL16 = UINT16_MAX;
static const uint32 MAX_VAL32 = UINT32_MAX;
static const uint64 MAX_VAL64 = UINT64_MAX;

#endif // #ifndef COMMON_TYPES_H
