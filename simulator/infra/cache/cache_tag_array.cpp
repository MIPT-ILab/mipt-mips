/**
 * cache_tag_array.cpp
 * Implementation of the cache tag array model.
 * @author Oleg Ladin, Denis Los, Andrey Agrachev, Pavel Kryukov
 * Copyright 2014-2019 MIPT-MIPS
 */

#include "infra/cache/cache_tag_array.h"
#include "infra/macro.h"
#include "infra/replacement/cache_replacement.h"

#include <sparsehash/dense_hash_map.h>

#include <utility>
#include <vector>

class AlwaysHitCacheTagArray : public CacheTagArray
{
    public:
        uint32 set( Addr /* unused */) const final { return 0; }
        Addr tag( Addr addr) const final { return addr; }
        int32 write( Addr /* unused */) final { return -1; }
        std::pair<bool, int32> read( Addr addr) final { return read_no_touch( addr); }
        std::pair<bool, int32> read_no_touch( Addr /* unused */) const final { return {true, -1}; }
};

class InfiniteCacheTagArray : public CacheTagArray
{
    public:
        InfiniteCacheTagArray() {
            lookup_helper.set_empty_key( impossible_key);
            lookup_helper.set_deleted_key( impossible_key - 1);
        }

        uint32 set( Addr /* unused */) const final { return 0; }
        Addr tag( Addr addr) const final { return addr; }
        int32 write( Addr addr) final;
        std::pair<bool, int32> read( Addr addr) final { return read_no_touch( addr); }
        std::pair<bool, int32> read_no_touch( Addr addr) const final;
    private:
        std::vector<Addr> tags;

        // hash table to lookup tags in O(1)
        google::dense_hash_map<Addr, int32> lookup_helper;
        const int32 impossible_key = INT32_MAX;
};

std::pair<bool, int32> InfiniteCacheTagArray::read_no_touch( Addr addr) const
{
    auto res = lookup_helper.find( addr);
    return res == lookup_helper.end()
        ? std::pair{ false, -1 }
        : std::pair{ true, res->second };
}

int32 InfiniteCacheTagArray::write( Addr addr)
{
    auto result = read_no_touch( addr);
    if ( result.first)
        return result.second;

    auto way = narrow_cast<int32>( tags.size());
    tags.emplace_back( addr);
    lookup_helper.emplace( addr, way);

    return way;
}

// Cache tag array module implementation
class CacheTagArraySizeCheck : public CacheTagArray
{
    public:
        CacheTagArraySizeCheck(
            uint32 size_in_bytes,
            uint32 ways,
            uint32 line_size,
            uint32 addr_size_in_bits
        );

        uint32 size_in_bytes;
        uint32 ways;
        const uint32 line_size;
        const uint32 addr_size_in_bits;
};

CacheTagArraySizeCheck::CacheTagArraySizeCheck(
    uint32 size_in_bytes,
    uint32 ways,
    uint32 line_size,
    uint32 addr_size_in_bits)
    : size_in_bytes( size_in_bytes)
    , ways( ways)
    , line_size( line_size)
    , addr_size_in_bits( addr_size_in_bits)
{
    if ( size_in_bytes == 0)
        throw CacheTagArrayInvalidSizeException("Cache size should be greater than zero");

    if ( ways == 0)
        throw CacheTagArrayInvalidSizeException("Num of ways should be greater than zero");

    if ( line_size == 0)
        throw CacheTagArrayInvalidSizeException("Size of the line should be greater than zero");

    if ( addr_size_in_bits == 0)
        throw CacheTagArrayInvalidSizeException("Address size should be greater than zero");

    if ( addr_size_in_bits > 32)
        throw CacheTagArrayInvalidSizeException("Address size should be less or equal than 32");

    if ( size_in_bytes < ways * line_size)
        throw CacheTagArrayInvalidSizeException("Cache size should be greater"
            "than the number of ways multiplied by line size");

    if ( !is_power_of_two( size_in_bytes))
        throw CacheTagArrayInvalidSizeException("Cache size should be a power of 2");

    if ( !is_power_of_two( line_size))
        throw CacheTagArrayInvalidSizeException("Block size should be a power of 2");

    if ( size_in_bytes % ( line_size * ways) != 0)
        throw CacheTagArrayInvalidSizeException("Cache size should be multiple of");
}

class CacheTagArraySize : public CacheTagArraySizeCheck
{
    protected:
        CacheTagArraySize(
            uint32 size_in_bytes,
            uint32 ways,
            uint32 line_size,
            uint32 addr_size_in_bits
        )
            : CacheTagArraySizeCheck( size_in_bytes, ways, line_size, addr_size_in_bits)
            , line_bits( std::countr_zero( line_size)) // the number of offset bits
            , sets( size_in_bytes / ( ways * line_size))
            , set_bits( std::countr_zero( sets) + line_bits)
            , addr_mask( bitmask<Addr>( addr_size_in_bits))
        { }

        auto get_line_bits() const noexcept { return line_bits; }
    public:
        const size_t line_bits;
        uint32 sets;
        const size_t set_bits;
        const Addr   addr_mask;

        uint32 set( Addr addr) const final { return ( ( addr & addr_mask) >> line_bits) & (sets - 1); }
        Addr tag( Addr addr) const final { return ( addr & addr_mask) >> set_bits; }
};

class ReplacementModule
{
    public:
        ReplacementModule( std::size_t number_of_sets, std::size_t number_of_ways, const std::string& replacement_policy);
        void touch( uint32 num_set, uint32 num_way) { replacement_info[ num_set]->touch( num_way); }
        auto update( uint32 num_set) { return replacement_info[ num_set]->update(); }

    private:
        std::vector<std::unique_ptr<CacheReplacement>> replacement_info;
};

ReplacementModule::ReplacementModule( std::size_t number_of_sets, std::size_t number_of_ways, const std::string& replacement_policy)
    : replacement_info( number_of_sets)
{
    for (auto& e : replacement_info)
        e = create_cache_replacement( replacement_policy, number_of_ways);
}

class SimpleCacheTagArray : public CacheTagArraySize
{
    public:
        SimpleCacheTagArray(
            uint32 size_in_bytes,
            uint32 ways,
            uint32 line_size,
            uint32 addr_size_in_bits,
            const std::string& repl_policy
        );

        int32 write( Addr addr) final;
        std::pair<bool, int32> read( Addr addr) final;
        std::pair<bool, int32> read_no_touch( Addr addr) const final;

    private:
        struct Tag
        {
            bool is_valid = false;
            Addr tag = {};
        };

        // tags storage
        std::vector<std::vector<Tag>> tags;

        // hash table to lookup tags in O(1)
        std::vector<google::dense_hash_map<Addr, int32>> lookup_helper;
        const int32 impossible_key = INT32_MAX;
        std::unique_ptr<ReplacementModule> replacement_module = nullptr;
};

SimpleCacheTagArray::SimpleCacheTagArray(
    uint32 size_in_bytes,
    uint32 ways,
    uint32 line_size,
    uint32 addr_size_in_bits,
    const std::string& repl_policy)
        : CacheTagArraySize( size_in_bytes, ways, line_size, addr_size_in_bits)
        , tags( sets, std::vector<Tag>( ways))
        , lookup_helper( sets, google::dense_hash_map<Addr, int32>( ways))
{
    replacement_module = std::make_unique<ReplacementModule>( sets, ways, repl_policy);

    // these are special dense_hash_map requirements
    for (uint32 i = 0; i < sets; i++) {
        lookup_helper[i].set_empty_key( impossible_key);
        lookup_helper[i].set_deleted_key( impossible_key - 1);
    }
}

std::pair<bool, int32> SimpleCacheTagArray::read( Addr addr)
{
    const auto lookup_result = read_no_touch( addr);
    const auto&[ is_hit, way] = lookup_result;

    if ( is_hit)
    {
        uint32 num_set = set( addr);
        replacement_module->touch( num_set, way); // for replacement policy (updates the current address)
    }

    return lookup_result;
}

std::pair<bool, int32> SimpleCacheTagArray::read_no_touch( Addr addr) const
{
    const uint32 num_set = set( addr);
    const Addr   num_tag = tag( addr);

    const auto& result = lookup_helper[ num_set].find( num_tag);
    return ( result != lookup_helper[ num_set].end())
           ? std::pair{ true, result->second}
           : std::pair{ false, -1};
}

int32 SimpleCacheTagArray::write( Addr addr)
{
    const Addr new_tag = tag( addr);

    // get cache coordinates
    const uint32 num_set = set( addr);
    const auto way = narrow_cast<int32>( replacement_module->update( num_set));

    // get an old tag
    auto& entry = tags[ num_set][ way];
    const Addr old_tag = entry.tag;

    // Remove old tag from lookup helper and add a new tag
    if ( entry.is_valid)
        lookup_helper[ num_set].erase( old_tag);
    lookup_helper[ num_set].emplace( new_tag, way);

    // Update tag array
    entry.tag = new_tag;
    entry.is_valid = true;

    return way;
}

std::unique_ptr<CacheTagArray> CacheTagArray::create(
    const std::string& type,
    uint32 size_in_bytes,
    uint32 ways,
    uint32 line_size,
    uint32 addr_size_in_bits)
{
    if ( type == "always_hit")
        return std::make_unique<AlwaysHitCacheTagArray>();
    if ( type == "infinite")
        return std::make_unique<InfiniteCacheTagArray>();

    return std::make_unique<SimpleCacheTagArray>( size_in_bytes, ways, line_size, addr_size_in_bits, type);
}

