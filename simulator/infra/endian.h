/**
 * endian.h - functions to deal with little-endian and big-endian
 *
 * @author Pavel Kryukov <pavel.kryukov@phystech.edu>
 * Copyright 2018-2019 MIPT-MIPS project
 */
 
#ifndef ENDIAN_H
#define ENDIAN_H

#include "macro.h"
#include "types.h"

#include <array>
#include <bit>
#include <climits>

// GCC 9 poorly optimizes that stuff: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=65424
// See https://godbolt.org/z/ff-NAF for example

template<typename T>
static inline constexpr
std::enable_if_t<std::is_same_v<T, unsign_t<T>>, T>
pack_array_le( std::array<std::byte, bytewidth<T>> array) noexcept
{
    T value{};
    size_t shift = 0;
    for ( const auto& el : array) {
        // Shift result has to be casted since it is 'int' by default for narrow types
        value |= T( T( uint8( el)) << shift);
        shift += CHAR_BIT;
    }

    return value;
}

template<typename T>
static inline constexpr
std::enable_if_t<std::is_same_v<T, unsign_t<T>>, T>
pack_array_be( std::array<std::byte, bytewidth<T>> array) noexcept
{
    T value{};
    size_t shift = (array.size() - 1) * CHAR_BIT;
    for ( const auto& el : array) {
        // Shift result has to be casted since it is 'int' by default for narrow types
        value |= T( T( uint8( el)) << shift);
        shift -= CHAR_BIT;
    }

    return value;
}

template<typename T>
static inline constexpr auto unpack_array_le( T value) noexcept
{
    std::array<std::byte, bytewidth<T>> array{};
    size_t shift = 0;
    for ( auto& el: array) {
        el = std::byte( uint8( value >> shift));
        shift += CHAR_BIT;
    }

    return array;
}

template<typename T>
static inline constexpr auto unpack_array_be( T value) noexcept
{
    std::array<std::byte, bytewidth<T>> array{};
    size_t shift = (array.size() - 1) * CHAR_BIT;
    for ( auto& el: array) {
        el = std::byte( uint8( value >> shift));
        shift -= CHAR_BIT;
    }

    return array;
}

template<typename T, std::endian e>
static constexpr inline auto unpack_array( T value) noexcept
{
    if constexpr (e == std::endian::little)
        return unpack_array_le<T>( value);
    else
        return unpack_array_be<T>( value);
}

template<typename T, std::endian e>
static inline constexpr auto pack_array( std::array<std::byte, bytewidth<T>> array) noexcept
{
    if constexpr (e == std::endian::little)
        return pack_array_le<T>( array);
    else
        return pack_array_be<T>( array);
}

template<typename T>
static inline constexpr T swap_endian( T value) noexcept
{
    return pack_array_le<T>( unpack_array_be<T>( value)); 
}

template<typename T, std::endian e>
static inline void constexpr put_value_to_pointer( std::byte* buf, T value, size_t size)
{
    auto array = unpack_array<T, e>(value);
    std::copy( array.begin(), array.begin() + size, buf);
}

template<typename T, std::endian e>
static inline constexpr T get_value_from_pointer( const std::byte* buf, size_t size)
{
    std::array<std::byte, bytewidth<T>> array{};
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    std::copy( buf, buf + size, array.begin());
    return pack_array<T, e>( array);
}

#endif // ENDIAN_H
