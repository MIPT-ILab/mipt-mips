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

#ifdef __SIZEOF_INT128__ // Use native GCC type if available as Boost may contain bugs

using int128 = __int128;
using uint128 = unsigned __int128

#else // __SIZEOF_INT128__

#include <boost/multiprecision/cpp_int.hpp>

using int128 = boost::multiprecision::int128_t;
using uint128 = boost::multiprecision::uint128_t;

#endif // __SIZEOF_INT128__

template<> struct sign<uint128>  { using type = int128; };
template<> struct unsign<int128> { using type = uint128; };

#endif // WIDE_TYPES_H
