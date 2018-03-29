/**
 * wide_types.h - Implementation of wide integer types
 *
 * @author Pavel Kryukov <pavel.kryukov@phystech.edu>
 * Copyright 2018 MIPT-MIPS project
 */
 
 // protection from multi-include
#ifndef WIDE_TYPES_H
#define WIDE_TYPES_H

#include <infra/types.h>

// Use native GCC type if available, as Boost <= 1.60 + GCC 7 generate a bug
#if defined(__GNUC__) && !defined(__clang__) && defined(__SIZEOF_INT128__)

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

using int128 = __int128;
using uint128 = unsigned __int128;

#pragma GCC diagnostic pop

#else // __SIZEOF_INT128__

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/integer.hpp>
using int128 = boost::multiprecision::int128_t;
using uint128 = boost::multiprecision::uint128_t;

#endif // __SIZEOF_INT128__

template<> struct sign<uint128>   { using type = int128;  };
template<> struct unsign<int128>  { using type = uint128; };
template<> struct doubled<uint64> { using type = uint128; };
template<> struct doubled<int64>  { using type = int128;  };

#endif // WIDE_TYPES_H
