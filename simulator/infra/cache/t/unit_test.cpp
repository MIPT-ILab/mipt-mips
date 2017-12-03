/**
 * Unit tests for CacheTagArray
 * @author Denis Los 
 */

// Google Test Library
#include <gtest/gtest.h> 

// Modules
#include "../LRUTagCache.h"
#include "../cache_tag_array.h"


#include <infra/types.h>


#define GTEST_ASSERT_NO_DEATH( statement)                        \
    ASSERT_EXIT( { statement}::exit( EXIT_SUCCESS),              \
                 ::testing::ExitedWithCode( EXIT_SUCCESS), "");  \
    statement




TEST( pass_wrong_arguments, Pass_Wrong_Arguments_To_CacheTagArray)
{
    // size_in_bytes = 128
    // ways = 0
    // line_size = 4
    // addr_size_in_bits = 32
    ASSERT_EXIT( CacheTagArray cache( 128, 0, 4, 32),
                 ::testing::ExitedWithCode( EXIT_FAILURE),
                 "ERROR.*");

    // size_in_bytes = 0
    // ways = 16
    // line_size = 4
    // addr_size_in_bits = 32
    ASSERT_EXIT( CacheTagArray cache( 0, 16, 4, 32),
                 ::testing::ExitedWithCode( EXIT_FAILURE),
                 "ERROR.*");

    // size_in_bytes = 128
    // ways = 16
    // line_size = 0
    // addr_size_in_bits = 32
    ASSERT_EXIT( CacheTagArray cache( 128, 16, 0, 32),
                 ::testing::ExitedWithCode( EXIT_FAILURE),
                 "ERROR.*");

    // size_in_bytes = 128
    // ways = 16
    // line_size = 4
    // addr_size_in_bits = 0
    ASSERT_EXIT( CacheTagArray cache( 128, 16, 4, 0),
                 ::testing::ExitedWithCode( EXIT_FAILURE),
                 "ERROR.*");

    // size_in_bytes = 0
    // ways = 0
    // line_size = 0
    // addr_size_in_bits = 0
    ASSERT_EXIT( CacheTagArray cache( 0, 0, 0, 0),
                 ::testing::ExitedWithCode( EXIT_FAILURE),
                 "ERROR.*");

    // size_in_bytes is power of 2, 
    // but the number of ways is not
    ASSERT_EXIT( CacheTagArray cache( 64, 9),
                 ::testing::ExitedWithCode( EXIT_FAILURE),
                 "ERROR.*");

    // the number of ways is power of 2,
    // but size_in_bytes is not
    ASSERT_EXIT( CacheTagArray cache( 500, 16),
                 ::testing::ExitedWithCode( EXIT_FAILURE),
                 "ERROR.*");

    // line_size is not power of 2
    ASSERT_EXIT( CacheTagArray cache( 512, 16, 12),
                 ::testing::ExitedWithCode( EXIT_FAILURE),
                 "ERROR.*");         
}



TEST( check_lru_update_and_find, Check_LRU_Full_Update_And_Find)
{
    const uint32 CAPACITY = 128;
    LRUTagCache<Addr> cache( CAPACITY);

    std::pair<bool, uint32> result;

    // try to find an element in the empty cache
    GTEST_ASSERT_NO_DEATH( result = cache.find( 10););
    ASSERT_FALSE( result.first);

    
    Addr addr = 0;
    for ( uint32 i = 0; i < CAPACITY; i++)
    {
        GTEST_ASSERT_NO_DEATH( cache.update( addr););
        GTEST_ASSERT_NO_DEATH( result = cache.find( addr););
        ASSERT_TRUE( result.first);
        ASSERT_EQ( result.second, i);
        addr++;
    }

    // make (Addr = 1; way = 1) the least recently used element  
    GTEST_ASSERT_NO_DEATH( cache.update( 0););
    
    // add new element with Addr = CAPACITY and check lru
    GTEST_ASSERT_NO_DEATH( cache.update( addr););
    GTEST_ASSERT_NO_DEATH( result = cache.find( addr););
    ASSERT_TRUE( result.first);
    ASSERT_EQ( result.second, 1);
}



TEST( check_method_get_capacity, Check_Method_Get_Capacity)
{
    const uint32 CAPACITY = 512;
    LRUTagCache<Addr> cache( CAPACITY);

    ASSERT_EQ( cache.get_capacity(), CAPACITY);

    const Addr TEST_ADDR = 12;
    GTEST_ASSERT_NO_DEATH( cache.update( TEST_ADDR););
    ASSERT_EQ( cache.get_capacity(), CAPACITY);
}



TEST( check_size_and_empty, Check_Methods_Size_And_Empty)
{
    const uint32 CAPACITY = 213;
    LRUTagCache<Addr> cache( CAPACITY);

    ASSERT_EQ( cache.size(), 0);
    ASSERT_TRUE( cache.empty());

    Addr addr = 0;
    for ( uint32 i = 0; i < CAPACITY; i++)
    {
        GTEST_ASSERT_NO_DEATH( cache.update( addr););
        ASSERT_EQ( cache.size(), i + 1);
        ASSERT_FALSE( cache.empty());
        addr++;
    }

    // Put element with Addr = CAPACITY in the cache
    GTEST_ASSERT_NO_DEATH( cache.update( addr););
    ASSERT_EQ( cache.size(), CAPACITY);
    ASSERT_FALSE( cache.empty());
}




int main( int argc, char** argv)
{
    ::testing::InitGoogleTest( &argc, argv);
    ::testing::FLAGS_gtest_death_test_style = "threadsafe";
    return RUN_ALL_TESTS();
}