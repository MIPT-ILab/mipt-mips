/**
 * macro_test.cpp - Compile-time testing of useful inline functions
 *
 * @author Pavel Kryukov <pavel.kryukov@phystech.edu>
 * Copyright 2017-2018 MIPT-MIPS
 */

#include <infra/macro.h>
#include <infra/endian.h>

static_assert(CHAR_BIT == 8, "MIPT-MIPS supports only 8-bit byte host machines");
static_assert(Endian::native == Endian::little || Endian::native == Endian::big, "MIPT-MIPS does not support mixed-endian hosts");

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

static_assert(all_ones<uint8>()  == 0xFFull);
static_assert(all_ones<uint16>() == 0xFFFFull);
static_assert(all_ones<uint32>() == 0xFFFF'FFFFull);

static_assert(msb_set<uint8>()  == 0x80ull);
static_assert(msb_set<uint16>() == 0x8000ull);
static_assert(msb_set<uint32>() == 0x8000'0000ull);

/* Check that NO_VAL values are really non-trivial */
static_assert(NO_VAL<uint8> != 0);
static_assert(NO_VAL<uint8> != all_ones<uint8>());
static_assert(NO_VAL<uint8> != msb_set<uint8>());
static_assert(NO_VAL<uint16> != 0);
static_assert(NO_VAL<uint16> != all_ones<uint16>());
static_assert(NO_VAL<uint16> != msb_set<uint16>());
static_assert(NO_VAL<uint32> != 0);
static_assert(NO_VAL<uint32> != all_ones<uint32>());
static_assert(NO_VAL<uint32> != msb_set<uint32>());
static_assert(NO_VAL<uint64> != 0);
static_assert(NO_VAL<uint64> != all_ones<uint64>());
static_assert(NO_VAL<uint64> != msb_set<uint64>());

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
static_assert(count_leading_zeroes<uint8>(0x0) == 8);
static_assert(count_leading_zeroes<uint32>(0x0) == 32);
static_assert(count_leading_zeroes<uint64>(0x0) == 64);
static_assert(count_leading_zeroes<uint8>(static_cast<uint8>(0xFF)) == 0);
static_assert(count_leading_zeroes<uint32>(~static_cast<uint32>(0)) == 0);
static_assert(count_leading_zeroes<uint64>(~static_cast<uint64>(0)) == 0);

static constexpr std::array<Byte, 4> test_array = {{Byte{0x78}, Byte{0x56}, Byte{0x34}, Byte{0x12}}};

// static_assert(unpack_array_le<uint32>( 0x12345678)[0] == test_array[0]);
// static_assert(unpack_array_be<uint32>( 0x12345678)[0] == test_array[3]);

static_assert(pack_array_le<uint32>( test_array) == 0x12345678);
static_assert(pack_array_be<uint32>( test_array) == 0x78563412);

// static_assert(unpack_array<uint32, Endian::little>( 0x12345678)[0] == test_array[0]);
// static_assert(unpack_array<uint32, Endian::big>( 0x12345678)[0] == test_array[3]);

static_assert(pack_array<uint32, Endian::little>( test_array) == 0x12345678);
static_assert(pack_array<uint32, Endian::big>( test_array) == 0x78563412);

/* Boost cannot instantiate count_leading_zeroes in constexpr context
static_assert(count_leading_zeroes<uint128>(0x0) == 128);
static_assert(count_leading_zeroes<uint128>(0xFF) == 120);
static_assert(count_leading_zeroes<uint128>(~0x0) == 0);
*/
