/**
 * cache_tag_array.h
 * Header for the cache tag array model.
 * @author Oleg Ladin, Denis Los
 */

#ifndef CACHE_TAG_ARRAY_H
#define CACHE_TAG_ARRAY_H

#include <infra/exception.h>
#include <infra/log.h>
#include <infra/macro.h>
#include <infra/replacement/cache_replacement.h>
#include <infra/types.h>
#include <sparsehash/dense_hash_map.h>

#include <list>
#include <memory>
#include <utility>
#include <vector>

// Replacement algorithm module (LRU)

class ReplacementModule
{
    public:
        ReplacementModule( std::size_t number_of_sets, std::size_t number_of_ways, const std::string& replacement_policy = "LRU");
        void touch( uint32 num_set, uint32 num_way) { replacement_info[ num_set]->touch( num_way); }
        auto update( uint32 num_set) { return replacement_info[ num_set]->update(); }

    private:
        std::vector<std::unique_ptr<CacheReplacement>> replacement_info;
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

        uint32 size_in_bytes;
        uint32 ways;
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
        uint32 sets;
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
            uint32 addr_size_in_bits = 32,
            bool replacement_module_generation = true
        );

        // hit or not
        virtual bool lookup( Addr addr) { return read( addr).first; }
        // create new entry in cache
        virtual Way write( Addr addr);
        // lookup the cache and update LRU info
        virtual std::pair<bool, Way> read( Addr addr);
        // find in the cache but do not update LRU info
        std::pair<bool, Way> read_no_touch( Addr addr) const;
    protected:
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
        std::unique_ptr<ReplacementModule> replacement_module = nullptr;
};

class AlwaysHitCacheTagArray : public CacheTagArray
{
    public:
        AlwaysHitCacheTagArray(
                uint32 size_in_bytes,
                uint32 ways,
                uint32 line_size,
                uint32 addr_size_in_bits = 32
        );

        bool lookup( Addr /* unused */) override { return true; }
};

class InfiniteCacheTagArray : public CacheTagArray
{
    public:
        InfiniteCacheTagArray(
                uint32 size_in_bytes,
                uint32 ways,
                uint32 line_size,
                uint32 addr_size_in_bits = 32
        );

        Way write( Addr addr) override;
        // Since it's not permitted to these interfaces, we have to keep both methods
        // even thow touch concept is pointless in the infinite cache model
        std::pair<bool, Way> read( Addr addr) override { return read_no_touch( addr); }
    private:
        void double_size();
        std::vector<Way> way_counter;
};

std::unique_ptr<CacheTagArray> create_cache_tag_array(
    uint32 size_in_bytes,
    uint32 ways,
    uint32 line_size,
    uint32 addr_size_in_bits = 32,
    const std::string& cache_tag_array_type = "always_hit"); //should be default for normal behaviour

#endif // CACHE_TAG_ARRAY_H
