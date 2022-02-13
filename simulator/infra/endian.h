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

template<size_t N>
constexpr auto pack_array_le( std::array<std::byte, N> array) noexcept
{
    using T = unsigned_integer_t<N * CHAR_BIT>;
    T value{};
    size_t shift = 0;
    for ( const auto& el : array) {
        // Shift result has to be casted since it is 'int' by default for narrow types
        value |= T( T( uint8( el)) << shift);
        shift += CHAR_BIT;
    }

    return value;
}

template<size_t N>
constexpr auto pack_array_be( std::array<std::byte, N> array) noexcept
{
    using T = unsigned_integer_t<N * CHAR_BIT>;
    T value{};
    size_t shift = (array.size() - 1) * CHAR_BIT;
    for ( const auto& el : array) {
        // Shift result has to be casted since it is 'int' by default for narrow types
        value |= T( T( uint8( el)) << shift);
        shift -= CHAR_BIT;
    }

    return value;
}

constexpr auto unpack_array_le( Unsigned auto value) noexcept
{
    std::array<std::byte, bytewidth<decltype(value)>> array{};
    size_t shift = 0;
    for ( auto& el: array) {
        el = std::byte( uint8( value >> shift));
        shift += CHAR_BIT;
    }

    return array;
}

constexpr auto unpack_array_be( Unsigned auto value) noexcept
{
    std::array<std::byte, bytewidth<decltype(value)>> array{};
    size_t shift = (array.size() - 1) * CHAR_BIT;
    for ( auto& el: array) {
        el = std::byte( uint8( value >> shift));
        shift -= CHAR_BIT;
    }

    return array;
}

template<std::endian e>
constexpr auto unpack_array( Unsigned auto value) noexcept
{
    if constexpr (e == std::endian::little)
        return unpack_array_le( value);
    else
        return unpack_array_be( value);
}

template<std::endian e, std::size_t N>
constexpr auto pack_array( std::array<std::byte, N> array) noexcept
{
    if constexpr (e == std::endian::little)
        return pack_array_le( array);
    else
        return pack_array_be( array);
}

constexpr auto swap_endian( Unsigned auto value) noexcept
{
    return pack_array_le( unpack_array_be( value)); 
}

template<Unsigned T, std::endian e>
constexpr void put_value_to_pointer( std::byte* buf, T value, size_t size)
{
    auto array = unpack_array<e>(value);
    std::copy( array.begin(), array.begin() + size, buf);
}

template<Unsigned T, std::endian e>
static inline constexpr T get_value_from_pointer( const std::byte* buf, size_t size)
{
    std::array<std::byte, bytewidth<T>> array{};
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    std::copy( buf, buf + size, array.begin());
    return pack_array<e>( array);
}

#endif // ENDIAN_H
