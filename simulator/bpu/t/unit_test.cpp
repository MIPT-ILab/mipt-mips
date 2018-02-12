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
    ASSERT_EQ( bp->is_taken(PC), false);

    PC = 16;
    ASSERT_EQ( bp->is_taken(PC), false);

    PC = 20;
    ASSERT_EQ( bp->is_taken(PC), false);

    PC = 12;
    ASSERT_EQ( bp->is_taken(PC), false);
}

TEST( Main, PredictingBits)
{
    BPFactory bp_factory;
    auto bp = bp_factory.create( "dynamic_two_bit", 128, 16);

    BPInterface bp_update;
    Addr PC = 12;
    Addr target = 28;

    // Learn
    bp->bp_update_init( bp_update, true, PC, target);
    bp->update( bp_update);
    ASSERT_EQ( bp->is_taken(PC), true);
    ASSERT_EQ( bp->get_target(PC), target);

    bp->bp_update_init( bp_update, true, PC, target);
    bp->update( bp_update);
    ASSERT_EQ( bp->is_taken(PC), true);
    ASSERT_EQ( bp->get_target(PC), target);

    // "Over" - learning
    bp->bp_update_init( bp_update, true, PC, target);
    bp->update( bp_update);
    ASSERT_EQ( bp->is_taken(PC), true);
    ASSERT_EQ( bp->get_target(PC), target);

    bp->bp_update_init( bp_update, true, PC, target);
    bp->update( bp_update);
    ASSERT_EQ( bp->is_taken(PC), true);
    ASSERT_EQ( bp->get_target(PC), target);

    // Moderate "Un" - learning
    bp->bp_update_init( bp_update, false, PC, NO_VAL32);
    bp->update( bp_update);
    ASSERT_EQ( bp->is_taken(PC), true);
    ASSERT_EQ( bp->get_target(PC), target);

    // Strong "un" - learning
    bp->bp_update_init( bp_update, false, PC, NO_VAL32);
    bp->update( bp_update);
    bp->update( bp_update);
    bp->update( bp_update);
    ASSERT_EQ( bp->is_taken(PC), false);

    bp->bp_update_init( bp_update, false, PC, NO_VAL32);
    bp->update( bp_update);
    ASSERT_EQ( bp->is_taken(PC), false);

    bp->bp_update_init( bp_update, false, PC, NO_VAL32);
    bp->update( bp_update);
    ASSERT_EQ( bp->is_taken(PC), false);

    bp->bp_update_init( bp_update, false, PC, NO_VAL32);
    bp->update( bp_update);
    ASSERT_EQ( bp->is_taken(PC), false);

    // Learn again
    bp->bp_update_init( bp_update, true, PC, target);
    bp->update( bp_update);
    ASSERT_EQ( bp->is_taken(PC), false);

    bp->bp_update_init( bp_update, true, PC, target);
    bp->update( bp_update);
    ASSERT_EQ( bp->is_taken(PC), true);
    ASSERT_EQ( bp->get_target(PC), target);
}


TEST( Overload, LRU)
{
    BPFactory bp_factory;
    auto bp = bp_factory.create( "dynamic_two_bit", 128, 16);
    BPInterface bp_update;

    const Addr PCconst = 16;
    Addr target = 48;

    // Trying to make it forget the PCconst
    for ( int i = 0; i < 1000; i++)
    {
        bp->bp_update_init( bp_update, false, i, NO_VAL32);
        bp->update( bp_update);
        if ( i % 50 == 0) 
        {
            bp->bp_update_init( bp_update, true, PCconst, target);
            bp->update( bp_update);
        }
    }

    // Checking some random PC and PCConst
    Addr PC = 4;
    ASSERT_EQ( bp->is_taken(PC), false);
    ASSERT_EQ( bp->is_taken(PCconst), true);
    ASSERT_EQ( bp->get_target(PCconst), target);
}

int main( int argc, char* argv[])
{
    ::testing::InitGoogleTest( &argc, argv);
    ::testing::FLAGS_gtest_death_test_style = "threadsafe";
    return RUN_ALL_TESTS();
}
