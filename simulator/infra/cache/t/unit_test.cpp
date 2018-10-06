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

#define KILO * 1024

static const std::map<CacheParameters, CacheResults> golden =
{   
    // ways, size, misses, hits
    {{ 1, 1 KILO},  {390614, 728471}},
    {{ 1, 2 KILO},  {306137, 812948}},
    {{ 1, 4 KILO},  {242688, 876397}},
    {{ 1, 8 KILO},  {183876, 935209}},
    {{ 1, 16 KILO}, {142662, 976423}},
    {{ 1, 32 KILO}, {100330, 1018755}},
    {{ 1, 64 KILO}, {68760, 1050325}},
    {{ 1, 128 KILO},{43114, 1075971}},
    {{ 1, 256 KILO},{38295, 1080790}},
    {{ 1, 512 KILO},{31134, 1087951}},
    {{ 1, 1024 KILO}, {31064, 1088021}},
    {{ 2, 1 KILO},  {358489, 760596}},
    {{ 2, 2 KILO},  {266913, 852172}},
    {{ 2, 4 KILO},  {201319, 917766}},
    {{ 2, 8 KILO},  {161085, 958000}},
    {{ 2, 16 KILO}, {123902, 995183}},
    {{ 2, 32 KILO}, {88968, 1030117}},
    {{ 2, 64 KILO}, {53685, 1065400}},
    {{ 2, 128 KILO},{37244, 1081841}},
    {{ 2, 256 KILO},{30626, 1088459}},
    {{ 2, 512 KILO},{30596, 1088489}},
    {{ 2, 1024 KILO}, {30400, 1088685}},
    {{ 4, 1 KILO},  {343381, 775704}},
    {{ 4, 2 KILO},  {257323, 861762}},
    {{ 4, 4 KILO},  {188436, 930649}},
    {{ 4, 8 KILO},  {149047, 970038}},
    {{ 4, 16 KILO}, {119688, 999397}},
    {{ 4, 32 KILO}, {86607, 1032478}},
    {{ 4, 64 KILO}, {48190, 1070895}},
    {{ 4, 128 KILO},{32202, 1086883}},
    {{ 4, 256 KILO},{30597, 1088488}},
    {{ 4, 512 KILO},{30400, 1088685}},
    {{ 4, 1024 KILO}, {30400, 1088685}},
    {{ 8, 1 KILO},  {340031, 779054}},
    {{ 8, 2 KILO},  {253850, 865235}},
    {{ 8, 4 KILO},  {185989, 933096}},
    {{ 8, 8 KILO},  {146187, 972898}},
    {{ 8, 16 KILO}, {117343, 1001742}},
    {{ 8, 32 KILO}, {85325, 1033760}},
    {{ 8, 64 KILO}, {45835, 1073250}},
    {{ 8, 128 KILO},{30601, 1088484}},
    {{ 8, 256 KILO},{30400, 1088685}},
    {{ 8, 512 KILO},{30400, 1088685}},
    {{ 8, 1024 KILO}, {30400, 1088685}},
    {{ 16, 1 KILO},  {338084, 781001}},
    {{ 16, 2 KILO},  {252963, 866122}},
    {{ 16, 4 KILO},  {185231, 933854}},
    {{ 16, 8 KILO},  {145383, 973702}},
    {{ 16, 16 KILO}, {116731, 1002354}},
    {{ 16, 32 KILO}, {84471, 1034614}},
    {{ 16, 64 KILO}, {44084, 1075001}},
    {{ 16, 128 KILO},{30463, 1088622}},
    {{ 16, 256 KILO},{30400, 1088685}},
    {{ 16, 512 KILO},{30400, 1088685}},
    {{ 16, 1024 KILO}, {30400, 1088685}},
    // Full associativity
    {{ (1 KILO / LINE_SIZE), 1 KILO},     {337333, 781752}},
    {{ (2 KILO / LINE_SIZE), 2 KILO},     {250920, 868165}},
    {{ (4 KILO / LINE_SIZE), 4 KILO},     {184816, 934269}},
    {{ (8 KILO / LINE_SIZE), 8 KILO},     {145443, 973642}},
    {{ (16 KILO / LINE_SIZE), 16 KILO},   {116112, 1002973}},
    {{ (32 KILO / LINE_SIZE), 32 KILO},   {82732, 1036353}},
    {{ (64 KILO / LINE_SIZE), 64 KILO},   {41512, 1077573}},
    {{ (128 KILO / LINE_SIZE), 128 KILO}, {30400, 1088685}},
    {{ (256 KILO / LINE_SIZE), 256 KILO}, {30400, 1088685}},
    {{ (512 KILO / LINE_SIZE), 512 KILO}, {30400, 1088685}},
    {{ (1024 KILO / LINE_SIZE), 1024 KILO}, {30400, 1088685}},
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
    for ( uint32 cache_size : { 1 KILO, 2 KILO, 4 KILO, 8 KILO, 16 KILO, 32 KILO, 64 KILO, 128 KILO, 256 KILO, 512 KILO, 1024 KILO })
        for ( uint32 associativity : { 1, 2, 4, 8, 16, int(cache_size / LINE_SIZE) })
            test( values, associativity, cache_size);
}
