// generic C
#include <cassert>
#include <cstdlib>

// Catch2
#include <catch.hpp>

// MIPT-MIPS modules
#include "../bpu.h"


TEST_CASE( "Initialization: WrongParameters")
{
    BPFactory bp_factory;
    // Check failing with wrong input values
    CHECK_THROWS_AS( bp_factory.create( "dynamic_three_bit", 128, 16), BPInvalidMode);
    CHECK_THROWS_AS( bp_factory.create( "dynamic_two_bit", 100, 20), BPInvalidMode);
}

TEST_CASE( "HitAndMiss: Miss")
{
    BPFactory bp_factory;
    auto bp = bp_factory.create( "dynamic_two_bit", 128, 16);

    // Check default cache miss behaviour
    Addr PC = 12;
    CHECK( bp->is_taken(PC) == false);

    PC = 16;
    CHECK( bp->is_taken(PC) == false);

    PC = 20;
    CHECK( bp->is_taken(PC) == false);

    PC = 12;
    CHECK( bp->is_taken(PC) == false);
}

TEST_CASE( "Main: PredictingBits")
{
    BPFactory bp_factory;
    auto bp = bp_factory.create( "dynamic_two_bit", 128, 16);

    Addr PC = 12;
    Addr target = 28;

    // Learn
    bp->update( BPInterface( PC, true, target));
    CHECK( bp->is_taken(PC) == true);
    CHECK( bp->get_target(PC) == target);

    bp->update( BPInterface( PC, true, target));
    CHECK( bp->is_taken(PC) == true);
    CHECK( bp->get_target(PC) == target);

    // "Over" - learning
    bp->update( BPInterface( PC, true, target));
    CHECK( bp->is_taken(PC) == true);
    CHECK( bp->get_target(PC) == target);

    bp->update( BPInterface( PC, true, target));
    CHECK( bp->is_taken(PC) == true);
    CHECK( bp->get_target(PC) == target);

    // Moderate "Un" - learning
    bp->update( BPInterface( PC, false, NO_VAL32));
    CHECK( bp->is_taken(PC) == true);
    CHECK( bp->get_target(PC) == target);

    // Strong "un" - learning
    bp->update( BPInterface( PC, false, NO_VAL32));
    bp->update( BPInterface( PC, false, NO_VAL32));
    bp->update( BPInterface( PC, false, NO_VAL32));
    CHECK( bp->is_taken(PC) == false);

    bp->update( BPInterface( PC, false, NO_VAL32));
    CHECK( bp->is_taken(PC) == false);

    bp->update( BPInterface( PC, false, NO_VAL32));
    CHECK( bp->is_taken(PC) == false);

    bp->update( BPInterface( PC, false, NO_VAL32));
    CHECK( bp->is_taken(PC) == false);

    // Learn again
    bp->update( BPInterface( PC, true, target));
    CHECK( bp->is_taken(PC) == false);

    bp->update( BPInterface( PC, true, target));
    CHECK( bp->is_taken(PC) == true);
    CHECK( bp->get_target(PC) == target);
}


TEST_CASE( "Overload: LRU")
{
    BPFactory bp_factory;
    auto bp = bp_factory.create( "dynamic_two_bit", 128, 16);

    const Addr PCconst = 16;
    Addr target = 48;

    // Trying to make it forget the PCconst
    for ( int i = 0; i < 1000; i++)
    {
        bp->update( BPInterface( i, false, NO_VAL32));
        if ( i % 50 == 0)
            bp->update( BPInterface( PCconst, true, target));
    }

    // Checking some random PC and PCConst
    Addr PC = 4;
    CHECK( bp->is_taken(PC) == false);
    CHECK( bp->is_taken(PCconst) == true);
    CHECK( bp->get_target(PCconst) == target);
}

