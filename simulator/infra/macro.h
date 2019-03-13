/**
 * macro.h - Implementation of useful inline functions
 *
 * @author Pavel Kryukov <pavel.kryukov@phystech.edu>
 * Copyright 2017-2018 MIPT-MIPS
 */

#ifndef COMMON__MACRO_H
#define COMMON__MACRO_H

#include <infra/types.h>

#include <algorithm>
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
constexpr size_t bitwidth = std::numeric_limits<T>::digits + std::numeric_limits<T>::is_signed;

/* 128 types have no std::numeric_limits */
template<> constexpr size_t bitwidth<uint128> = 128u;
template<> constexpr size_t bitwidth<int128> = 128u;

/* Byte width of integer type */
template<typename T>
constexpr size_t bytewidth = bitwidth<T> / CHAR_BIT;

// https://stackoverflow.com/questions/109023/how-to-count-the-number-of-set-bits-in-a-32-bit-integer
template<typename T>
constexpr auto popcount( T x) noexcept
{
    static_assert( std::is_integral<T>::value, "popcount works only for integral types");
    static_assert( std::numeric_limits<T>::radix == 2, "popcount works only for binary types");
    static_assert( bitwidth<T> <= bitwidth<uint64>, "popcount works only for uint64 and narrower types");
    return std::bitset<bitwidth<T>>( typename std::make_unsigned<T>::type{ x }).count();
}

/*
 * Returns value of T type with only the most significant bit set
 * Examples: msb_set<uint8>() -> 0x80
 */
template <typename T>
static constexpr T msb_set()
{
    return T{ 1u} << (bitwidth<T> - 1);
}

/*
 * Returns a value full on one bits.
 * all_ones<uint8>()  -> 0xFF
 * all_ones<uint32>() -> 0xFFFF'FFFF
 */
template <typename T>
static constexpr T all_ones()
{
    return (msb_set<T>() - 1u) | msb_set<T>();
}

/* Returns a bitmask with desired amount of LSB set to '1'
 * Examples: bitmask<uint32>(0)  -> 0x0
 *           bitmask<uint32>(5)  -> 0x1F
 *           bitmask<uint32>(32) -> 0xFFFF'FFFF
 */
template <typename T>
static constexpr T bitmask(unsigned int const onecount)
{
    return onecount != 0 ? all_ones<T>() >> (bitwidth<T> - onecount) : T{ 0};
}

template <typename T>
static constexpr bool has_zero( const T& value)
{
    return T(~value) != T();
}

template <typename T>
static constexpr inline size_t count_leading_zeroes(const T& value) noexcept
{
    size_t count = 0;
    for ( auto mask = msb_set<T>(); mask > 0; mask >>= 1u)
    {
        if ( ( value & mask) != 0)
           break;
        count++;
    }
    return count;
}

template <typename T>
static constexpr inline size_t count_leading_ones(const T& value) noexcept
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
    return bitwidth<UT> - count_leading_zeroes<UT>( uvalue - ( uvalue & ( uvalue - 1u))) - 1u;
}

template <typename T>
constexpr size_t log_bitwidth = find_first_set(bitwidth<T>);

/*
 * Templated no-value (non-trivial data of given size)
 */
template<typename T> constexpr T NO_VAL = all_ones<T>();
template<> constexpr uint8  NO_VAL<uint8>  = NO_VAL8;
template<> constexpr uint16 NO_VAL<uint16> = NO_VAL16;
template<> constexpr uint32 NO_VAL<uint32> = NO_VAL32;
template<> constexpr uint64 NO_VAL<uint64> = NO_VAL64;

/*
 * Performs an arithmetic right shift, i.e. shift with progapating
 * the most significant bit.
 * 0xF0 sra 2 -> 0xFC
 */
template <typename T>
static constexpr T arithmetic_rs(const T& value, size_t shamt)
{
    using ST = sign_t<T>;
    T result = 0;
    // Result of shifting right a signed value is implementation defined,
    // but for the most of cases it does arithmetic right shift
    // Let's check what our implementation does and reuse it if it is OK
    // NOLINTNEXTLINE(hicpp-signed-bitwise)
    if constexpr ((ST{ -2} >> 1u) == ST{ -1})
        // Compiler does arithmetic shift for signed values, trust it
        // Clang warns about implementation defined code, but we ignore that
        // NOLINTNEXTLINE(hicpp-signed-bitwise)
        result = narrow_cast<ST>(value) >> shamt;
    else if ((value & msb_set<T>()) == 0)
        result = value >> shamt;        // just shift if MSB is zero
    else
        result = ~((~value) >> shamt);   // invert to propagate zeroes and invert back
    return result;
}

static inline uint128 arithmetic_rs(uint128 value, size_t shamt)
{
    if ((value & msb_set<uint128>()) == 0)
        return value >> shamt;        // just shift if MSB is zero

    return ~((~value) >> shamt);   // invert to propagate zeroes and invert back
}

template<size_t N, typename T>
T sign_extension( T value)
{
    if constexpr (N < bitwidth<T>) {
        const T msb = T{ 1} << ( N - 1);
        value = ( ( value & bitmask<T>(N)) ^ msb) - msb;
    }
    return value;
}

#endif
