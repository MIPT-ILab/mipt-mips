/**
 * cache_tag_array.h
 * Header for the cache tag array model.
 * MIPT-MIPS Assignment 5.
 * Ladin Oleg.
 */

/* Protection from multi-including. */
#ifndef CACHE_TAG_ARRAY_H
#define CACHE_TAG_ARRAY_H

/* C++ libraries. */
#include <list>
#include <vector>

/* Simulator modules. */
#include <infra/types.h>
#include <infra/log.h>

/* Replacement algorithm modules (LRU). */
class LRUInfo
{
    /*
     * "lru" contains sequences of using for each set. First number in set list
     * is the least used.
     */
    std::vector< std::list< uint32>> lru = {};

public:
    LRUInfo( uint32 ways, uint32 sets);
    void touch( uint32 set, uint32 way);
    uint32 update( uint32 set);
};

class CacheTagArrayCheck : private Log
{
protected:
    CacheTagArrayCheck( uint32 size_in_bytes,
                        uint32 ways,
                        uint32 line_size,
                        uint32 addr_size_in_bits);
public:
        const uint32 size_in_bytes;
        const uint32 ways;
        const uint32 line_size;
        const uint32 addr_size_in_bits;

};

class CacheTagArray : public CacheTagArrayCheck
{
    private:
        /* The set (line) of the cache. */
        struct CacheTag
        {
            Addr line = 0; // data
            bool is_valid = false; // validity
        };

        std::vector<std::vector<CacheTag>> array = {}; // array of tags
        LRUInfo* lru = nullptr; // LRU algorithm module

        const uint32 num_sets;

        CacheTagArray& operator=( const CacheTagArray&) = delete;
        CacheTagArray( const CacheTagArray&) = delete;

    public:
        CacheTagArray( uint32 size_in_bytes,
                       uint32 ways,
                       uint32 line_size = 4,
                       uint32 addr_size_in_bits = 32);
        ~CacheTagArray() override;

        /* lookup the cache and update LRU info */
        std::pair<bool, uint32> read( Addr addr);
        /* find in the cache but do not update LRU info */
        std::pair<bool, uint32> read_no_touch( Addr addr) const;
        /* create new entry in cache */
        uint32 write( Addr addr);

        uint32 set( Addr addr) const;
        Addr tag( Addr addr) const;
};

#endif // #ifndef CACHE_TAG_ARRAY_H
