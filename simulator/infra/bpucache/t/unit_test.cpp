/**
 * Unit tests for BPU cache
 * @author Denis Los 
 */

// Google Test Library
#include <gtest/gtest.h> 

// Modules
#include "../LRUTagCache.h"
#include "../bpucache.h"


#include <infra/types.h>


#define GTEST_ASSERT_NO_DEATH( statement)                        \
    ASSERT_EXIT( { statement}::exit( EXIT_SUCCESS),              \
                 ::testing::ExitedWithCode( EXIT_SUCCESS), "");  \
    statement




TEST( pass_wrong_arguments, Pass_Wrong_Arguments_To_BPUCache)
{
    // size_in_entries = 128
    // ways = 0
    ASSERT_EXIT( BPUCache cache = BPUCache( 128, 0),
                 ::testing::ExitedWithCode( EXIT_FAILURE),
                 "ERROR.*");

    // size_in_entries = 0
    // ways = 16             
    ASSERT_EXIT( BPUCache cache = BPUCache( 0, 16),
                 ::testing::ExitedWithCode( EXIT_FAILURE),
                 "ERROR.*");

    // size_in_entries = 0
    // ways = 0
    ASSERT_EXIT( BPUCache cache = BPUCache( 0, 0),
                 ::testing::ExitedWithCode( EXIT_FAILURE),
                 "ERROR.*");

    // size_in_entries is power of 2, 
    // but the number of ways is not
    ASSERT_EXIT( BPUCache cache = BPUCache( 32, 9),
                 ::testing::ExitedWithCode( EXIT_FAILURE),
                 "ERROR.*");

    // the number of ways is power of 2,
    // but size_in_entries is not
    ASSERT_EXIT( BPUCache cache = BPUCache( 5, 16),
                 ::testing::ExitedWithCode( EXIT_FAILURE),
                 "ERROR.*");          
}



TEST( check_lru_update_and_find, Check_LRU_Full_Update_And_Find)
{
    const uint32 CAPACITY = 128;
    LRUTagCache<Addr, uint32> cache( CAPACITY);

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
    LRUTagCache<Addr, uint32> cache( CAPACITY);

    ASSERT_EQ( cache.get_capacity(), CAPACITY);

    const Addr TEST_ADDR = 12;
    GTEST_ASSERT_NO_DEATH( cache.update( TEST_ADDR););
    ASSERT_EQ( cache.get_capacity(), CAPACITY);
}



TEST( check_size_and_empty, Check_Methods_Size_And_Empty)
{
    const uint32 CAPACITY = 213;
    LRUTagCache<Addr, uint32> cache( CAPACITY);

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