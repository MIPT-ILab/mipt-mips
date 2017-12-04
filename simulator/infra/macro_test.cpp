/**
 * macro_test.cpp - Compile-time testing of useful inline functions
 *
 * @author Pavel Kryukov <pavel.kryukov@phystech.edu>
 * Copyright 2017 MIPT-MIPS
 */

#include <infra/macro.h>
 
static char array[] = "Hello World!";
static_assert(countof(array) == sizeof(array) / sizeof(char));

static_assert(is_power_of_two(1));
static_assert(is_power_of_two(2));
static_assert(is_power_of_two(4));
static_assert(!is_power_of_two(5));

static_assert(min_sizeof<char, int, uint64>() == sizeof(char));
static_assert(max_sizeof<char, int, uint64>() == sizeof(uint64));

static_assert(bitwidth<int8> == 8);
static_assert(bitwidth<int16> == 16);
static_assert(bitwidth<int32> == 32);
static_assert(bitwidth<int64> == 64);

static_assert(bitwidth<uint8> == 8);
static_assert(bitwidth<uint16> == 16);
static_assert(bitwidth<uint32> == 32);
static_assert(bitwidth<uint64> == 64);

/*
static_assert(popcount(0) == 0);
static_assert(popcount(1) == 1);
static_assert(popcount(MAX_VAL8) == 8);
static_assert(popcount(MAX_VAL16) == 16);
static_assert(popcount(MAX_VAL32) == 32);
static_assert(popcount(~MAX_VAL32) == 0);
static_assert(popcount(~static_cast<uint64>(MAX_VAL32)) == 0);
*/

static_assert(bitmask<uint32>(1) == 1);
static_assert(bitmask<uint32>(3) == 7);
static_assert(bitmask<uint32>(32) == MAX_VAL32);
