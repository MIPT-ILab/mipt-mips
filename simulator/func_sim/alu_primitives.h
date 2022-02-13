/*
 * alu.h - implementation of some ALU primitives
 * Copyright 2014-2020 MIPT-MIPS
 */

#include <infra/macro.h>
#include <infra/types.h>
#include <infra/uint128.h>

#include <bit>

constexpr auto popcount( Unsigned auto x) noexcept              { return std::popcount(x); }
constexpr auto circ_ls( Unsigned auto x, size_t shamt)          { return std::rotl( x, sign_cast<int>( shamt)); }
constexpr auto circ_rs( Unsigned auto x, size_t shamt)          { return std::rotr( x, sign_cast<int>( shamt)); }
constexpr auto count_leading_zeroes( Unsigned auto x) noexcept  { return std::countl_zero( x); }
constexpr auto count_leading_ones( Unsigned auto x) noexcept    { return std::countl_one( x); }
constexpr auto count_trailing_zeroes( Unsigned auto x) noexcept { return std::countr_zero( x); }

/*  
 * Performs an arithmetic right shift, i.e. shift with progapating
 * the most significant bit.
 * 0xF0 sra 2 -> 0xFC
 */
template <typename T>
static constexpr T arithmetic_rs( const T& value, size_t shamt)
{
    using ST = sign_t<T>;
    T result = 0;
    // Result of shifting right a signed value is implementation defined,
    // but for the most of cases it does arithmetic right shift
    // Let's check what our implementation does and reuse it if it is OK
    // NOLINTNEXTLINE(hicpp-signed-bitwise)
    if constexpr ( !std::is_same_v<T, uint128> && ( ST{ -2} >> 1U) == ST{ -1})
        // Compiler does arithmetic shift for signed values, trust it
        // Clang warns about implementation defined code, but we ignore that
        // NOLINTNEXTLINE(hicpp-signed-bitwise)
        result = sign_cast<ST>(value) >> shamt;
    else if ( ( value & msb_set<T>()) == 0)
        result = value >> shamt;        // just shift if MSB is zero
    else
        result = ones_rs( value, shamt);
    return result;
}

bool is_negative( Unsigned auto value) { return (value & msb_set<decltype(value)>()) != 0; }
bool is_positive( Unsigned auto value) { return !is_negative( value) && value != 0; }

template<typename T, typename T1, typename T2> static
auto test_addition_overflow( T1 val1, T2 val2)
{
    const T result = narrow_cast<T>( val1) + narrow_cast<T>( val2);
    const bool is_overflow =
        ( is_positive( val1) && is_positive( val2) && is_negative( result)) ||
        ( is_negative( val1) && is_negative( val2) && is_positive( result));

    return std::pair{ result, is_overflow};
}

template<typename T, typename T1, typename T2> static
auto test_subtraction_overflow( T1 val1, T2 val2)
{
    const T result = narrow_cast<T>( val1) - narrow_cast<T>( val2);
    const bool is_overflow =
        ( is_positive( val1) && is_negative( val2) && is_negative( result)) ||
        ( is_negative( val1) && is_positive( val2) && is_positive( result));

    return std::pair{ result, is_overflow};
}

/*
 * Returns an interleaved mask
 * 0 -> 0x5555'5555 (0101010101...)
 * 1 -> 0x3333'3333 (0011001100...)
 * 2 -> 0x0F0F'0F0F (0000111100...)
 */
template<typename T>
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
template<typename T, size_t shift>
static constexpr T shuffle_mask( size_t density)
{
    T result{};
    for ( size_t i = 0; i < bitwidth<T>; ++i)
        if ( (i >> density) % 4 == shift)
            result |= lsb_set<T>() << i;

    return result;
}

template<typename T>
static inline T gen_reverse( T value, size_t shamt)
{
    for ( size_t i = 0; i < log_bitwidth<T>; ++i) {
        const auto shift = 1U << i;
        if ( ( shamt & shift) != 0)
            value = ( ( value & interleaved_mask<T>( i)) << shift) | ( ( value & ~interleaved_mask<T>( i)) >> shift);
    }

    return value;
}

template<typename T>
static inline T gen_or_combine( T value, size_t shamt)
{
    for ( size_t i = 0; i < log_bitwidth<T>; ++i) {
        const auto shift = 1U << i;
        if ( ( shamt & shift) != 0)
            value |= ( ( value & interleaved_mask<T>( i)) << shift) | ( (value & ~interleaved_mask<T>( i)) >> shift);
    }

    return value;
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

template<>
constexpr auto circ_ls( uint128 x, size_t shamt)
{
    if ( shamt == 0 || shamt == bitwidth<uint128>)
        return x;
    return ( x << shamt) | ( x >> ( bitwidth<uint128> - shamt));
}

template<>
constexpr auto circ_rs( uint128 x, size_t shamt)
{
    return circ_ls( std::move( x), bitwidth<uint128> - shamt);
}