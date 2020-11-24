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

template<> struct unsign<uint128>  { using type = uint128; };
template<> struct unsign<int128>   { using type = uint128; };
template<> struct sign<uint128>    { using type = int128;  };
template<> struct doubled<uint64>  { using type = uint128; };
template<> struct doubled<int64>   { using type = int128;  };
template<> struct halved<uint128>  { using type = uint64;  };
template<> inline constexpr size_t bitwidth<uint128> = 128U;
template<> inline constexpr size_t bitwidth<int128> = 128U;

#endif // #ifndef COMMON_TYPES_H
