/**
 * Unit tests for argv_loader
 * @author Rustem Yunusov <yunusov.rr@phystech.edu>
 * Copyright 2019 MIPT-MIPS iLab project
 */

#include <catch.hpp>
#include "../argv_loader.h"

TEST_CASE( "ArgvLoader: argc = 0")
{
    const char* argv[1] = {nullptr};
    CHECK_THROWS_AS( ArgvLoader( argv), InvalidArgs);
}

TEST_CASE( "ArgvLoader: argv = nullptr")
{
    CHECK_THROWS_AS( ArgvLoader( nullptr), InvalidArgs);
}

TEST_CASE( "ArgvLoader: argv[2] = nullptr while argc >= 3")
{
    const char* argv[3] = { "a", "b", "c"};
    auto argv_loader = ArgvLoader( argv);
    argv[2] = nullptr;
    auto mem = FuncMemory::create_plain_memory( 20);
    CHECK_THROWS_AS( argv_loader.load_argv_to( mem, 0), ArgvLoaderError);
}
