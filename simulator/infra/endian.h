/**
 * endian.h - functions to deal with little-endian and big-endian
 *
 * @author Pavel Kryukov <pavel.kryukov@phystech.edu>
 * Copyright 2018-2019 MIPT-MIPS project
 */
 
#ifndef ENDIAN_H
#define ENDIAN_H

#include "byte.h"
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
    T value{};
    for ( size_t i = 0; i < array.size(); ++i) // NOLINTNEXTLINE
        value |= unsign_t<T>( uint8( array[i])) << (i * CHAR_BIT);

    return value;
}

template<typename T>
static inline constexpr T pack_array_be( std::array<Byte, bytewidth<T>> array) noexcept
{
    T value{};
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
    if constexpr (e == Endian::little)
        return unpack_array_le<T>( value);
    else
        return unpack_array_be<T>( value);
}

template<typename T, Endian e>
static inline constexpr auto pack_array( std::array<Byte, bytewidth<T>> array) noexcept
{
    if constexpr (e == Endian::little)
        return pack_array_le<T>( array);
    else
        return pack_array_be<T>( array);
}

template<typename T>
static inline constexpr T swap_endian( T value) noexcept
{
    return pack_array_le<T>( unpack_array_be<T>( value)); 
}

template<typename T, Endian e>
static inline void constexpr put_value_to_pointer( Byte* buf, T value, size_t size) {
    auto array = unpack_array<T, e>(value);
    for ( size_t i = 0; i < size; ++i) // NOLINTNEXTLINE
        *(buf + i) = array[i];
}

template<typename T, Endian e>
static inline constexpr T get_value_from_pointer( const Byte* buf, size_t size) {
    std::array<Byte, bytewidth<T>> array{};
    for ( size_t i = 0; i < size; ++i) // NOLINTNEXTLINE
        array[i] = *( buf + i);

    return pack_array<T, e>( array);
}

#endif // ENDIAN_H
