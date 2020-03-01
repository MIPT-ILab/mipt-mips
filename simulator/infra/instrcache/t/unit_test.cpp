/**
 * A cache for decoded instructions in functional simulator
 * @author Denis Los
*/

#include <catch.hpp>

// Modules
#include <infra/instrcache/instr_cache.h>
#include <infra/macro.h>
#include <infra/types.h>

class Dummy {
    const size_t value;
    const size_t pc;
public:
    explicit Dummy( size_t val, size_t pc) : value( val), pc( pc) {};
    bool is_same( const Dummy& rhs) const { return value == rhs.value && pc == rhs.pc; }
    bool operator==( const Dummy& rhs) const { return is_same(rhs); }
};

TEST_CASE( "update_and_find_int: Update_Find_And_Check_Using_Int")
{
    InstrCache<Addr, Dummy, 8192, 0, all_ones<Addr>()> cache{};

    const Addr PC = 0x401c04;
    const Dummy test_number( 0x103abf9, 0x401c04);

    cache.update( PC, test_number);
    auto result = cache.find( PC);

    CHECK( result.first);
    CHECK( result.second == test_number);
}

TEST_CASE( "check_method_size: Check_Method_Size")
{
    InstrCache<Addr, Dummy, 8192, 0, all_ones<Addr>()> instr_cache{};

    uint32 instr_bytes = 0x2484ae10;
    Addr PC = 0x30ae17;
    const std::size_t SIZE = decltype(instr_cache)::get_capacity() / 12;

    for ( std::size_t i = 0; i < SIZE; ++i)
    {
        Dummy instr( instr_bytes++, PC);
        instr_cache.update( PC++, instr);
    }
    CHECK( SIZE == instr_cache.size());

    instr_cache.erase( PC - 1);
    CHECK( SIZE - 1 == instr_cache.size());
}

TEST_CASE( "update_and_find: Update_Find_And_Check")
{
    InstrCache<Addr, Dummy, 8192, 0, all_ones<Addr>()> instr_cache{};

    const uint32 instr_bytes = 0x3c010400;
    const Addr PC = 0x401c04;
    const Dummy instr( instr_bytes, PC);

    instr_cache.update( PC, instr);
    CHECK( instr_cache.find( PC).first);
}

TEST_CASE( "check_method_erase: Check_Method_Erase")
{
    InstrCache<Addr, Dummy, 8192, 0, all_ones<Addr>()> instr_cache{};

    const uint32 instr_bytes = 0x3c010400;
    const Addr PC = 0x401c04;
    const Dummy instr( instr_bytes, PC);

    instr_cache.update( PC, instr);
    instr_cache.erase( PC);

    CHECK( !instr_cache.find( PC).first);
}

TEST_CASE( "check_method_empty: Check_Method_Empty")
{
    InstrCache<Addr, Dummy, 8192, 0, all_ones<Addr>()> instr_cache{};

    const uint32 instr_bytes = 0x2484ae10;
    const Addr PC = 0x400d05;
    const Dummy instr( instr_bytes, PC);

    CHECK( instr_cache.empty());
    instr_cache.update( PC, instr);
    CHECK( !instr_cache.empty());
}


TEST_CASE( "exceed_capacity_and_test_lru: Add_More_Elements_Than_Capacity_And_Check")
{
    constexpr const auto CAPACITY = 8192U;

    InstrCache<std::size_t, Dummy, CAPACITY, 0, all_ones<std::size_t>()> cache;
    for ( std::size_t i = 1; i <= CAPACITY; ++i) // note the <=
        cache.update( i, Dummy( i, i));

    cache.update( 1, Dummy( 1, 1));
    cache.update( CAPACITY / 2, Dummy( CAPACITY / 2, CAPACITY / 2));

    cache.update( CAPACITY + 1, Dummy( CAPACITY + 1, CAPACITY + 1));

    CHECK( cache.size() == CAPACITY);
    CHECK( !cache.empty());
    CHECK( !cache.find( 2).first);
}


