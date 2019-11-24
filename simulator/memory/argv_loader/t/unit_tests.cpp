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
    CHECK( ArgvLoader<uint64,  Endian::little>( argv).load_to( mem, 0) == 46);
    CHECK( ( mem->read<uint64, Endian::little>( 0)) == 3);
    CHECK( ( mem->read<uint64, Endian::little>( 8))  == 40);
    CHECK( ( mem->read<uint64, Endian::little>( 16)) == 42);
    CHECK( ( mem->read<uint64, Endian::little>( 24)) == 44);
    CHECK( ( mem->read<uint64, Endian::little>( 32)) == 0);
    CHECK( ( mem->read<uint16, Endian::little>( 40)) == 'a');
    CHECK( ( mem->read<uint16, Endian::little>( 42)) == 'b');
    CHECK( ( mem->read<uint16, Endian::little>( 44)) == 'c');
}

TEST_CASE( "ArgvLoader: load of argc, argv[] and envp[]")
{
    const char* argv[4] = { "a", "b", "c"};
    const char* envp[3] = { "d", "e"};
    auto mem = FuncMemory::create_plain_memory( 20);
    CHECK( ArgvLoader<uint64,  Endian::little>( argv, envp).load_to( mem, 0) == 74);
    CHECK( ( mem->read<uint64, Endian::little>( 0)) == 3);
    CHECK( ( mem->read<uint64, Endian::little>( 8))  == 64);
    CHECK( ( mem->read<uint64, Endian::little>( 16)) == 66);
    CHECK( ( mem->read<uint64, Endian::little>( 24)) == 68);
    CHECK( ( mem->read<uint64, Endian::little>( 32)) == 0);
    CHECK( ( mem->read<uint64, Endian::little>( 40)) == 70);
    CHECK( ( mem->read<uint64, Endian::little>( 48)) == 72);
    CHECK( ( mem->read<uint64, Endian::little>( 56)) == 0);
    CHECK( ( mem->read<uint16, Endian::little>( 64)) == 'a');
    CHECK( ( mem->read<uint16, Endian::little>( 66)) == 'b');
    CHECK( ( mem->read<uint16, Endian::little>( 68)) == 'c');
    CHECK( ( mem->read<uint16, Endian::little>( 70)) == 'd');
    CHECK( ( mem->read<uint16, Endian::little>( 72)) == 'e');
    CHECK( ( mem->read<uint16, Endian::little>( 74)) == 0);
}
