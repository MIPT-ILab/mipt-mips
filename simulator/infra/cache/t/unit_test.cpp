/**
 * Tests for CacheTagArray
 * @author Oleg Ladin, Denis Los
 */

#include <catch.hpp>

// Module
#include "../cache_tag_array.h"

#include <infra/types.h>

#include <fstream>
#include <map>
#include <vector>

static const uint32 LINE_SIZE = 4; // why not 32?

static const uint32 cache_size = 4;
static const uint32 cache_ways = 4;
static const uint32 cache_line_size = 1;
static const uint32 addr_size_in_bits = 32;

TEST_CASE( "pass_wrong_arguments: Pass_Wrong_Arguments_To_CacheTagArraySizeCheck")
{
    // size_in_bytes = 128
    // ways = 0
    // line_size = 4
    // addr_size_in_bits = 32
    CHECK_THROWS_AS( CacheTagArraySizeCheck( 128, 0, 4, 32), CacheTagArrayInvalidSizeException);

    // size_in_bytes = 0
    // ways = 16
    // line_size = 4
    // addr_size_in_bits = 32
    CHECK_THROWS_AS( CacheTagArraySizeCheck( 0, 16, 4, 32), CacheTagArrayInvalidSizeException);

    // size_in_bytes = 128
    // ways = 16
    // line_size = 0
    // addr_size_in_bits = 32
    CHECK_THROWS_AS( CacheTagArraySizeCheck( 128, 16, 0, 32), CacheTagArrayInvalidSizeException);

    // size_in_bytes = 128
    // ways = 16
    // line_size = 4
    // addr_size_in_bits = 0
    CHECK_THROWS_AS( CacheTagArraySizeCheck( 128, 16, 4, 0), CacheTagArrayInvalidSizeException);

    // size_in_bytes = 0
    // ways = 0
    // line_size = 0
    // addr_size_in_bits = 0
    CHECK_THROWS_AS( CacheTagArraySizeCheck( 0, 0, 0, 0), CacheTagArrayInvalidSizeException);

    // size_in_bytes is power of 2,
    // but the number of ways is not
    CHECK_THROWS_AS( CacheTagArraySizeCheck( 64, 9, 4, 32), CacheTagArrayInvalidSizeException);

    // the number of ways is power of 2,
    // but size_in_bytes is not
    CHECK_THROWS_AS( CacheTagArraySizeCheck( 500, 16, 4, 32), CacheTagArrayInvalidSizeException);

    // line_size is not power of 2
    CHECK_THROWS_AS( CacheTagArraySizeCheck( 512, 16, 12, 32), CacheTagArrayInvalidSizeException);

    // address is 48 bits
    CHECK_THROWS_AS( CacheTagArraySizeCheck( 128, 4, 4, 48), CacheTagArrayInvalidSizeException);

    // too small cache
    CHECK_THROWS_AS( CacheTagArraySizeCheck( 8, 4, 4, 32), CacheTagArrayInvalidSizeException);
}

TEST_CASE( "Check always_hit CacheTagArrayCache model")
{
    std::unique_ptr<CacheTagArray> test_tags = create_cache_tag_array( cache_size, cache_ways, cache_line_size, addr_size_in_bits, "always_hit");
    CHECK( test_tags->lookup( 1) == true);
}

TEST_CASE( "Check infinite CacheTagArrayCache model ( fully-associative)")
{
    std::unique_ptr<CacheTagArray> test_tags = create_cache_tag_array( cache_size, cache_ways, cache_line_size, addr_size_in_bits, "infinite");

    //make sure that the only set will overflow so cache size will double up
    for ( uint32 i = 0; i < cache_ways + 1; i++)
        test_tags->write( i);
    //make sure that there was no replacement
    for ( uint32 i = 0; i < cache_ways + 1; i++)
        CHECK( test_tags->lookup( i) == true);

    CHECK( test_tags->size_in_bytes == 2 * cache_size);
    CHECK( test_tags->ways == 2 * cache_ways);
}

TEST_CASE( "Check infinite CacheTagArray model with multiple sets")
{
    std::unique_ptr<CacheTagArray> test_tags = create_cache_tag_array( cache_size * 4, cache_ways, cache_line_size, addr_size_in_bits, "infinite");

    //make sure that addresses have same sets
    for ( uint32 i = 0; i < cache_ways + 1; i++)
    {
        CHECK ( test_tags->set( i * 0x10000000) == 0);
        test_tags->write( i * 0x10000000);
    }
    //make sure that there was no replacement
    for ( uint32 i = 0; i < cache_ways + 1; i++)
        CHECK( test_tags->lookup( i * 0x10000000) == true);

    CHECK( test_tags->size_in_bytes == cache_size * 4 * 2);
    CHECK( test_tags->ways == 2 * cache_ways);
}



