// generic C
#include <cassert>
#include <cstdlib>

// Catch2
#include <catch.hpp>

// MIPT-MIPS modules
#include "../bpu.h"

TEST_CASE( "Initialization: WrongParameters")
{
    // Check failing with wrong input values
    CHECK_THROWS_AS( BaseBP::create_bp( "saturating_three_bits", 128, 16), BPInvalidMode);
    CHECK_THROWS_AS( BaseBP::create_bp( "saturating_two_bits", 100, 20), BPInvalidMode);
}

TEST_CASE( "Static, all branches not taken")
{
    auto bp = BaseBP::create_bp( "always_not_taken", 128, 16);

    Addr PC = 28;
    Addr target = 12;

    bp->update( BPInterface( PC, false, target));
    bp->update( BPInterface( PC, true, target));
    bp->update( BPInterface( PC, true, target));

    CHECK_FALSE( bp->is_taken(PC) );
    CHECK( bp->get_target(PC) == PC + 4);
}

TEST_CASE( "Static, all branches taken")
{
    auto bp = BaseBP::create_bp( "always_taken", 128, 16);

    Addr PC = 28;
    Addr target = 12;

    bp->update( BPInterface( PC, true, target));
    CHECK( bp->is_taken(PC) );
    CHECK( bp->get_target(PC) == target);
}

TEST_CASE( "One bit predictor")
{
    auto bp = BaseBP::create_bp( "saturating_one_bit", 128, 16);

    Addr PC = 28;
    Addr target = 12;
    
    bp->update( BPInterface( PC, true, target));
    CHECK( bp->is_taken(PC) );
    CHECK( bp->get_target(PC) == target);
}

TEST_CASE( "Two bit predictor, basic")
{
    auto bp = BaseBP::create_bp( "saturating_two_bits", 128, 16);

    Addr PC = 28;
    Addr target = 12;
    
    bp->update( BPInterface( PC, true, target));
    CHECK( bp->is_taken(PC) );
    CHECK( bp->get_target(PC) == target);
}

TEST_CASE( "Two bit predictor, advanced")
{
    auto bp = BaseBP::create_bp( "saturating_two_bits", 128, 16);

    Addr PC = 12;
    Addr target = 28;

    // Learn
    bp->update( BPInterface( PC, true, target));
    CHECK( bp->is_taken(PC) );
    CHECK( bp->get_target(PC) == target);

    bp->update( BPInterface( PC, true, target));
    CHECK( bp->is_taken(PC) );
    CHECK( bp->get_target(PC) == target);

    // "Over" - learning
    bp->update( BPInterface( PC, true, target));
    CHECK( bp->is_taken(PC) );
    CHECK( bp->get_target(PC) == target);

    bp->update( BPInterface( PC, true, target));
    CHECK( bp->is_taken(PC) );
    CHECK( bp->get_target(PC) == target);

    // Moderate "Un" - learning
    bp->update( BPInterface( PC, false, NO_VAL32));
    CHECK( bp->is_taken(PC) );
    CHECK( bp->get_target(PC) == target);

    // Strong "un" - learning
    bp->update( BPInterface( PC, false, NO_VAL32));
    bp->update( BPInterface( PC, false, NO_VAL32));
    bp->update( BPInterface( PC, false, NO_VAL32));
    CHECK_FALSE(bp->is_taken(PC));

    bp->update( BPInterface( PC, false, NO_VAL32));
    CHECK_FALSE(bp->is_taken(PC));

    bp->update( BPInterface( PC, false, NO_VAL32));
    CHECK_FALSE(bp->is_taken(PC));

    bp->update( BPInterface( PC, false, NO_VAL32));
    CHECK_FALSE(bp->is_taken(PC));

    // Learn again
    bp->update( BPInterface( PC, true, target));
    CHECK_FALSE(bp->is_taken(PC));

    bp->update( BPInterface( PC, true, target));
    CHECK( bp->is_taken(PC) );
    CHECK( bp->get_target(PC) == target);
}

TEST_CASE( "Adaptive two bit prediction")
{

}

TEST_CASE( "Cache Miss")
{
    auto bp = BaseBP::create_bp( "saturating_two_bits", 128, 16);

    // Check default cache miss behaviour
    Addr PC = 12;
    CHECK_FALSE(bp->is_taken(PC));

    PC = 16;
    CHECK_FALSE(bp->is_taken(PC));

    PC = 20;
    CHECK_FALSE(bp->is_taken(PC));

    PC = 12;
    CHECK_FALSE(bp->is_taken(PC));
}

TEST_CASE( "Overload: LRU")
{
    auto bp = BaseBP::create_bp( "saturating_two_bits", 128, 16);

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
    CHECK_FALSE( bp->is_taken(PC) );
    CHECK( bp->is_taken(PCconst) );
    CHECK( bp->get_target(PCconst) == target);
}
