/**
 * Tests for CacheTagArray
 * @author Oleg Ladin, Denis Los 
 */

// Google Test Library
#include <gtest/gtest.h> 

// Module
#include "../cache_tag_array.h"

#include <infra/types.h>

#include <fstream>
#include <vector>

TEST( pass_wrong_arguments, Pass_Wrong_Arguments_To_CacheTagArraySizeCheck)
{
    // size_in_bytes = 128
    // ways = 0
    // line_size = 4
    // addr_size_in_bits = 32
    ASSERT_EXIT( CacheTagArraySizeCheck cache( 128, 0, 4, 32),
                 ::testing::ExitedWithCode( EXIT_FAILURE),
                 "ERROR.*");

    // size_in_bytes = 0
    // ways = 16
    // line_size = 4
    // addr_size_in_bits = 32
    ASSERT_EXIT( CacheTagArraySizeCheck cache( 0, 16, 4, 32),
                 ::testing::ExitedWithCode( EXIT_FAILURE),
                 "ERROR.*");

    // size_in_bytes = 128
    // ways = 16
    // line_size = 0
    // addr_size_in_bits = 32
    ASSERT_EXIT( CacheTagArraySizeCheck cache( 128, 16, 0, 32),
                 ::testing::ExitedWithCode( EXIT_FAILURE),
                 "ERROR.*");

    // size_in_bytes = 128
    // ways = 16
    // line_size = 4
    // addr_size_in_bits = 0
    ASSERT_EXIT( CacheTagArraySizeCheck cache( 128, 16, 4, 0),
                 ::testing::ExitedWithCode( EXIT_FAILURE),
                 "ERROR.*");

    // size_in_bytes = 0
    // ways = 0
    // line_size = 0
    // addr_size_in_bits = 0
    ASSERT_EXIT( CacheTagArraySizeCheck cache( 0, 0, 0, 0),
                 ::testing::ExitedWithCode( EXIT_FAILURE),
                 "ERROR.*");

    // size_in_bytes is power of 2, 
    // but the number of ways is not
    ASSERT_EXIT( CacheTagArraySizeCheck cache( 64, 9, 4, 32),
                 ::testing::ExitedWithCode( EXIT_FAILURE),
                 "ERROR.*");

    // the number of ways is power of 2,
    // but size_in_bytes is not
    ASSERT_EXIT( CacheTagArraySizeCheck cache( 500, 16, 4, 32),
                 ::testing::ExitedWithCode( EXIT_FAILURE),
                 "ERROR.*");

    // line_size is not power of 2
    ASSERT_EXIT( CacheTagArraySizeCheck cache( 512, 16, 12, 32),
                 ::testing::ExitedWithCode( EXIT_FAILURE),
                 "ERROR.*");         
}

static const uint32 LINE_SIZE = 4; // why not 32?

static void test( std::ifstream& miss_rate_file, const std::vector<Addr>& values,
                  uint32 associativity, uint32 cache_size)
{
    CacheTagArray cta( cache_size, associativity, LINE_SIZE);

    std::size_t hit = 0;
    std::size_t miss = 0;

    for ( const auto& addr : values)
        if ( cta.read( addr).first)
        {
            hit++;
        }
        else
        {
            miss++;
            cta.write( addr); // load to the cache
        }

    // hit and miss numbers are both needed 
    // because mem_trace file can be changed
    std::size_t sample_hit;
    std::size_t sample_miss;

    // read sample miss rates from miss_rate file
    // and check whether a file with sample miss rates has been corrupted
    ASSERT_TRUE( miss_rate_file >> sample_miss);
    ASSERT_TRUE( miss_rate_file >> sample_hit);

    // check whether sample hit and miss numbers
    // are equal to the evaluated ones 
    ASSERT_EQ( hit, sample_hit);
    ASSERT_EQ( miss, sample_miss);
}

TEST( miss_rate_sim, Miss_Rate_Sim_Test)
{
    const std::string MEM_TRACE_FILENAME = "mem_trace.txt";
    const std::string MISS_RATE_FILENAME = "miss_rate.txt";

    // open and check mem_trace file
    std::ifstream mem_trace_file;
    mem_trace_file.open( MEM_TRACE_FILENAME, std::ifstream::in);
    ASSERT_TRUE( mem_trace_file.is_open());

    // open and check miss_rate file
    std::ifstream miss_rate_file;
    miss_rate_file.open( MISS_RATE_FILENAME, std::ifstream::in);
    ASSERT_TRUE( miss_rate_file.is_open());

    // Cache parameters
    std::vector<uint32> associativities = { 1, 2, 4, 8, 16 };
    std::vector<uint32> cache_sizes = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024 };
   
    Addr addr;
    std::vector<uint32> values;
    values.reserve(100000);
    while ( mem_trace_file >> std::hex >> addr)
        values.push_back(addr);

    // test CacheTagArray on different parameters
    for ( auto associativity : associativities)
        for ( auto cache_size : cache_sizes)
            test( miss_rate_file, values, associativity, 1024 * cache_size);
    
    // test full-assotiative cache
    for ( auto cache_size : cache_sizes)
        test( miss_rate_file, values, 1024 * cache_size / LINE_SIZE, 1024 * cache_size);

    mem_trace_file.close();
    miss_rate_file.close();
}

int main( int argc, char** argv)
{
    ::testing::InitGoogleTest( &argc, argv);
    ::testing::FLAGS_gtest_death_test_style = "threadsafe";
    return RUN_ALL_TESTS();
}
