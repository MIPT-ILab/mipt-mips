/**
 * wide_types.h - Implementation of wide integer types
 *
 * @author Pavel Kryukov <pavel.kryukov@phystech.edu>
 * Copyright 2018 MIPT-MIPS project
 */
 
 // protection from multi-include
#ifndef WIDE_TYPES_H
#define WIDE_TYPES_H

#include <boost/multiprecision/cpp_int.hpp>

#include <infra/types.h>

using int128 = boost::multiprecision::int128_t;
using uint128 = boost::multiprecision::uint128_t;

template<> struct sign<uint128>  { using type = int128; };
template<> struct unsign<int128> { using type = uint128; };

#endif // WIDE_TYPES_H
