/*
* This is an open source non-commercial project. Dear PVS-Studio, please check it.
* PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
*/
// generic C
#include <cassert>
#include <cstdlib>

// Google Test library
#include <gtest/gtest.h>

// MIPT-MIPS modules
#include "../bpu.h"


TEST( Initialization, WrongParameters)
{
    BPFactory bp_factory;
    // Check failing with wrong input values
    ASSERT_EXIT( bp_factory.create( "dynamic_two_bit", 100, 20),  ::testing::ExitedWithCode( EXIT_FAILURE), "ERROR.*");
    ASSERT_EXIT( bp_factory.create( "dynamic_two_bit", 100, 20);, ::testing::ExitedWithCode( EXIT_FAILURE), "ERROR.*");
    ASSERT_EXIT( bp_factory.create( "dynamic_two_bit", 100, 20);,  ::testing::ExitedWithCode( EXIT_FAILURE), "ERROR.*");
}

TEST( HitAndMiss, Miss)
{
    BPFactory bp_factory;
    auto bp = bp_factory.create( "dynamic_two_bit", 128, 16);

    // Check default cache miss behaviour
    Addr PC = 12;
    ASSERT_EQ( bp->is_taken(PC), 0);

    PC = 16;
    ASSERT_EQ( bp->is_taken(PC), 0);

    PC = 20;
    ASSERT_EQ( bp->is_taken(PC), 0);

    PC = 12;
    ASSERT_EQ( bp->is_taken(PC), 0);
}

TEST( Main, PredictingBits)
{
    BPFactory bp_factory;
    auto bp = bp_factory.create( "dynamic_two_bit", 128, 16);

    Addr PC = 12;
    Addr target = 28;

    // Teaching
    bp->update( true, PC, target);
    ASSERT_EQ( bp->is_taken(PC), 1);
    ASSERT_EQ( bp->get_target(PC), target);

    bp->update( true, PC, target);
    ASSERT_EQ( bp->is_taken(PC), 1);
    ASSERT_EQ( bp->get_target(PC), target);

    // "Over" - teaching
    bp->update( true, PC, target);
    ASSERT_EQ( bp->is_taken(PC), 1);
    ASSERT_EQ( bp->get_target(PC), target);

    bp->update( true, PC, target);
    ASSERT_EQ( bp->is_taken(PC), 1);
    ASSERT_EQ( bp->get_target(PC), target);

    // "Un" - teaching
    bp->update( false, PC, NO_VAL32);
    ASSERT_EQ( bp->is_taken(PC), 1);
    ASSERT_EQ( bp->get_target(PC), target);

    // Strong "un" - teaching
    bp->update( false, PC, NO_VAL32);
    bp->update( false, PC, NO_VAL32);
    bp->update( false, PC, NO_VAL32);
    ASSERT_EQ( bp->is_taken(PC), 0);

    bp->update( false, PC, NO_VAL32);
    ASSERT_EQ( bp->is_taken(PC), 0);

    bp->update( false, PC, NO_VAL32);
    ASSERT_EQ( bp->is_taken(PC), 0);

    bp->update( false, PC, NO_VAL32);
    ASSERT_EQ( bp->is_taken(PC), 0);

    // Teaching again
    bp->update( true, PC, target);
    ASSERT_EQ( bp->is_taken(PC), 0);

    bp->update( true, PC, target);
    ASSERT_EQ( bp->is_taken(PC), 1);
    ASSERT_EQ( bp->get_target(PC), target);
}


TEST( Overload, LRU)
{
    BPFactory bp_factory;
    auto bp = bp_factory.create( "dynamic_two_bit", 128, 16);

    const Addr PCconst = 16;
    Addr target = 48;

    // Trying to make it forget the PCconst
    for ( int i = 0; i < 1000; i++)
    {
        bp->update( false, i, NO_VAL32);
        if ( !( i % 50))
            bp->update( true, PCconst, target);
    }

    // Checking some random PC and PCConst
    Addr PC = 4;
    ASSERT_EQ( bp->is_taken(PC), 0);
    ASSERT_EQ( bp->is_taken(PCconst), 1);
    ASSERT_EQ( bp->get_target(PCconst), target);
}

int main( int argc, char* argv[])
{
    ::testing::InitGoogleTest( &argc, argv);
    ::testing::FLAGS_gtest_death_test_style = "threadsafe";
    return RUN_ALL_TESTS();
}
