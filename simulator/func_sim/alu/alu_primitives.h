/*
 * alu.h - implementation of some ALU primitives
 * Copyright 2014-2020 MIPT-MIPS
 */

#include <infra/macro.h>
#include <infra/types.h>
#include <infra/uint128.h>

#include <bit>

constexpr auto popcount( Unsigned auto x) noexcept              { return std::popcount(x); }
constexpr auto count_leading_zeroes( Unsigned auto x) noexcept  { return std::countl_zero( x); }
constexpr auto count_leading_ones( Unsigned auto x) noexcept    { return std::countl_one( x); }
constexpr auto count_trailing_zeroes( Unsigned auto x) noexcept { return std::countr_zero( x); }

template<Unsigned T> static
auto test_addition_overflow( Unsigned auto val1, Unsigned auto val2)
{
    const T result = narrow_cast<T>( val1) + narrow_cast<T>( val2);
    const bool is_overflow =
        ( signify( val1) > 0 && signify( val2) > 0 && signify( result) < 0) ||
        ( signify( val1) < 0 && signify( val2) < 0 && signify( result) > 0);

    return std::pair{ result, is_overflow};
}

template<Unsigned T> static
auto test_subtraction_overflow( Unsigned auto val1, Unsigned auto val2)
{
    const T result = narrow_cast<T>( val1) - narrow_cast<T>( val2);
    const bool is_overflow =
        ( signify( val1) > 0 && signify( val2) < 0 && signify( result) < 0) ||
        ( signify( val1) < 0 && signify( val2) > 0 && signify( result) > 0);

    return std::pair{ result, is_overflow};
}

/*
 * Returns an interleaved mask
 * 0 -> 0x5555'5555 (0101010101...)
 * 1 -> 0x3333'3333 (0011001100...)
 * 2 -> 0x0F0F'0F0F (0000111100...)
 */
template<Unsigned T>
static constexpr T interleaved_mask( size_t density)
{
    T result{};
    for ( size_t i = 0; i < bitwidth<T>; ++i)
        if ( ( i >> density) % 2 == 0)
            result |= lsb_set<T>() << i;

    return result;
}

/*
 * With shift = 2, returns a left unshuffling mask
 * 0 -> 0x4444'4444 (0010'0010'01...)
 * 1 -> 0x3030'3030 (0000'1100'00...)
 * 2 -> 0x0F00'0F00
 * With shift = 1, return a right unshuffling mask
 * 0 -> 0x2222'2222 (0100'0100'00...)
 * 1 -> 0x0C0C'0C0C (0011'0000'00...)
 * 2 -> 0x00F0'00F0
 */
template<Unsigned T, size_t shift>
static constexpr T shuffle_mask( size_t density)
{
    T result{};
    for ( size_t i = 0; i < bitwidth<T>; ++i)
        if ( (i >> density) % 4 == shift)
            result |= lsb_set<T>() << i;

    return result;
}

// uint128 implementations

template<>
inline auto popcount( uint128 x) noexcept
{
    const auto& u = unpack_to<uint64>( std::move( x));
    return popcount( u[0]) + popcount( u[1]);
}

template<>
constexpr auto count_leading_zeroes( uint128 x) noexcept
{
    const auto& u = unpack_to<uint64>( std::move( x));
    return u[1] == 0 ? std::countl_zero(u[0]) + bitwidth<uint64> : std::countl_zero(u[1]);
}

template<>
constexpr auto count_leading_ones( uint128 x) noexcept
{
    const auto& u = unpack_to<uint64>( std::move( x));
    return u[1] == all_ones<uint64>() ? std::countl_one(u[0]) + bitwidth<uint64> : std::countl_one(u[1]);
}

template<>
constexpr auto count_trailing_zeroes( uint128 x) noexcept
{
    const auto& u = unpack_to<uint64>( std::move( x));
    return u[0] == 0 ? std::countr_zero(u[1]) + bitwidth<uint64> : std::countr_zero(u[0]);
}
