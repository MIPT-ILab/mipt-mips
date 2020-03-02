/**
 * Branch Prediction unit tests
 * @author George Korepanov, Yan Logovsky, Rustem Yunusov, Pavel Kryukov
 * Copyright 2017-2019 MIPT-MIPS
 */

#include <catch.hpp>
#include <infra/replacement/cache_replacement.h>
#include <modules/fetch/bpu/bpu.h>

#include <cassert>
#include <cstdlib>
#include <sstream>

TEST_CASE( "Initialization: WrongParameters")
{
    // Check failing with wrong input values
    CHECK_THROWS_AS( BaseBP::create_bp( "saturating_three_bits", "LRU", 128, 16, 32), BPInvalidMode);
    CHECK_THROWS_AS( BaseBP::create_bp( "saturating_two_bits", "URL", 128, 16, 32), CacheReplacementException);
    CHECK_THROWS_AS( BaseBP::create_bp( "saturating_two_bits", "LRU", 100, 20, 32), BPInvalidMode);
}

static auto to_string( const BPInterface& info)
{
    std::ostringstream oss;
    oss << info;
    return oss.str();
}

TEST_CASE( "BPInterface: dump")
{
    CHECK( to_string( BPInterface( 0x28, false, 0x30, false)) == "{ pc=0x28, target=0x30, NT, miss }");
}

TEST_CASE( "Static, all branches not taken")
{
    auto bp = BaseBP::create_bp( "always_not_taken", "LRU", 128, 16, 32);

    Addr PC = 28;
    Addr target = 12;

    bp->update( BPInterface( PC, false, target, false));
    bp->update( BPInterface( PC, true, target, true));
    bp->update( BPInterface( PC, true, target, true));

    CHECK_FALSE( bp->is_taken(PC) );
    CHECK( bp->get_target(PC) == PC + 4);
}

TEST_CASE( "Static, all branches taken")
{
    auto bp = BaseBP::create_bp( "always_taken", "LRU", 128, 16, 32);

    Addr PC = 28;
    Addr target = 12;

    bp->update( BPInterface( PC, true, target, false));
    CHECK( bp->is_taken(PC) );
    CHECK( bp->get_target(PC) == target);
}

TEST_CASE( "Backward only branches taken")
{
    auto bp = BaseBP::create_bp( "backward_jumps", "LRU", 128, 16, 32);

    Addr PC = 28;
    Addr target = 12;

    bp->update( BPInterface( PC, true, target, false));
    CHECK( bp->is_taken(PC) );
    CHECK( bp->get_target(PC) == target);
}

TEST_CASE( "Backward only branches taken in case of forward jump")
{
    auto bp = BaseBP::create_bp( "backward_jumps", "LRU", 128, 16, 32);

    Addr PC = 28;
    Addr target = 36;

    bp->update( BPInterface( PC, true, target, false));
    CHECK_FALSE( bp->is_taken(PC) );
    CHECK( bp->get_target(PC) == PC + 4);
}

TEST_CASE( "One bit predictor")
{
    auto bp = BaseBP::create_bp( "saturating_one_bit", "LRU", 128, 16, 32);

    Addr PC = 28;
    Addr target = 12;

    bp->update( BPInterface( PC, true, target, false));
    CHECK( bp->is_taken(PC) );
    CHECK( bp->get_target(PC) == target);
}

TEST_CASE( "One bit predictor in case of changed target")
{
    auto bp = BaseBP::create_bp( "saturating_one_bit", "LRU", 128, 16, 32);

    Addr PC = 28;
    Addr target = 12;

    //learn
    bp->update( BPInterface( PC, true, target, false));
    //change the target
    target = 16;
    bp->update( BPInterface( PC, true, target, false));
    CHECK( bp->get_target(PC) == target);
}

TEST_CASE( "Two bit predictor, basic")
{
    auto bp = BaseBP::create_bp( "saturating_two_bits", "LRU", 128, 16, 32);

    Addr PC = 28;
    Addr target = 12;

    bp->update( BPInterface( PC, true, target, false));
    CHECK( bp->is_taken(PC) );
    CHECK( bp->get_target(PC) == target);
}

TEST_CASE( "Two bit predictor, advanced")
{
    auto bp = BaseBP::create_bp( "saturating_two_bits", "LRU", 128, 16, 32);

    Addr PC = 12;
    Addr target = 28;

    // Learn
    bp->update( BPInterface( PC, true, target, false));
    CHECK( bp->is_taken(PC) );
    CHECK( bp->get_target(PC) == target);

    bp->update( BPInterface( PC, true, target, true));
    CHECK( bp->is_taken(PC) );
    CHECK( bp->get_target(PC) == target);

    // "Over" - learning
    bp->update( BPInterface( PC, true, target, true));
    CHECK( bp->is_taken(PC) );
    CHECK( bp->get_target(PC) == target);

    bp->update( BPInterface( PC, true, target, true));
    CHECK( bp->is_taken(PC) );
    CHECK( bp->get_target(PC) == target);

    // Moderate "Un" - learning
    bp->update( BPInterface( PC, false, NO_VAL32, true));
    CHECK( bp->is_taken(PC) );
    CHECK( bp->get_target(PC) == NO_VAL32);

    // Strong "un" - learning
    bp->update( BPInterface( PC, false, NO_VAL32, true));
    bp->update( BPInterface( PC, false, NO_VAL32, true));
    bp->update( BPInterface( PC, false, NO_VAL32, true));
    CHECK_FALSE(bp->is_taken(PC));

    bp->update( BPInterface( PC, false, NO_VAL32, true));
    CHECK_FALSE(bp->is_taken(PC));

    bp->update( BPInterface( PC, false, NO_VAL32, true));
    CHECK_FALSE(bp->is_taken(PC));

    bp->update( BPInterface( PC, false, NO_VAL32, true));
    CHECK_FALSE(bp->is_taken(PC));

    // Learn again
    bp->update( BPInterface( PC, true, target, true));
    CHECK_FALSE(bp->is_taken(PC));

    bp->update( BPInterface( PC, true, target, true));
    CHECK( bp->is_taken(PC) );
    CHECK( bp->get_target(PC) == target);
}

static auto get_trained_adaptive_two_level_predictor( Addr PC, Addr target)
{
    auto bp = BaseBP::create_bp( "adaptive_two_levels", "LRU", 128, 16, 32);

    // Learn in sequence 001001001
    bp->update( BPInterface( PC, false, target, false));
    bp->update( BPInterface( PC, false, target, true));
    bp->update( BPInterface( PC, true, target, true));

    bp->update( BPInterface( PC, false, target, true));
    bp->update( BPInterface( PC, false, target, true));
    bp->update( BPInterface( PC, true, target, true));

    bp->update( BPInterface( PC, false, target, true));
    bp->update( BPInterface( PC, false, target, true));
    bp->update( BPInterface( PC, true, target, true));

    return bp;
}

TEST_CASE( "Adaptive two bit prediction")
{
    Addr PC = 12;
    Addr target = 28;
    auto bp = get_trained_adaptive_two_level_predictor( PC, target);

    //check prediction on 00 sequence
    bp->update( BPInterface( PC, false, target, false));
    bp->update( BPInterface( PC, false, target, true));
    CHECK( bp->is_taken(PC) );
    CHECK( bp->get_target(PC) == target);

    //check prediction on 01 sequence
    bp->update( BPInterface( PC, false, target, true));
    bp->update( BPInterface( PC, true, target, true));
    CHECK_FALSE( bp->is_taken(PC) );
    CHECK( bp->get_target(PC) == PC + 4);

    //check prediction on 10 sequence
    bp->update( BPInterface( PC, true, target, true));
    bp->update( BPInterface( PC, false, target, true));
    CHECK_FALSE( bp->is_taken(PC) );
    CHECK( bp->get_target(PC) == PC + 4);
}

TEST_CASE( "Adaptive two bit prediction in case of changed target")
{
    Addr PC = 12;
    Addr target = 28;
    auto bp = get_trained_adaptive_two_level_predictor( PC, target);

    // use different target
    target = 24;
    bp->update( BPInterface( PC, true, target, false));

    //check if the target was updated
    bp->update( BPInterface( PC, false, target, true));
    bp->update( BPInterface( PC, false, target, true));
    CHECK( bp->get_target(PC) == target);
}

TEST_CASE( "Cache Miss")
{
    auto bp = BaseBP::create_bp( "saturating_two_bits", "LRU", 128, 16, 32);

    // Check default cache miss behaviour
    Addr PC = 12;
    CHECK_FALSE( bp->is_hit( PC));
    CHECK_FALSE( bp->is_taken( PC));

    PC = 16;
    CHECK_FALSE( bp->is_hit( PC));
    CHECK_FALSE( bp->is_taken( PC));

    PC = 20;
    CHECK_FALSE( bp->is_hit( PC));
    CHECK_FALSE( bp->is_taken( PC));

    PC = 12;
    CHECK_FALSE( bp->is_hit( PC));
    CHECK_FALSE( bp->is_taken( PC));
}

TEST_CASE( "Overload: LRU")
{
    auto bp = BaseBP::create_bp( "saturating_two_bits", "LRU", 128, 16, 32);

    const Addr PCconst = 16;
    Addr target = 48;

    // Trying to make it forget the PCconst
    for ( int i = 0; i < 1000; i++)
    {
        bp->update( BPInterface( i, false, NO_VAL32, false));
        if ( i % 50 == 0)
            bp->update( BPInterface( PCconst, true, target, bp->is_hit( PCconst)));
    }

    // Checking some random PC and PCConst
    Addr PC = 4;
    CHECK_FALSE( bp->is_taken(PC) );
    CHECK( bp->is_taken(PCconst) );
    CHECK( bp->get_target(PCconst) == target);
}
