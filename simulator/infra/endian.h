/**
 * endian.h - functions to deal with little-endian and big-endian
 *
 * @author Pavel Kryukov <pavel.kryukov@phystech.edu>
 * Copyright 2018 MIPT-MIPS project
 */
 
// protection from multi-include
#ifndef ENDIAN_H
#define ENDIAN_H

#include "macro.h"
#include <array>
#include <climits>

// Replace it with std::endian if C++20 is shipped
enum class Endian
{
#ifdef _WIN32
    little = 0,
    big    = 1,
    native = little
#else
    little = __ORDER_LITTLE_ENDIAN__,
    big    = __ORDER_BIG_ENDIAN__,
    native = __BYTE_ORDER__
#endif
};

template<typename T>
static inline constexpr T pack_array_le( std::array<Byte, bytewidth<T>> array) noexcept
{
    T value = 0;
    for ( size_t i = 0; i < array.size(); ++i) // NOLINTNEXTLINE
        value |= unsign_t<T>( uint8( array[i])) << (i * CHAR_BIT);

    return value;
}

template<typename T>
static inline constexpr T pack_array_be( std::array<Byte, bytewidth<T>> array) noexcept
{
    T value = 0;
    for ( size_t i = 0; i < array.size(); ++i) // NOLINTNEXTLINE
        value |= unsign_t<T>( uint8( array[i])) << ((array.size() - i - 1) * CHAR_BIT);

    return value;
}

template<typename T>
static inline constexpr auto unpack_array_le( T value) noexcept
{
    std::array<Byte, bytewidth<T>> array{};
    for ( size_t i = 0; i < array.size(); ++i) // NOLINTNEXTLINE
        array[i] = Byte( uint8( value >> ( i * CHAR_BIT)));

    return array;
}

template<typename T>
static inline constexpr auto unpack_array_be( T value) noexcept
{
    std::array<Byte, bytewidth<T>> array{};
    for ( size_t i = 0; i < array.size(); ++i) // NOLINTNEXTLINE
        array[i] = Byte( uint8( value >> ((array.size() - i - 1) * CHAR_BIT)));

    return array;
}

template<typename T, Endian e>
static constexpr inline auto unpack_array( T value) noexcept
{
    if constexpr (e == Endian::little) // NOLINTNEXTLINE(bugprone-suspicious-semicolon) llvm bug 35824
        return unpack_array_le<T>( value);

    return unpack_array_be<T>( value);
}

template<typename T, Endian e>
static inline constexpr auto pack_array( std::array<Byte, bytewidth<T>> array) noexcept
{
    if constexpr (e == Endian::little) // NOLINTNEXTLINE(bugprone-suspicious-semicolon) llvm bug 35824
        return pack_array_le<T>( array);

    return pack_array_be<T>( array);
}

template<typename T>
static inline constexpr T swap_endian( T value) noexcept
{
    return pack_array_le<T>( unpack_array_be<T>( value)); 
}

#endif // ENDIAN_H
