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
#include <common/types.h>
#include <common/log.h>

/* Replacement algorithm modules (LRU). */
class LRUInfo
{
    /*
     * "lru" contains sequences of using for each set. First number in set list
     * is the least used.
     */
    std::vector< std::list< unsigned int> > lru = {};

public:
    LRUInfo( unsigned int ways, unsigned int sets);
    void touch( int set, unsigned int way);
    int update( int set);
};

class CacheTagArray : protected Log
{
    private:
        /* The set (line) of the cache. */
        struct CacheSet
        {
            uint64 line = 0; // data
            bool is_valid = false; // validity
        };
        const unsigned int size_in_bytes;
        const unsigned int ways;
        const unsigned int block_size_in_bytes;
        const unsigned int addr_size_in_bits;
        CacheSet** set = nullptr; // array of tags
        LRUInfo* lru = nullptr; // LRU algorithm module

        /* Checks if it possible to create cache. */
        void checkArgs( unsigned int size_in_bytes,
                        unsigned int ways,
                        unsigned int block_size_in_bytes,
                        unsigned int addr_size_in_bits);

        unsigned int getSetNum( Addr addr) const;
        uint64 getTagNum( Addr addr) const;

        CacheTagArray& operator=( const CacheTagArray&) = delete;
        CacheTagArray( const CacheTagArray&) = delete;

    public:
        CacheTagArray( unsigned int size_in_bytes,
                       unsigned int ways,
                       unsigned int block_size_in_bytes = 4,
                       unsigned int addr_size_in_bits = 32);
        ~CacheTagArray();

        /* lookup the cache and update LRU info */
        bool read( Addr addr, unsigned int* way = nullptr);
        /* find in the cache but do not update LRU info */
        bool read_no_touch( Addr addr, unsigned int* way = nullptr) const;
        /* create new entry in cache */
        void write( Addr addr, unsigned int* way = nullptr);
};

#endif // #ifndef CACHE_TAG_ARRAY_H
