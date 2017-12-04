/**
 * macro.h - Implementation of useful inline functions
 *
 * @author Pavel Kryukov <pavel.kryukov@phystech.edu>
 * Copyright 2017 MIPT-MIPS
 */

// protection from multi-include
#ifndef COMMON__MACRO_H
#define COMMON__MACRO_H

#include <bitset>
#include <limits>
#include <type_traits>
#include <algorithm>

/* Returns size of a static array */
template<typename T, size_t N>
constexpr size_t countof( const T (& /* unused */)[N]) noexcept { return N; }

/* Checks if values is power of two */
template<typename T>
constexpr bool is_power_of_two( const T& n) noexcept { return (n & (n - 1)) == 0; }

/* Ignore return value */
template<typename T>
void ignored( const T& /* unused */) noexcept { }

/* Find minimal sizeof */
template<typename ... Args>
constexpr size_t min_sizeof() noexcept { return std::min(sizeof(Args)...); }

/* Find maximal sizeof */
template<typename ... Args>
constexpr size_t max_sizeof() noexcept { return std::max(sizeof(Args)...); }

// sizeof(x)*CHAR_BIT
template<typename T>
constexpr auto bitwidth = std::numeric_limits<T>::digits + std::numeric_limits<T>::is_signed;

// https://stackoverflow.com/questions/109023/how-to-count-the-number-of-set-bits-in-a-32-bit-integer
template<typename T,
         typename = std::enable_if_t<std::is_integral<T>::value>,         // only integral
         typename = std::enable_if_t<std::numeric_limits<T>::radix == 2>, // only binary
         typename = std::enable_if_t<bitwidth<T> <= bitwidth<unsigned long long>> // only narrow
        >
auto popcount( T x) noexcept
{
    return std::bitset<bitwidth<T>>( static_cast<typename std::make_unsigned<T>::type>( x)).count();
}

#endif
