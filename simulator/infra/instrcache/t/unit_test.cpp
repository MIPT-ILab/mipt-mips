/**
 * A cache for decoded instructions in functional simulator
 * @author Denis Los
*/

#include <catch.hpp>

// Modules
#include "../LRUCache.h"

#include <infra/types.h>
#include <mips/mips_instr.h>


class Dummy {
    const size_t value;
public:
    explicit Dummy( uint32 val) : value( val) {};
    bool is_same( const Dummy& rhs) const { return value == rhs.value; }
    bool operator==( const Dummy& rhs) const { return is_same(rhs); }
    friend std::ostream& operator<<( std::ostream& out, const Dummy& val)
    {
        return out << val.value;
    }
};

TEST_CASE( "update_and_find_int: Update_Find_And_Check_Using_Int")
{
    LRUCache<Addr, Dummy, 8192> cache{};

    const Addr PC = 0x401c04;
    const Dummy test_number( 0x103abf9);

    cache.update( PC, test_number);
    auto result = cache.find( PC);

    CHECK( result.first);
    CHECK( result.second == test_number);
}

TEST_CASE( "check_method_size: Check_Method_Size")
{
    LRUCache<Addr, MIPS32Instr, 8192> instr_cache{};

    uint32 instr_bytes = 0x2484ae10;
    Addr PC = 0x30ae17;
    const std::size_t SIZE = LRUCache<Addr, MIPS32Instr, 8192>::get_capacity() / 12;

    for ( std::size_t i = 0; i < SIZE; ++i)
    {
        MIPS32Instr instr( instr_bytes++, PC);
        instr_cache.update( PC++, instr);
    }
    CHECK( SIZE == instr_cache.size());

    instr_cache.erase( PC - 1);
    CHECK( SIZE - 1 == instr_cache.size());
}

TEST_CASE( "update_and_find: Update_Find_And_Check")
{
    LRUCache<Addr, MIPS32Instr, 8192> instr_cache{};

    const uint32 instr_bytes = 0x3c010400;
    const Addr PC = 0x401c04;
    const MIPS32Instr instr( instr_bytes, PC);

    instr_cache.update( PC, instr);
    CHECK( instr_cache.find( PC).first);
}

TEST_CASE( "check_method_erase: Check_Method_Erase")
{
    LRUCache<Addr, MIPS32Instr, 8192> instr_cache{};

    const uint32 instr_bytes = 0x3c010400;
    const Addr PC = 0x401c04;
    const MIPS32Instr instr( instr_bytes, PC);

    instr_cache.update( PC, instr);
    instr_cache.erase( PC);

    CHECK( !instr_cache.find( PC).first);
}

TEST_CASE( "check_method_empty: Check_Method_Empty")
{
    LRUCache<Addr, MIPS32Instr, 8192> instr_cache{};

    const uint32 instr_bytes = 0x2484ae10;
    const Addr PC = 0x400d05;
    const MIPS32Instr instr( instr_bytes, PC);

    CHECK( instr_cache.empty());
    instr_cache.update( PC, instr);
    CHECK( !instr_cache.empty());
}

TEST_CASE( "exceed_capacity_and_test_lru: Add_More_Elements_Than_Capacity_And_Check")
{
    constexpr const auto CAPACITY = 8192u;

    LRUCache<std::size_t, Dummy, CAPACITY> cache;

    for ( std::size_t i = 1; i <= CAPACITY; ++i) // note the <=
        cache.update( i, Dummy( i));

    cache.update( 1, Dummy( 1));
    cache.update( CAPACITY / 2, Dummy( CAPACITY / 2));

    cache.update( CAPACITY + 1, Dummy( CAPACITY + 1));

    CHECK( cache.size() == CAPACITY);
    CHECK( !cache.empty());
    CHECK( !cache.find( 2).first);
}

