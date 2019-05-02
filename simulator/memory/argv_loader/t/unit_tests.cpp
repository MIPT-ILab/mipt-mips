/**
 * Unit tests for argv_loader
 * @author Rustem Yunusov <yunusov.rr@phystech.edu>
 * Copyright 2019 MIPT-MIPS iLab project
 */

#include <catch.hpp>
#include "../argv_loader.h"

TEST_CASE( "ArgvLoader: argv[2] = nullptr while argc >= 3")
{
    const char* argv[4] = { "a", "b", "c"};
    auto argv_loader = ArgvLoader( argv);
    argv[2] = nullptr;
    auto mem = FuncMemory::create_plain_memory( 20);
    CHECK_THROWS_AS( argv_loader.load_argv_to( mem, 0), ArgvLoaderError);
}

TEST_CASE( "ArgvLoader: load of argc and argv[]")
{
    const char* argv[4] = { "a", "b", "c"};
    auto argv_loader = ArgvLoader( argv);
    auto mem = FuncMemory::create_plain_memory( 20);
    CHECK( argv_loader.load_argv_to( mem, 0) == 42);
    CHECK( ( mem->read<int,    Endian::little>( 0)) == 3);
    CHECK( ( mem->read<uint64, Endian::little>( 4))  == uint64( argv[0]));
    CHECK( ( mem->read<uint64, Endian::little>( 12)) == uint64( argv[1]));
    CHECK( ( mem->read<uint64, Endian::little>( 20)) == uint64( argv[2]));
    CHECK( ( mem->read<uint64, Endian::little>( 28)) == 0);
    CHECK( ( mem->read<uint16, Endian::little>( 36)) == 'a');
    CHECK( ( mem->read<uint16, Endian::little>( 38)) == 'b');
    CHECK( ( mem->read<uint16, Endian::little>( 40)) == 'c');
}

TEST_CASE( "ArgvLoader: load of argc, argv[] and envp[]")
{
    const char* argv[4] = { "a", "b", "c"};
    const char* envp[3] = { "d", "e"};
    auto argv_loader = ArgvLoader( argv, envp);
    auto mem = FuncMemory::create_plain_memory( 20);
    CHECK( argv_loader.load_argv_to( mem, 0) == 70);
    CHECK( ( mem->read<int,    Endian::little>( 0)) == 3);
    CHECK( ( mem->read<uint64, Endian::little>( 4))  == uint64( argv[0]));
    CHECK( ( mem->read<uint64, Endian::little>( 12)) == uint64( argv[1]));
    CHECK( ( mem->read<uint64, Endian::little>( 20)) == uint64( argv[2]));
    CHECK( ( mem->read<uint64, Endian::little>( 28)) == 0);
    CHECK( ( mem->read<uint64, Endian::little>( 36)) == uint64( envp[0]));
    CHECK( ( mem->read<uint64, Endian::little>( 44)) == uint64( envp[1]));
    CHECK( ( mem->read<uint64, Endian::little>( 52)) == 0);
    CHECK( ( mem->read<uint16, Endian::little>( 60)) == 'a');
    CHECK( ( mem->read<uint16, Endian::little>( 62)) == 'b');
    CHECK( ( mem->read<uint16, Endian::little>( 64)) == 'c');
    CHECK( ( mem->read<uint16, Endian::little>( 66)) == 'd');
    CHECK( ( mem->read<uint16, Endian::little>( 68)) == 'e');
    CHECK( ( mem->read<uint16, Endian::little>( 70)) == 0);
}
