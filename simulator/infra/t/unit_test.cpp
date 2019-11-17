/**
 * macro_test.cpp - testing of useful inline functions
 *
 * @author Pavel Kryukov <pavel.kryukov@phystech.edu>
 * Copyright 2017-2018 MIPT-MIPS
 */

#include <catch.hpp>

#include <infra/argv.h>
#include <infra/endian.h>
#include <infra/exception.h>
#include <infra/log.h>
#include <infra/macro.h>

#include <cctype>
#include <memory>
#include <sstream>

static_assert(CHAR_BIT == 8, "MIPT-MIPS supports only 8-bit byte host machines");
static_assert(Endian::native == Endian::little || Endian::native == Endian::big, "MIPT-MIPS does not support mixed-endian hosts");

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
static_assert(popcount(~narrow_cast<uint64>(MAX_VAL32)) == 0);
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
static_assert(count_leading_zeroes<uint8>(uint8{ 0xFF}) == 0);
static_assert(count_leading_zeroes<uint32>(~uint32{ 0}) == 0);
static_assert(count_leading_zeroes<uint64>(~uint64{ 0}) == 0);

static_assert(find_first_set<uint64>(0) == 64);
static_assert(find_first_set<uint32>(0) == 32);
static_assert(find_first_set<uint8>(0) == 8);
static_assert(find_first_set<uint64>(1) == 0);
static_assert(find_first_set<uint32>(1) == 0);
static_assert(find_first_set<uint64>(all_ones<uint64>()) == 0);
static_assert(find_first_set<uint64>(2) == 1);
static_assert(find_first_set<uint64>(3) == 0);
static_assert(find_first_set<uint64>(0xFFFF000) == 12);
static_assert(find_first_set<uint64>(msb_set<uint64>()) == 63);

static_assert(log_bitwidth<uint32> == 5);
static_assert(log_bitwidth<uint64> == 6);

static constexpr std::array<Byte, 4> test_array = {{Byte{0x78}, Byte{0x56}, Byte{0x34}, Byte{0x12}}};

static_assert(unpack_array_le<uint32>( 0x12345678)[0] == test_array[0]);
static_assert(unpack_array_be<uint32>( 0x12345678)[0] == test_array[3]);

static_assert(pack_array_le<uint32>( test_array) == 0x12345678);
static_assert(pack_array_be<uint32>( test_array) == 0x78563412);

static_assert(unpack_array<uint32, Endian::little>( 0x12345678)[0] == test_array[0]);
static_assert(unpack_array<uint32, Endian::big>( 0x12345678)[0] == test_array[3]);

static_assert(pack_array<uint32, Endian::little>( test_array) == 0x12345678);
static_assert(pack_array<uint32, Endian::big>( test_array) == 0x78563412);

static_assert(swap_endian<uint32>(0xFAFBFCFD) == 0xFDFCFBFA);
static_assert(swap_endian<uint8>(0xFA) == 0xFA);

static_assert(get_value_from_pointer<uint16, Endian::little>( test_array.data(), 2) == 0x5678);
static_assert(get_value_from_pointer<uint16, Endian::big>( test_array.data(), 2) == 0x7856);

template<Endian e>
static constexpr auto check_to_pointer()
{
    std::array<Byte, 2> res{};
    put_value_to_pointer<uint16, e>( res.data(), 0x3456, 2);
    return res;
}

static_assert(check_to_pointer<Endian::little>()[0] == Byte{ 0x56});
static_assert(check_to_pointer<Endian::little>()[1] == Byte{ 0x34});
static_assert(check_to_pointer<Endian::big>()[0] == Byte{ 0x34});
static_assert(check_to_pointer<Endian::big>()[1] == Byte{ 0x56});

static constexpr std::array<const char*, 4> some_argv = {"rm", "-rf", "/", nullptr};
static_assert( count_argc( some_argv.data()) == 3);

/* Boost cannot instantiate count_leading_zeroes in constexpr context
static_assert(count_leading_zeroes<uint128>(0x0) == 128);
static_assert(count_leading_zeroes<uint128>(0xFF) == 120);
static_assert(count_leading_zeroes<uint128>(~0x0) == 0);
*/

TEST_CASE("arithmetic shift for 128 instructions")
{
    CHECK( arithmetic_rs( msb_set<uint128>() >> 1, 15) == (uint128{ 1}       << (128 - 17)));
    CHECK( arithmetic_rs( msb_set<uint128>(), 16)      == (uint128{ 0x1ffff} << (128 - 17)));
}

TEST_CASE("sign extension")
{
    CHECK( sign_extension<16, uint32>( 0)          == 0);
    CHECK( sign_extension<16, uint32>( 0x1234)     == 0x1234);
    CHECK( sign_extension<16, uint32>( 0x8000)     == 0xffff8000);
    CHECK( sign_extension<16, uint32>( 0xffff)     == 0xffffffff);
    CHECK( sign_extension<16, uint32>( 0xffffffff) == 0xffffffff);
    CHECK( sign_extension<8,  uint32>( 0xffff0000) == 0x0);
    CHECK( sign_extension<16, uint32>( 0xffff0000) == 0x0);
    CHECK( sign_extension<17, uint32>( 0xffff0000) == 0xffff0000);
    CHECK( sign_extension<32, uint64>( 0x80000000) == 0xffff'ffff'8000'0000);
    CHECK( ~sign_extension<16, uint128>( 0xff00) == 0xff );
}

TEST_CASE("Exception")
{
    try {
        throw Exception( "Hello World!");
    }
    catch (const std::runtime_error& e) {
        CHECK( std::string( "Unqualified exception:\tHello World!\n") == e.what() );
    }
}

TEST_CASE("Logging enabled")
{
    std::ostringstream oss;
    LogOstream ls(oss);
    ls.enable();
    ls << "Hello World! " << std::hex << 20 << std::endl;
    CHECK( oss.str() == "Hello World! 14\n" );
}

TEST_CASE("Logging disabled")
{
    std::ostringstream oss;
    LogOstream ls(oss);
    ls.enable();
    ls.disable();
    ls << "Hello World! " << std::hex << 20 << std::endl;
    CHECK( oss.str().empty() );
}

TEST_CASE("Find first set")
{
    auto val = std::make_unique<unsigned>( 0);
    CHECK( find_first_set( *val) == bitwidth<unsigned> );
}

static std::string uint128_to_hex_string(uint128 number)
{
    std::ostringstream out;
    out << std::hex << std::uppercase << number;
    return out.str();
}

static std::string uint128_to_hex_showbase_string(uint128 number)
{
    std::ostringstream out;
    out << std::showbase << std::hex << std::uppercase << number;
    return out.str();
}

static std::string uint128_to_dec_string(uint128 number)
{
     std::ostringstream out;
     out << number;
     return out.str();
}

static std::string uint128_to_oct_string(uint128 number)
{
     std::ostringstream out;
     out << std::oct << number;
     return out.str();
}

static std::string str_toupper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), 
                   [](unsigned char c){ return std::toupper(c); }
                  );
    return s;
}

TEST_CASE("Test uint128 decimal printing")
{
    CHECK( uint128_to_dec_string( 1) == "1");
    CHECK( uint128_to_dec_string( narrow_cast<uint128>(UINT64_MAX) + 1) == "18446744073709551616");
    CHECK( uint128_to_dec_string( uint128{ 10'000'000'000'000'000'000ULL} * 2) == "20000000000000000000");
    CHECK( uint128_to_dec_string( all_ones<uint128>()) == "340282366920938463463374607431768211455");
}

TEST_CASE("Test uint128 hexadecimal printing")
{
    CHECK( uint128_to_hex_string( 1) == "1");
    CHECK( uint128_to_hex_string( 0xABCDEF) == "ABCDEF");
    CHECK( uint128_to_hex_string( narrow_cast<uint128>(UINT64_MAX) + 1) == "10000000000000000");
    CHECK( uint128_to_hex_string( all_ones<uint128>()) == "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
    // Boost bug: https://github.com/boostorg/multiprecision/issues/129
    CHECK( str_toupper( uint128_to_hex_showbase_string( all_ones<uint128>())) == "0XFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
}

TEST_CASE("Test uint128 octal printing")
{
    CHECK( uint128_to_oct_string( 1) == "1");
    CHECK( uint128_to_oct_string( 0765) == "765");
    CHECK( uint128_to_oct_string( narrow_cast<uint128>(UINT64_MAX) + 1) == "2000000000000000000000");
    CHECK( uint128_to_oct_string( all_ones<uint128>()) == "3777777777777777777777777777777777777777777");
}

TEST_CASE("Test uint32 circular left shift")
{
    uint32 value = 0xA0B0'C0D0;
    CHECK( value == circ_ls( value, 0));
    CHECK( value == circ_ls( value, 32));
    CHECK( 0x0B0C'0D0A == circ_ls( value, 4));
}

TEST_CASE("Test uint64 circular left shift")
{
    uint64 value = 0xA0B0'C0D0'A0B0'C0D0;
    CHECK( value == circ_ls( value, 0));
    CHECK( value == circ_ls( value, 64));
    CHECK( 0x0B0C'0D0A'0B0C'0D0A == circ_ls( value, 4));
}
