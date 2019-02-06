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

static const uint32 LINE_SIZE = 4; // why not 32?

struct CacheParameters {
    uint32 associativity;
    uint32 cache_size;
    bool operator<(const CacheParameters& rhs) const {
        if (associativity == rhs.associativity)
            return (cache_size < rhs.cache_size);
        
        return associativity < rhs.associativity;
    }
};

struct CacheResults {
    uint32 misses;
    uint32 hits;
};

static const std::map<CacheParameters, CacheResults> golden =
{   
    // ways, size, misses, hits
    {{ 1, 1 * 1024},  {390614, 728471}},
    {{ 1, 2 * 1024},  {306137, 812948}},
    {{ 1, 4 * 1024},  {242688, 876397}},
    {{ 1, 8 * 1024},  {183876, 935209}},
    {{ 1, 16 * 1024}, {142662, 976423}},
    {{ 1, 32 * 1024}, {100330, 1018755}},
    {{ 1, 64 * 1024}, {68760, 1050325}},
    {{ 1, 128 * 1024},{43114, 1075971}},
    {{ 1, 256 * 1024},{38295, 1080790}},
    {{ 1, 512 * 1024},{31134, 1087951}},
    {{ 1, 1024 * 1024}, {31064, 1088021}},
    {{ 2, 1 * 1024},  {358489, 760596}},
    {{ 2, 2 * 1024},  {266913, 852172}},
    {{ 2, 4 * 1024},  {201319, 917766}},
    {{ 2, 8 * 1024},  {161085, 958000}},
    {{ 2, 16 * 1024}, {123902, 995183}},
    {{ 2, 32 * 1024}, {88968, 1030117}},
    {{ 2, 64 * 1024}, {53685, 1065400}},
    {{ 2, 128 * 1024},{37244, 1081841}},
    {{ 2, 256 * 1024},{30626, 1088459}},
    {{ 2, 512 * 1024},{30596, 1088489}},
    {{ 2, 1024 * 1024}, {30400, 1088685}},
    {{ 4, 1 * 1024},  {343381, 775704}},
    {{ 4, 2 * 1024},  {257323, 861762}},
    {{ 4, 4 * 1024},  {188436, 930649}},
    {{ 4, 8 * 1024},  {149047, 970038}},
    {{ 4, 16 * 1024}, {119688, 999397}},
    {{ 4, 32 * 1024}, {86607, 1032478}},
    {{ 4, 64 * 1024}, {48190, 1070895}},
    {{ 4, 128 * 1024},{32202, 1086883}},
    {{ 4, 256 * 1024},{30597, 1088488}},
    {{ 4, 512 * 1024},{30400, 1088685}},
    {{ 4, 1024 * 1024}, {30400, 1088685}},
    {{ 8, 1 * 1024},  {340031, 779054}},
    {{ 8, 2 * 1024},  {253850, 865235}},
    {{ 8, 4 * 1024},  {185989, 933096}},
    {{ 8, 8 * 1024},  {146187, 972898}},
    {{ 8, 16 * 1024}, {117343, 1001742}},
    {{ 8, 32 * 1024}, {85325, 1033760}},
    {{ 8, 64 * 1024}, {45835, 1073250}},
    {{ 8, 128 * 1024},{30601, 1088484}},
    {{ 8, 256 * 1024},{30400, 1088685}},
    {{ 8, 512 * 1024},{30400, 1088685}},
    {{ 8, 1024 * 1024}, {30400, 1088685}},
    {{ 16, 1 * 1024},  {338084, 781001}},
    {{ 16, 2 * 1024},  {252963, 866122}},
    {{ 16, 4 * 1024},  {185231, 933854}},
    {{ 16, 8 * 1024},  {145383, 973702}},
    {{ 16, 16 * 1024}, {116731, 1002354}},
    {{ 16, 32 * 1024}, {84471, 1034614}},
    {{ 16, 64 * 1024}, {44084, 1075001}},
    {{ 16, 128 * 1024},{30463, 1088622}},
    {{ 16, 256 * 1024},{30400, 1088685}},
    {{ 16, 512 * 1024},{30400, 1088685}},
    {{ 16, 1024 * 1024}, {30400, 1088685}},
    // Full associativity
    {{ (1 * 1024 / LINE_SIZE), 1 * 1024},     {337333, 781752}},
    {{ (2 * 1024 / LINE_SIZE), 2 * 1024},     {250920, 868165}},
    {{ (4 * 1024 / LINE_SIZE), 4 * 1024},     {184816, 934269}},
    {{ (8 * 1024 / LINE_SIZE), 8 * 1024},     {145443, 973642}},
    {{ (16 * 1024 / LINE_SIZE), 16 * 1024},   {116112, 1002973}},
    {{ (32 * 1024 / LINE_SIZE), 32 * 1024},   {82732, 1036353}},
    {{ (64 * 1024 / LINE_SIZE), 64 * 1024},   {41512, 1077573}},
    {{ (128 * 1024 / LINE_SIZE), 128 * 1024}, {30400, 1088685}},
    {{ (256 * 1024 / LINE_SIZE), 256 * 1024}, {30400, 1088685}},
    {{ (512 * 1024 / LINE_SIZE), 512 * 1024}, {30400, 1088685}},
    {{ (1024 * 1024 / LINE_SIZE), 1024 * 1024}, {30400, 1088685}},
};

static void test( const std::vector<Addr>& values, uint32 associativity, uint32 _size)
{
    CacheTagArray cta(_size, associativity, LINE_SIZE);

    std::size_t hits = 0;
    std::size_t misses = 0;

    for ( const auto& addr : values)
        if ( cta.read( addr).first)
        {
            hits++;
        }
        else
        {
            misses++;
            cta.write( addr); // load to the
        }

    // check whether sample hit and miss numbers
    // are equal to the evaluated ones 
    CHECK( hits == golden.at({associativity, _size}).hits);
    CHECK( misses == golden.at({associativity, _size}).misses);
}

static std::vector<Addr> read_values()
{
    const std::string MEM_TRACE_FILENAME = TEST_DATA_PATH "mem_trace.txt";

    // open and check mem_trace file
    std::ifstream mem_trace_file;
    mem_trace_file.open( MEM_TRACE_FILENAME, std::ifstream::in);
    CHECK( mem_trace_file.is_open());
    
    Addr addr;
    std::vector<Addr> values;
    values.reserve(100000);
    while ( mem_trace_file >> std::hex >> addr)
        values.push_back(addr);

    mem_trace_file.close();

    return values;
}

TEST_CASE( "miss_rate_sim: Miss_Rate_Sim_Test")
{
    // Input data
    auto values = read_values();

    // test CacheTagArray on different parameters
    for ( uint32 cache_size : { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024 })
        for ( uint32 associativity : { 1, 2, 4, 8, 16, int(cache_size * 1024 / LINE_SIZE) })
            test( values, associativity, cache_size * 1024);
}
