/**
 * Unit tests for argv_loader
 * @author Rustem Yunusov <yunusov.rr@phystech.edu>
 * Copyright 2019 MIPT-MIPS iLab project
 */

#include <memory/argv_loader/argv_loader.h>

#include <catch.hpp>

TEST_CASE( "ArgvLoader: load of argc and argv[]")
{
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
    const char* const argv[4] = { "a", "b", "c"};
    auto mem = FuncMemory::create_plain_memory( 20);
    CHECK( ArgvLoader<uint64,  std::endian::little>( argv_cast( argv)).load_to( mem, 0) == 46);
    CHECK( ( mem->read<uint64, std::endian::little>( 0)) == 3);
    CHECK( ( mem->read<uint64, std::endian::little>( 8))  == 40);
    CHECK( ( mem->read<uint64, std::endian::little>( 16)) == 42);
    CHECK( ( mem->read<uint64, std::endian::little>( 24)) == 44);
    CHECK( ( mem->read<uint64, std::endian::little>( 32)) == 0);
    CHECK( ( mem->read<uint16, std::endian::little>( 40)) == 'a');
    CHECK( ( mem->read<uint16, std::endian::little>( 42)) == 'b');
    CHECK( ( mem->read<uint16, std::endian::little>( 44)) == 'c');
}

TEST_CASE( "ArgvLoader: load of argc, argv[] and envp[]")
{
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
    const char* const argv[4] = { "a", "b", "c"};

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
    const char* const envp[3] = { "d", "e"};

    auto mem = FuncMemory::create_plain_memory( 20);
    CHECK( ArgvLoader<uint64,  std::endian::little>( argv_cast( argv), argv_cast( envp)).load_to( mem, 0) == 74);
    CHECK( ( mem->read<uint64, std::endian::little>( 0)) == 3);
    CHECK( ( mem->read<uint64, std::endian::little>( 8))  == 64);
    CHECK( ( mem->read<uint64, std::endian::little>( 16)) == 66);
    CHECK( ( mem->read<uint64, std::endian::little>( 24)) == 68);
    CHECK( ( mem->read<uint64, std::endian::little>( 32)) == 0);
    CHECK( ( mem->read<uint64, std::endian::little>( 40)) == 70);
    CHECK( ( mem->read<uint64, std::endian::little>( 48)) == 72);
    CHECK( ( mem->read<uint64, std::endian::little>( 56)) == 0);
    CHECK( ( mem->read<uint16, std::endian::little>( 64)) == 'a');
    CHECK( ( mem->read<uint16, std::endian::little>( 66)) == 'b');
    CHECK( ( mem->read<uint16, std::endian::little>( 68)) == 'c');
    CHECK( ( mem->read<uint16, std::endian::little>( 70)) == 'd');
    CHECK( ( mem->read<uint16, std::endian::little>( 72)) == 'e');
    CHECK( ( mem->read<uint16, std::endian::little>( 74)) == 0);
}
