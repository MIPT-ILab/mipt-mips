/**
 * macro.h - Implementation of useful inline functions
 *
 * @author Pavel Kryukov <pavel.kryukov@phystech.edu>
 * Copyright 2017 MIPT-MIPS
 */

// protection from multi-include
#ifndef COMMON__MACRO_H
#define COMMON__MACRO_H

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

#endif
