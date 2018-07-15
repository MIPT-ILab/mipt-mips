/**
 * Tests for CacheTagArray
 * @author Oleg Ladin, Denis Los 
 */

// Google Test Library
#include <catch.hpp> 

// Module
#include "../cache_tag_array.h"

#include <infra/types.h>

#include <fstream>
#include <vector>

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
}

static const uint32 LINE_SIZE = 4; // why not 32?

static void test( std::ifstream& miss_rate_file, const std::vector<Addr>& values,
                  uint32 associativity, uint32 _size)
{
    CacheTagArray cta(_size, associativity, LINE_SIZE);

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
            cta.write( addr); // load to the
        }

    // hit and miss numbers are both needed 
    // because mem_trace file can be changed
    std::size_t sample_hit;
    std::size_t sample_miss;

    // read sample miss rates from miss_rate file
    // and check whether a file with sample miss rates has been corrupted
    miss_rate_file >> sample_miss;
    miss_rate_file >> sample_hit;

    // check whether sample hit and miss numbers
    // are equal to the evaluated ones 
    CHECK( hit == sample_hit);
    CHECK( miss == sample_miss);
}

TEST_CASE( "miss_rate_sim: Miss_Rate_Sim_Test")
{
    const std::string MEM_TRACE_FILENAME = TEST_DATA_PATH "mem_trace.txt";
    const std::string MISS_RATE_FILENAME = TEST_DATA_PATH "miss_rate.txt";

    // open and check mem_trace file
    std::ifstream mem_trace_file;
    mem_trace_file.open( MEM_TRACE_FILENAME, std::ifstream::in);
    CHECK( mem_trace_file.is_open());

    // open and check miss_rate file
    std::ifstream miss_rate_file;
    miss_rate_file.open( MISS_RATE_FILENAME, std::ifstream::in);
    CHECK( miss_rate_file.is_open());

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
    
    // test full-assotiative
    for ( auto cache_size : cache_sizes)
        test( miss_rate_file, values, 1024 * cache_size / LINE_SIZE, 1024 * cache_size);

    mem_trace_file.close();
    miss_rate_file.close();
}
