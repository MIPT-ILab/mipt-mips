/**
 * cache_tag_array.h
 * Header for the cache tag array model.
 * @author Oleg Ladin, Denis Los
 */

#ifndef CACHE_TAG_ARRAY_H
#define CACHE_TAG_ARRAY_H

#include <list>
#include <utility>
#include <memory>
#include <vector>
#include <unordered_map>
#include <utility>

#include <infra/exception.h>
#include <infra/log.h>
#include <infra/macro.h>
#include <infra/types.h>
#include <infra/replacement/cache_replacement.h>
#include <sparsehash/dense_hash_map.h>

// Replacement algorithm modules (LRU)

class ReplacementModule
{
    public:
        ReplacementModule( std::size_t number_of_sets, std::size_t number_of_ways, std::string replacement_policy = "LRU");
        ~ReplacementModule() { replacement_info.clear(); }

        void touch( uint32 num_set, uint32 num_way) { replacement_info[ num_set]->touch( num_way); }
        auto update( uint32 num_set) { return replacement_info[ num_set]->update(); }

    private:
        std::vector<std::unique_ptr<CacheReplacementInterface>> replacement_info;
};

struct CacheTagArrayInvalidSizeException final : Exception
{
    explicit CacheTagArrayInvalidSizeException(const std::string& msg)
        : Exception("Invalid cache size", msg)
    { }
};

// Cache tag array module implementation
class CacheTagArraySizeCheck : public Log
{
    public:
        CacheTagArraySizeCheck(
            uint32 size_in_bytes,
            uint32 ways,
            uint32 line_size,
            uint32 addr_size_in_bits
        );

        const uint32 size_in_bytes;
        const uint32 ways;
        const uint32 line_size;
        const uint32 addr_size_in_bits;
};

class CacheTagArraySize : public CacheTagArraySizeCheck
{
    protected:
        CacheTagArraySize(
            uint32 size_in_bytes,
            uint32 ways,
            uint32 line_size,
            uint32 addr_size_in_bits
        );
        const size_t line_bits;
    public:
        const uint32 sets;
        const size_t set_bits;
        const Addr   addr_mask;

        // extract set from address
        uint32 set( Addr addr) const;
        // extract tag from address
        Addr tag( Addr addr) const;
};

class CacheTagArray : public CacheTagArraySize
{
    public:
        using Way = uint32;

        CacheTagArray(
            uint32 size_in_bytes,
            uint32 ways,
            uint32 line_size,
            uint32 addr_size_in_bits = 32
        );

        // hit or not
        bool lookup( Addr addr) { return read( addr).first; }
        // lookup the cache and update LRU info
        std::pair<bool, Way> read( Addr addr);
        // find in the cache but do not update LRU info
        std::pair<bool, Way> read_no_touch( Addr addr) const;
        // create new entry in cache
        Way write( Addr addr);
    private:
        struct Tag
        {
            bool is_valid = false;
            Addr tag = 0u;
        };

        // tags storage
        std::vector<std::vector<Tag>> tags;

        // hash tabe to lookup tags in O(1)
        std::vector<google::dense_hash_map<Addr, Way>> lookup_helper;
        const uint32 impossible_key = INT32_MAX;
        ReplacementModule replacement_module; // LRU algorithm module
};

#endif // CACHE_TAG_ARRAY_H
