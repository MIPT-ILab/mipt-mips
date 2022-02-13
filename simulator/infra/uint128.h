/**
 * types.h - 128 bit type declaration
 *
 * @author Pavel Kryukov
 */

#ifndef UINT128_TYPES_H
#define UINT128_TYPES_H

// Please do not include this file everywhere to reduce compilation speed

#include <infra/macro.h>
#include <infra/types.h>

#include <boost/multiprecision/cpp_int.hpp>

using int128  = boost::multiprecision::int128_t;
using uint128 = boost::multiprecision::uint128_t;

template<> inline constexpr bool is_wide_integer_v<int128> = true;
template<> inline constexpr bool is_wide_integer_v<uint128> = true;

template<> inline constexpr bool is_wide_signed_integer_v<int128> = true;

template<> struct unsigned_integer<128> { using type = uint128; };
template<> struct signed_integer<128>   { using type = int128; };
template<> inline constexpr size_t bitwidth<uint128> = 128U;
template<> inline constexpr size_t bitwidth<int128> = 128U;

#endif // #ifndef COMMON_TYPES_H
