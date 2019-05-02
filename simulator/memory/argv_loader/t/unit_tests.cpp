/**
 * Unit tests for argv_loader
 * @author Rustem Yunusov <yunusov.rr@phystech.edu>
 * Copyright 2019 MIPT-MIPS iLab project
 */

#include "../argv_loader.h"

#include <catch.hpp>

TEST_CASE( "ArgvLoader: load of argc and argv[]")
{
    const char* argv[4] = { "a", "b", "c"};
    auto mem = FuncMemory::create_plain_memory( 20);
    CHECK( ArgvLoader<Endian::little>( argv).load_to( mem, 0) == 42);
    CHECK( ( mem->read<int,    Endian::little>( 0)) == 3);
    CHECK( ( mem->read<uint64, Endian::little>( 4))  == 36);
    CHECK( ( mem->read<uint64, Endian::little>( 12)) == 38);
    CHECK( ( mem->read<uint64, Endian::little>( 20)) == 40);
    CHECK( ( mem->read<uint64, Endian::little>( 28)) == 0);
    CHECK( ( mem->read<uint16, Endian::little>( 36)) == 'a');
    CHECK( ( mem->read<uint16, Endian::little>( 38)) == 'b');
    CHECK( ( mem->read<uint16, Endian::little>( 40)) == 'c');
}

TEST_CASE( "ArgvLoader: load of argc, argv[] and envp[]")
{
    const char* argv[4] = { "a", "b", "c"};
    const char* envp[3] = { "d", "e"};
    auto mem = FuncMemory::create_plain_memory( 20);
    CHECK( ArgvLoader<Endian::little>( argv, envp).load_to( mem, 0) == 70);
    CHECK( ( mem->read<int,    Endian::little>( 0)) == 3);
    CHECK( ( mem->read<uint64, Endian::little>( 4))  == 60);
    CHECK( ( mem->read<uint64, Endian::little>( 12)) == 62);
    CHECK( ( mem->read<uint64, Endian::little>( 20)) == 64);
    CHECK( ( mem->read<uint64, Endian::little>( 28)) == 0);
    CHECK( ( mem->read<uint64, Endian::little>( 36)) == 66);
    CHECK( ( mem->read<uint64, Endian::little>( 44)) == 68);
    CHECK( ( mem->read<uint64, Endian::little>( 52)) == 0);
    CHECK( ( mem->read<uint16, Endian::little>( 60)) == 'a');
    CHECK( ( mem->read<uint16, Endian::little>( 62)) == 'b');
    CHECK( ( mem->read<uint16, Endian::little>( 64)) == 'c');
    CHECK( ( mem->read<uint16, Endian::little>( 66)) == 'd');
    CHECK( ( mem->read<uint16, Endian::little>( 68)) == 'e');
    CHECK( ( mem->read<uint16, Endian::little>( 70)) == 0);
}
