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

/* The set (line) of the cache. */
struct CacheSet
{
    uint64 line = 0; // data
    bool is_valid = false; // validaty
};

/* Replacement algorithm modules (LRU). */
class LRUInfo
{
    /*
     * "lru" contains sequences of using for each set. First number in set list
     * is the least used.
     */
    std::vector< std::list< unsigned int> > lru = {};

public:
    LRUInfo( unsigned int ways, unsigned int sets) : lru( sets)
    {
        std::list< unsigned int> l;
        for ( unsigned int i = 0; i < ways; ++i)
        {
            l.push_front( i);
        }
        std::fill_n( lru.begin(), sets, l);
    }
    /*
     * On hit - mark (push front) way that contains the set.
     */
    void update( int set, unsigned int way)
    {
        auto& list = lru[ set];
        for ( auto it = list.begin(); it != list.end(); ++it)
        {
            if ( *it == way)
            {
                list.splice( list.begin(), list, it);
                return;
            }
        }
    }
    /* Get number of the Least Resently Used way and push front it.*/
    int update( int set)
    {
        auto& list = lru[ set];
        list.splice( list.begin(), list, std::prev( list.end()));
        return list.front();
    }
};

class CacheTagArray : protected Log
{
    private:
        const unsigned int size_in_bytes;
        const unsigned int ways;
        const unsigned short block_size_in_bytes;
        const unsigned short addr_size_in_bits;
        CacheSet** set = nullptr; // array of tags
        LRUInfo* lru = nullptr; // LRU algorithm module

        /* Checks if it possiable to create cache. */
        void checkArgs( unsigned int size_in_bytes,
                        unsigned int ways,
                        unsigned short block_size_in_bytes,
                        unsigned short addr_size_in_bits);
        unsigned int getSetNum( uint64 addr);
        uint64 getTagNum( uint64 addr);
        CacheTagArray& operator=( const CacheTagArray&) = delete;
        CacheTagArray( const CacheTagArray&) = delete;

    public:
        CacheTagArray( unsigned int size_in_bytes,
                       unsigned int ways,
                       unsigned short block_size_in_bytes = 4,
                       unsigned short addr_size_in_bits = 32);
        ~CacheTagArray();
        bool read( uint64 addr); // find in the cache
        void write( uint64 addr); // add to the cache
};

#endif // #ifndef CACHE_TAG_ARRAY_H
