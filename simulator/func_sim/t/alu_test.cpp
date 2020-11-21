/**
 * alu_test.cpp - testing of useful inline functions
 *
 * @author Pavel Kryukov <pavel.kryukov@phystech.edu>
 * Copyright 2017-2018 MIPT-MIPS
 */
 
#include <catch.hpp>

#include <func_sim/alu_primitives.h>

static_assert(is_power_of_two(1U));
static_assert(is_power_of_two(2U));
static_assert(is_power_of_two(4U));
static_assert(!is_power_of_two(5U));

static_assert(count_leading_zeroes<uint8>(0xFF) == 0);
static_assert(count_leading_zeroes<uint32>(0xFF) == 24);
static_assert(count_leading_zeroes<uint64>(0xFF) == 56);
static_assert(count_leading_zeroes<uint8>(0x0) == 8);
static_assert(count_leading_zeroes<uint32>(0x0) == 32);
static_assert(count_leading_zeroes<uint64>(0x0) == 64);
static_assert(count_leading_zeroes<uint8>(uint8{ 0xFF}) == 0);
static_assert(count_leading_zeroes<uint32>(~uint32{ 0}) == 0);
static_assert(count_leading_zeroes<uint64>(~uint64{ 0}) == 0);

static_assert( arithmetic_rs<uint64>( 0xA, 1) == 0x5);
static_assert( arithmetic_rs<uint64>( msb_set<uint64>(), 3) == ones_rs<uint64>( msb_set<uint64>(), 3));

static_assert( interleaved_mask<uint32>(0) == 0x5555'5555);
static_assert( interleaved_mask<uint32>(1) == 0x3333'3333);
static_assert( interleaved_mask<uint32>(2) == 0x0F0F'0F0F);
static_assert( interleaved_mask<uint32>(3) == 0x00FF'00FF);
static_assert( interleaved_mask<uint32>(4) == 0x0000'FFFF);

static_assert( shuffle_mask<uint32, 2>(0) == 0x4444'4444);
static_assert( shuffle_mask<uint32, 2>(1) == 0x3030'3030);
static_assert( shuffle_mask<uint32, 2>(2) == 0x0F00'0F00);

static_assert( shuffle_mask<uint32, 1>(0) == 0x2222'2222);
static_assert( shuffle_mask<uint32, 1>(1) == 0x0C0C'0C0C);
static_assert( shuffle_mask<uint32, 1>(2) == 0x00F0'00F0);

static_assert(popcount(0U) == 0);
static_assert(popcount(1U) == 1);
static_assert(popcount(MAX_VAL8) == 8);
static_assert(popcount(MAX_VAL16) == 16);
static_assert(popcount(MAX_VAL32) == 32);
static_assert(popcount(~MAX_VAL32) == 0);

TEST_CASE("count_leading for 128 bit")
{
    CHECK( count_leading_zeroes<uint128>( 0) == 128);
    CHECK( count_leading_zeroes<uint128>( 0xFF) == 120);
    CHECK( count_leading_zeroes<uint128>( all_ones<uint128>()) == 0);
    CHECK( count_leading_zeroes<uint128>( msb_set<uint128>()) == 0);
    CHECK( count_leading_zeroes<uint128>( bitmask<uint128>( 65)) == 63);

    CHECK( count_leading_ones<uint128>( 0) == 0);
    CHECK( count_leading_ones<uint128>( 0xFF) == 0);
    CHECK( count_leading_ones<uint128>( all_ones<uint128>()) == 128);
    CHECK( count_leading_ones<uint128>( msb_set<uint128>()) == 1);
    CHECK( count_leading_ones<uint128>( ~bitmask<uint128>( 65)) == 63);
}

TEST_CASE("count_trailing for 128 bit")
{
    CHECK( count_trailing_zeroes<uint128>( 0) == 128);
    CHECK( count_trailing_zeroes<uint128>( 0xFF) == 0);
    CHECK( count_trailing_zeroes<uint128>( all_ones<uint128>()) == 0);
    CHECK( count_trailing_zeroes<uint128>( msb_set<uint128>()) == 127);
    CHECK( count_trailing_zeroes<uint128>( ~bitmask<uint128>( 65)) == 65);
}


TEST_CASE("ones shift for 128 instructions")
{
    CHECK( ones_ls<uint128>( 0x1, 1) == 0x3);
    CHECK( ones_ls<uint128>( 0x8, 4) == 0x8f);
    CHECK( ones_ls<uint128>( msb_set<uint128>(), 1) == 0x1);

    const size_t shift = 128 - 17;
    CHECK( ones_rs( msb_set<uint128>() >> 1, 15) == (uint128{ 0x1fffd} << shift));
    CHECK( ones_rs( msb_set<uint128>(), 16)      == (uint128{ 0x1ffff} << shift));
    CHECK( arithmetic_rs( msb_set<uint128>() >> 1, 15) == (uint128{ 1}       << shift));
    CHECK( arithmetic_rs( msb_set<uint128>(), 16)      == (uint128{ 0x1ffff} << shift));
}

TEST_CASE("circ shift for 128")
{
    CHECK( ones_ls<uint128>( 0x1, 1) == 0x3);
    CHECK( ones_ls<uint128>( 0x8, 4) == 0x8f);
    CHECK( ones_ls<uint128>( msb_set<uint128>(), 1) == 0x1);

    const size_t shift = 128 - 17;
    CHECK( ones_rs( msb_set<uint128>() >> 1, 15) == (uint128{ 0x1fffd} << shift));
    CHECK( ones_rs( msb_set<uint128>(), 16)      == (uint128{ 0x1ffff} << shift));
    CHECK( arithmetic_rs( msb_set<uint128>() >> 1, 15) == (uint128{ 1}       << shift));
    CHECK( arithmetic_rs( msb_set<uint128>(), 16)      == (uint128{ 0x1ffff} << shift));
}

static_assert( circ_ls<uint32>( 0xA0B0'C0D7, 0)  == 0xA0B0'C0D7);
static_assert( circ_ls<uint32>( 0xA0B0'C0D7, 32) == 0xA0B0'C0D7);
static_assert( circ_ls<uint32>( 0xA0B0'C0D7, 4)  == 0x0B0'C0D7A);

static_assert( circ_ls<uint64>( 0xA0B0'C0D0'A0B0'C0D7, 0)  == 0xA0B0'C0D0'A0B0'C0D7);
static_assert( circ_ls<uint64>( 0xA0B0'C0D0'A0B0'C0D7, 32) == 0xA0B0'C0D7'A0B0'C0D0);
static_assert( circ_ls<uint64>( 0xA0B0'C0D0'A0B0'C0D7, 4)  == 0x0B0C'0D0A'0B0C'0D7A);

static_assert( circ_rs<uint32>( 0xA0B0'C0D7, 0)  == 0xA0B0'C0D7);
static_assert( circ_rs<uint32>( 0xA0B0'C0D7, 32) == 0xA0B0'C0D7);
static_assert( circ_rs<uint32>( 0xA0B0'C0D7, 4)  == 0x7A0B'0C0D);

static_assert( circ_rs<uint64>( 0xA0B0'C0D0'A0B0'C0D7, 0)  == 0xA0B0'C0D0'A0B0'C0D7);
static_assert( circ_rs<uint64>( 0xA0B0'C0D0'A0B0'C0D7, 32) == 0xA0B0'C0D7'A0B0'C0D0);
static_assert( circ_rs<uint64>( 0xA0B0'C0D0'A0B0'C0D7, 4)  == 0x7A0B'0C0D'0A0B'0C0D);

TEST_CASE("circular right shift for 128 bit")
{
    CHECK( unpack_to<uint64>( circ_rs<uint128>( 0xABCD, 8))[0] == 0xAB);
    CHECK( unpack_to<uint64>( circ_rs<uint128>( 0xABCD, 8))[1] == 0xCD00'0000'0000'0000);
    CHECK( unpack_to<uint64>( circ_rs<uint128>( 0xABCD, 0))[0] == 0xABCD);
    CHECK( unpack_to<uint64>( circ_rs<uint128>( 0xABCD, 0))[1] == 0);
    CHECK( unpack_to<uint64>( circ_rs<uint128>( 0xABCD, 128))[0] == 0xABCD);
    CHECK( unpack_to<uint64>( circ_rs<uint128>( 0xABCD, 128))[1] == 0x0);
}
