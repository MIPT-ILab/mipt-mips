/**
 * types.h - Implementation of types that are independent of host system
 * The types are used in algorithms affected by size of a variable
 *
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012-2019 MIPT-MIPS project
 */

#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <climits>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>

template<typename T> constexpr bool is_wide_integer_v = false;
template<typename T> constexpr bool is_wide_signed_integer_v = false;

template<typename T>
concept Integer = std::numeric_limits<T>::is_integer || is_wide_integer_v<T>;

template<typename T>
concept Unsigned = Integer<T> && !std::numeric_limits<T>::is_signed && !is_wide_signed_integer_v<T>;

template<typename T>
concept Signed = Integer<T> && !Unsigned<T>;

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

/* Bit widths of integer types */
template<Integer T> static constexpr size_t bitwidth = std::numeric_limits<T>::digits + std::numeric_limits<T>::is_signed;
template<Integer T> static constexpr size_t bytewidth = bitwidth<T> / CHAR_BIT;
template<Integer T> static constexpr size_t half_bitwidth = bitwidth<T> >> 1;

template<size_t N> struct unsigned_integer;
template<> struct unsigned_integer<64> { using type = uint64; };
template<> struct unsigned_integer<32> { using type = uint32; };
template<> struct unsigned_integer<16> { using type = uint16; };
template<> struct unsigned_integer<8>  { using type = uint8; };
template<size_t N> using unsigned_integer_t = typename unsigned_integer<N>::type;

template<size_t N> struct signed_integer;
template<> struct signed_integer<64> { using type = int64; };
template<> struct signed_integer<32> { using type = int32; };
template<> struct signed_integer<16> { using type = int16; };
template<> struct signed_integer<8>  { using type = int8; };
template<size_t N> using signed_integer_t = typename signed_integer<N>::type;

/* Convert signed to unsigned and vice versa */
template<Integer T> using unsign_t = unsigned_integer_t<bitwidth<T>>;
template<Integer T> using sign_t   = signed_integer_t<bitwidth<T>>;

/* Convert type to 2x type */
template<Unsigned T>           using doubled_t = unsigned_integer_t<bitwidth<T> * 2>;
template<size_t N, Unsigned T> using packed_t  = unsigned_integer_t<bitwidth<T> * N>;

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

template <Unsigned To>
static constexpr To narrow_cast(const Integer auto& value)
{
    return static_cast<To>( value);
}

template <Signed To>
static constexpr To sign_cast(const Unsigned auto& value)
{
    return static_cast<To>( value);
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
