/**
 * cache_tag_array.h - Header of module implementing simple cache model
 * @author Alexander Pronin <alexander.pronin.mipt@gmail.com>
 * Copyright 2015 MIPT-MIPS
 */

#ifndef CACHE_TAG_ARRAY
#define CACHE_TAG_ARRAY

#include <assert.h>
#include <types.h>

// LRU list for current set
class LRUlist
{
    void update( unsigned int pos);   // mark pos elem as just used
public:
    unsigned int* lru_ways;           // array with nums of ways in lru order
    unsigned int  num_of_ways;
    unsigned int  last;               // next way to init

    LRUlist( unsigned int ways);      // init LRU array for current line
    ~LRUlist();
    void find( unsigned int way);     // find way int LRU list and mark it as just used
    void insert();                    // add new elem
};

// Set
class Set
{
public:
    unsigned int num_of_ways;
    bool*    init;
    uint64*  tag;                     // array with tags in different ways
    LRUlist* LRU;

    Set( unsigned short offset_bits, unsigned int ways);
    ~Set();
};

// Cache
class CacheTagArray
{
    unsigned short set_bits;
    unsigned short offset_bits;
    unsigned int   num_of_ways;

    uint64 set_num;
    uint64 offset_num;

    uint64 addr_mask;
    uint64 set_mask;
    uint64 offset_mask;

    Set** set;

public:
    CacheTagArray( unsigned int size_in_bytes,
                   unsigned int ways,
                   unsigned short block_size_in_bytes, 
                   unsigned short addr_size_in_bits);   // init the cache with current parametres

    ~CacheTagArray();

    bool read( uint64 addr);                            // check if addr stored in the cache

    void write( uint64 addr);                           // mark that addr is stored in the cache
};

#endif
