/**
 * Tests for CacheTagArray
 * @author Oleg Ladin, Denis Los
 */

#include <catch.hpp>

#include <infra/cache/cache_tag_array.h>
#include <infra/replacement/cache_replacement.h>
#include <infra/types.h>

#include <fstream>
#include <map>
#include <vector>

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
    CHECK_THROWS_AS( CacheTagArray::create( "LRU", 128, 0, 4, 32), CacheTagArrayInvalidSizeException);

    // size_in_bytes = 0
    // ways = 16
    // line_size = 4
    // addr_size_in_bits = 32
    CHECK_THROWS_AS( CacheTagArray::create( "LRU", 0, 16, 4, 32), CacheTagArrayInvalidSizeException);

    // size_in_bytes = 128
    // ways = 16
    // line_size = 0
    // addr_size_in_bits = 32
    CHECK_THROWS_AS( CacheTagArray::create( "LRU", 128, 16, 0, 32), CacheTagArrayInvalidSizeException);

    // size_in_bytes = 128
    // ways = 16
    // line_size = 4
    // addr_size_in_bits = 0
    CHECK_THROWS_AS( CacheTagArray::create( "LRU", 128, 16, 4, 0), CacheTagArrayInvalidSizeException);

    // size_in_bytes = 0
    // ways = 0
    // line_size = 0
    // addr_size_in_bits = 0

    CHECK_THROWS_AS( CacheTagArray::create( "LRU", 0, 0, 0, 0), CacheTagArrayInvalidSizeException);

    // size_in_bytes is power of 2,
    // but the number of ways is not
    CHECK_THROWS_AS( CacheTagArray::create( "LRU", 64, 9, 4, 32), CacheTagArrayInvalidSizeException);

    // the number of ways is power of 2,
    // but size_in_bytes is not
    CHECK_THROWS_AS( CacheTagArray::create( "LRU", 500, 16, 4, 32), CacheTagArrayInvalidSizeException);

    // line_size is not power of 2
    CHECK_THROWS_AS( CacheTagArray::create( "LRU", 512, 16, 12, 32), CacheTagArrayInvalidSizeException);

    // address is 48 bits
    CHECK_THROWS_AS( CacheTagArray::create( "LRU", 128, 4, 4, 48), CacheTagArrayInvalidSizeException);

    // too small cache
    CHECK_THROWS_AS( CacheTagArray::create( "LRU", 8, 4, 4, 32), CacheTagArrayInvalidSizeException);

    // wrong mode
    CHECK_THROWS_AS( CacheTagArray::create( "abracadabra", 4096, 16, 64, 32), CacheReplacementException);
}

TEST_CASE( "Check always_hit CacheTagArrayCache model")
{
    auto test_tags = CacheTagArray::create( "always_hit", cache_size, cache_ways, cache_line_size, addr_size_in_bits);
    test_tags->write( 0); // noop
    for ( uint32 i = 0; i < cache_ways * 5; i++) {
        CHECK( test_tags->lookup( i));
        CHECK( test_tags->set( i) == 0);
        CHECK( test_tags->tag( i) == i);
    }
}

TEST_CASE( "Check infinite CacheTagArrayCache model ( fully-associative)")
{
    auto test_tags = CacheTagArray::create( "infinite", cache_size, cache_ways, cache_line_size, addr_size_in_bits);

    for ( uint32 i = 0; i < cache_ways * 5; i++)
        test_tags->write( i);

    //make sure that there was no replacement
    for ( uint32 i = 0; i < cache_ways * 5; i++) {
        CHECK( test_tags->lookup( i));
        CHECK( test_tags->set( i) == 0);
        CHECK( test_tags->tag( i) == i);
    }
}

TEST_CASE( "Infinite cache: WaW case")
{
    auto test_tags = CacheTagArray::create( "infinite", cache_size, cache_ways, cache_line_size, addr_size_in_bits);
    auto result = test_tags->write( 0x1000);
    CHECK( test_tags->read_no_touch( 0x1000).first);
    CHECK( test_tags->write( 0x1000) == result);
    CHECK( test_tags->read_no_touch( 0x1000).first);
    CHECK( test_tags->read_no_touch( 0x1000).second == result);
}

TEST_CASE( "Check infinite CacheTagArray model with multiple sets")
{
    auto test_tags = CacheTagArray::create( "infinite", cache_size * 4, cache_ways, cache_line_size, addr_size_in_bits);

    //make sure that addresses have same sets
    for ( uint32 i = 0; i < cache_ways + 1; i++)
    {
        CHECK ( test_tags->set( i * 0x10000000) == 0);
        test_tags->write( i * 0x10000000);
    }
    //make sure that there was no replacement
    for ( uint32 i = 0; i < cache_ways + 1; i++)
        CHECK( test_tags->lookup( i * 0x10000000) == true);
}
