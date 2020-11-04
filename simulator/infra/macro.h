/**
 * macro.h - Implementation of useful inline functions
 *
 * @author Pavel Kryukov <pavel.kryukov@phystech.edu>
 * Copyright 2017-2019 MIPT-MIPS
 */

#ifndef COMMON_MACRO_H
#define COMMON_MACRO_H

#include <infra/types.h>

#include <algorithm>
#include <array>
#include <bitset>
#include <climits>
#include <limits>
#include <type_traits>

/* Checks if values is power of two */
template<typename T>
constexpr bool is_power_of_two( const T& n) noexcept { return (n & (n - 1)) == 0; }

/* Ignore return value */
template<typename T>
void ignored( const T& /* unused */) noexcept { }

/* Find minimal sizeof */
template<typename ... Args>
constexpr size_t min_sizeof() noexcept { return (std::min)({sizeof(Args)...}); }

/* Find maximal sizeof */
template<typename ... Args>
constexpr size_t max_sizeof() noexcept { return (std::max)({sizeof(Args)...}); }

/* Bit width of integer type */
template<typename T>
static constexpr size_t bitwidth = std::numeric_limits<T>::digits + std::numeric_limits<T>::is_signed;

/* 128 types have no std::numeric_limits */
template<> inline constexpr size_t bitwidth<uint128> = 128U;
template<> inline constexpr size_t bitwidth<int128> = 128U;

/* Byte width of integer type */
template<typename T>
static constexpr size_t bytewidth = bitwidth<T> / CHAR_BIT;

/* Bit width / 2 */
template<typename T>
static constexpr size_t half_bitwidth = bitwidth<T> >> 1;

/*
 * Returns value of T type with only the most significant bit set
 * Examples: msb_set<uint8>() -> 0x80
 */
template <typename T>
static constexpr T msb_set() noexcept
{
    return T{ 1U} << (bitwidth<T> - 1);
}

/*
 * Return value of T with only the lest significant bit set
 * Examples: lsb_set<uint8>() -> 0x01
 */
template <typename T>
static constexpr T lsb_set() noexcept
{
    return 1;
}

/*
 * Returns a value full on one bits.
 * all_ones<uint8>()  -> 0xFF
 * all_ones<uint32>() -> 0xFFFF'FFFF
 */
template <typename T>
static constexpr T all_ones()
{
    return (msb_set<T>() - 1U) | msb_set<T>();
}

/* Returns a bitmask with desired amount of LSB set to '1'
 * Examples: bitmask<uint32>(0)  -> 0x0
 *           bitmask<uint32>(5)  -> 0x1F
 *           bitmask<uint32>(32) -> 0xFFFF'FFFF
 */
template <typename T>
static constexpr T bitmask( size_t onecount)
{
    return onecount != 0 ? all_ones<T>() >> ( bitwidth<T> - onecount) : T{ 0};
}

template<typename To, typename From>
static constexpr inline auto unpack_to( From src) noexcept
{
    std::array<To, bytewidth<From> / bytewidth<To>> result{};
    const constexpr size_t offset = bitwidth<To>;
    size_t shift = 0;
    for ( auto& v : result) {
        v = narrow_cast<To>( ( src >> shift) & bitmask<From>( offset));
        shift += offset;
    }

    return result;
}

template<typename From, size_t N>
static constexpr inline auto pack_from( std::array<From, N> src) noexcept
{
    using To = packed_t<N, From>;
    To result{};
    size_t shift = 0;
    for ( const auto& value : src) {
        result |= To{ value} << shift;
        shift += bitwidth<From>;
    }
    return result;
}        

// https://stackoverflow.com/questions/109023/how-to-count-the-number-of-set-bits-in-a-32-bit-integer
template<typename T>
constexpr auto popcount( T x) noexcept
{
    static_assert( std::is_integral<T>::value, "popcount works only for integral types");
    static_assert( std::numeric_limits<T>::radix == 2, "popcount works only for binary types");
    static_assert( bitwidth<T> <= bitwidth<uint64>, "popcount works only for uint64 and narrower types");
    return std::bitset<bitwidth<T>>( typename std::make_unsigned<T>::type{ x }).count();
}

static inline auto popcount( uint128 x) noexcept
{
    const auto& u = unpack_to<uint64>( std::move( x));
    return popcount( u[0]) + popcount( u[1]);
}

template <typename T>
static constexpr bool has_zero( const T& value)
{
    return T(~value) != T();
}

template <typename T>
static constexpr inline auto count_leading_zeroes( const T& value) noexcept
{
    uint8 count = 0;
    for ( auto mask = msb_set<T>(); mask > 0; mask >>= 1U)
    {
        if ( ( value & mask) != 0)
           break;
        count++;
    }
    return count;
}

template <typename T>
static constexpr inline auto count_trailing_zeroes( const T& value) noexcept
{
    uint8 count = 0;
    for ( auto mask = lsb_set<T>(); mask > 0; mask <<= 1U)
    {
        if ( ( value & mask) != 0)
           break;
        count++;
    }
    return count;
}

template <typename T>
static constexpr inline auto count_leading_ones( const T& value) noexcept
{
    return count_leading_zeroes<T>( ~value);
}

template <typename T>
static constexpr inline size_t find_first_set(const T& value) noexcept
{
    if ( value == 0)
        return bitwidth<T>;
    using UT = typename std::make_unsigned<T>::type;
    UT uvalue{ value};
    return bitwidth<UT> - count_leading_zeroes<UT>( uvalue - ( uvalue & ( uvalue - 1U))) - 1U;
}

template <typename T>
static constexpr size_t log_bitwidth = find_first_set(bitwidth<T>);

/*
 * Templated no-value (non-trivial data of given size)
 */
template<typename T> static constexpr T NO_VAL = all_ones<T>();
template<> inline constexpr uint8  NO_VAL<uint8>  = NO_VAL8;
template<> inline constexpr uint16 NO_VAL<uint16> = NO_VAL16;
template<> inline constexpr uint32 NO_VAL<uint32> = NO_VAL32;
template<> inline constexpr uint64 NO_VAL<uint64> = NO_VAL64;

template<typename T>
static constexpr T ones_ls( const T& value, size_t shamt)
{
    return ~( ~value << shamt);
}

template<typename T>
static constexpr T ones_rs( const T& value, size_t shamt)
{
#ifdef _MSC_FULL_VER
    T result = 0;
    if constexpr ( std::is_same_v<T, uint128> || _MSC_FULL_VER >= 192328105) {
        result = ~( ~value >> shamt);
    }
    else {
        // Workaround for Visual Studio bug
        // https://developercommunity.visualstudio.com/content/problem/833637/wrong-compilation-for-ones-right-shift.html
        // TODO (pkryukov): remove once we switch to C++20
        const volatile auto x = ~value;
        result = ~( x >> shamt);
    }
    return result;
#else
    return ~( ~value >> shamt);
#endif
}

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
        result = narrow_cast<ST>(value) >> shamt;
    else if ( ( value & msb_set<T>()) == 0)
        result = value >> shamt;        // just shift if MSB is zero
    else
        result = ones_rs( value, shamt);
    return result;
}

/* Circular left shift*/
template<typename T>
static constexpr T circ_ls( const T& value, size_t shamt)
{
    if ( shamt == 0 || shamt == bitwidth<T>)
        return value;
    return ( value << shamt) | ( value >> ( bitwidth<T> - shamt));
}

/* Circular right shift */
template<typename T>
static constexpr T circ_rs( const T& value, size_t shamt)
{
    return circ_ls(value, bitwidth<T> - shamt);
}

template<typename T>
T sign_extension( T value, size_t bits)
{
    if ( bits < bitwidth<T>) {
        const T msb = bits == 0 ? 0 : T{ 1} << ( bits - 1);
        value = ( ( value & bitmask<T>(bits)) ^ msb) - msb;
    }
    return value;
}

template<size_t N, typename T>
T sign_extension( T value)
{
    return sign_extension(value, N);
}

template<typename T>
bool is_negative( T value)
{
    return (value & msb_set<T>()) != 0;
}

template<typename T>
bool is_positive( T value)
{
    return !is_negative( value) && value != 0;
}

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

template<typename T>
static inline T trap_vector_address( T value)
{
    return value >> 2U & ~(bitmask<T>( 3));
}

#endif
