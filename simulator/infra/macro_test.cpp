/**
 * macro_test.cpp - Compile-time testing of useful inline functions
 *
 * @author Pavel Kryukov <pavel.kryukov@phystech.edu>
 * Copyright 2017 MIPT-MIPS
 */

#include <infra/macro.h>

static char array[] = "Hello World!";
static_assert(countof(array) == sizeof(array) / sizeof(char));

static_assert(is_power_of_two(1u));
static_assert(is_power_of_two(2u));
static_assert(is_power_of_two(4u));
static_assert(!is_power_of_two(5u));

static_assert(min_sizeof<char, int, uint64>() == sizeof(char));
static_assert(max_sizeof<char, int, uint64>() == sizeof(uint64));

static_assert(bitwidth<int8> == 8);
static_assert(bitwidth<int16> == 16);
static_assert(bitwidth<int32> == 32);
static_assert(bitwidth<int64> == 64);
static_assert(bitwidth<int128> == 128);

static_assert(bitwidth<uint8> == 8);
static_assert(bitwidth<uint16> == 16);
static_assert(bitwidth<uint32> == 32);
static_assert(bitwidth<uint64> == 64);
static_assert(bitwidth<uint128> == 128);

static_assert(std::is_same_v<uint8,  unsign_t<int8>>);
static_assert(std::is_same_v<uint16, unsign_t<int16>>);
static_assert(std::is_same_v<uint32, unsign_t<int32>>);
static_assert(std::is_same_v<uint64, unsign_t<int64>>);

static_assert(std::is_same_v<int8,  sign_t<uint8>>);
static_assert(std::is_same_v<int16, sign_t<uint16>>);
static_assert(std::is_same_v<int32, sign_t<uint32>>);
static_assert(std::is_same_v<int64, sign_t<uint64>>);

static_assert(bitwidth<doubled_t<int8>> == 2 * bitwidth<int8>);
static_assert(bitwidth<doubled_t<int16>> == 2 * bitwidth<int16>);
static_assert(bitwidth<doubled_t<int32>> == 2 * bitwidth<int32>);

static_assert(bitwidth<doubled_t<uint8>> == 2 * bitwidth<uint8>);
static_assert(bitwidth<doubled_t<uint16>> == 2 * bitwidth<uint16>);
static_assert(bitwidth<doubled_t<uint32>> == 2 * bitwidth<uint32>);

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

static_assert(count_leading_zeroes<uint8>(0xFF) == 0);
static_assert(count_leading_zeroes<uint32>(0xFF) == 24);
static_assert(count_leading_zeroes<uint64>(0xFF) == 56);
static_assert(count_leading_zeroes<uint128>(0xFF) == 120);
static_assert(count_leading_zeroes<uint8>(0x0) == 8);
static_assert(count_leading_zeroes<uint32>(0x0) == 32);
static_assert(count_leading_zeroes<uint64>(0x0) == 64);
static_assert(count_leading_zeroes<uint128>(0x0) == 128);
static_assert(count_leading_zeroes<uint8>(~0x0) == 0);
static_assert(count_leading_zeroes<uint32>(~0x0) == 0);
static_assert(count_leading_zeroes<uint64>(~0x0) == 0);
static_assert(count_leading_zeroes<uint128>(~0x0) == 0);
