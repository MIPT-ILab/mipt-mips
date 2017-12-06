/**
 * cache_tag_array.h
 * Header for the cache tag array model.
 * @author Oleg Ladin, Denis Los
 */

#ifndef CACHE_TAG_ARRAY_H
#define CACHE_TAG_ARRAY_H

#include <infra/types.h>
#include <infra/log.h>
#include <infra/macro.h>

#include <vector>
#include <unordered_map>
#include <utility>
#include <list>

// Replacement algorithm modules (LRU)
class LRUCacheInfo
{
    public:
        explicit LRUCacheInfo( std::size_t ways);

        void touch( std::size_t way);
        std::size_t update();

    private:
        std::list<std::size_t> lru_list{};
        std::unordered_map<std::size_t, decltype(lru_list.cbegin())> lru_hash{};

        const std::size_t ways;
};

class LRUModule
{
    public:
        LRUModule( std::size_t number_of_sets, std::size_t number_of_ways)
            : lru_info( number_of_sets, LRUCacheInfo( number_of_ways))
        { }

        void touch( uint32 num_set, uint32 num_way) { lru_info[ num_set].touch( num_way); }

        uint32 update( uint32 num_set) { return lru_info[ num_set].update(); }

    private:
        std::vector<LRUCacheInfo> lru_info;
};

// Cache tag array module implementation
class CacheTagArrayCheck : private Log
{
    protected:
        CacheTagArrayCheck( uint32 size_in_bytes,
                            uint32 ways,
                            uint32 line_size,
                            uint32 addr_size_in_bits);

    public:
        const uint32 size_in_bytes;
        const uint32 number_of_ways;
        const uint32 line_size;
        const uint32 addr_size_in_bits;
};

class CacheTagArray : public CacheTagArrayCheck
{
    public:
        using Way = uint32;

        CacheTagArray( uint32 size_in_bytes,
                       uint32 ways,
                       uint32 line_size = 4,
                       uint32 addr_size_in_bits = 32);

        // lookup the cache and update LRU info
        std::pair<bool, Way> read( Addr addr);
        // find in the cache but do not update LRU info
        std::pair<bool, Way> read_no_touch( Addr addr) const;
        // create new entry in cache
        Way write( Addr addr);

        // extract set from address
        uint32 set( Addr addr) const;
        // extract tag from address
        Addr tag( Addr addr) const;
    private:
        const uint32 number_of_sets;
        const Addr   addr_mask;

        struct Tag
        {
            bool is_valid = false;
            Addr tag = 0u;
        };

        // tags storage
        std::vector<std::vector<Tag>> tags;

        // hash tabe to lookup tags in O(1)
        std::vector<std::unordered_map<Addr, Way>> lookup_helper;
        LRUModule lru_module; // LRU algorithm module
};

#endif // CACHE_TAG_ARRAY_H
