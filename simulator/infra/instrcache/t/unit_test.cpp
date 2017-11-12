/*
* This is an open source non-commercial project. Dear PVS-Studio, please check it.
* PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
*/
/**
 * A cache for decoded instructions in functional simulator
 * @author Denis Los
*/

// Google test library
#include <gtest/gtest.h>


// Modules
#include "../instr_cache.h"
#include "../LRUCache.h"


#include <infra/types.h>
#include <mips/mips_instr.h>




TEST( update_and_find_int, Update_Find_And_Check_Using_Int)
{
    InstrCache<uint32> cache{};

    const Addr PC = 0x401c04;
    const uint32 test_number = 0x103abf9;

    cache.update( PC, test_number);
    auto it = cache.find( PC);

    ASSERT_TRUE( it != cache.end());
    ASSERT_EQ( it -> first, PC);
    ASSERT_EQ( (*it -> second).second, test_number);
}


TEST( update_and_find, Update_Find_And_Check)
{
    InstrCache<FuncInstr> instr_cache{};

    const uint32 instr_bytes = 0x3c010400;
    const Addr PC = 0x401c04;
    const FuncInstr instr( instr_bytes, PC);

    instr_cache.update( PC, instr);
    ASSERT_TRUE( instr_cache.find( PC) != instr_cache.end());
} 


TEST( check_method_erase, Check_Method_Erase)
{
    InstrCache<FuncInstr> instr_cache{};

    const uint32 instr_bytes = 0x3c010400;
    const Addr PC = 0x401c04;
    const FuncInstr instr( instr_bytes, PC);

    instr_cache.update( PC, instr);
    instr_cache.erase( PC);

    ASSERT_TRUE( instr_cache.find( PC) == instr_cache.end());
}


TEST( check_method_empty, Check_Method_Empty)
{
    InstrCache<FuncInstr> instr_cache{};

    const uint32 instr_bytes = 0x2484ae10;
    const Addr PC = 0x400d05;
    const FuncInstr instr( instr_bytes, PC);

    ASSERT_TRUE( instr_cache.empty());
    instr_cache.update( PC, instr);
    ASSERT_FALSE( instr_cache.empty());
}


TEST( check_method_size, Check_Method_Size)
{
    InstrCache<FuncInstr> instr_cache{};

    uint32 instr_bytes = 0x2484ae10;
    Addr PC = 0x30ae17;
    const std::size_t SIZE = InstrCache<FuncInstr> :: CAPACITY / 12;

    for ( std::size_t i = 0; i < SIZE; i++)
    {
        FuncInstr instr( instr_bytes++, PC);
        instr_cache.update( PC++, instr); 
    } 
    ASSERT_EQ( SIZE, instr_cache.size());

    instr_cache.erase( PC - 1);
    ASSERT_EQ( SIZE - 1, instr_cache.size());
}


TEST( exceed_capacity_and_test_lru, Add_More_Elements_Than_Capacity_And_Check)
{
    const std::size_t CAPACITY = InstrCache<std::size_t>::CAPACITY;

    LRUCache<std::size_t, std::size_t> cache(CAPACITY);

    for ( std::size_t i = 1; i <= CAPACITY; i++)
        cache.update( i, i); 
    cache.update( 1, 27);
    cache.update( CAPACITY / 2, CAPACITY / 4);

    cache.update( CAPACITY + 1, CAPACITY + 1);

    ASSERT_EQ( cache.size(), CAPACITY);
    ASSERT_FALSE( cache.empty());
    ASSERT_TRUE( cache.find( 2) == cache.end());
} 



int main( int argc, char** argv)
{
    ::testing::InitGoogleTest( &argc, argv);
    ::testing::FLAGS_gtest_death_test_style = "threadsafe";
    return RUN_ALL_TESTS();
}