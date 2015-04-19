/**
 * cach_tag_array.h - Header of the module implementing different
 * types of cache.
 * @author Dmitry Ermilov <ermilov@phistech.edu>
 * For MIPT MIPS 2015
 */

#ifndef CACHE_TAG_ARRAY_H_INCLUDED
#define CACHE_TAG_ARRAY_H_INCLUDED

// Generci C
#include <cstdlib>

// uArchSim modules
#include "types.h"


class CacheTagArray
{
    enum CacheType
    {
        FULL_ASSOSIATIVE = 0,
        DIRECT = 1,
        TWO_WAY = 2,
        FOUR_WAY = 4,
        EIGHT_WAY = 8
    }  type;

    unsigned cache_size;
    unsigned set_bits;
    unsigned tag_bits;
    unsigned line_bits;

    uint32 set_mask;
    uint64 tag_mask;

public:

    CacheTagArray( unsigned int size_in_bytes,
                   unsigned int ways,
                   unsigned short block_size_in_bytes = 4,
                   unsigned short addr_size_in_bits = 32);
    virtual ~CacheTagArray() {};
    virtual bool read( uint64 addr) = 0;
    virtual void write( uint64 addr) = 0;

    uint64 get_tag( uint64 address);
    uint32 get_set( uint64 address);

    unsigned culc_way_size() const;
    unsigned culc_set_bits() const;
    unsigned culc_line_bits( unsigned block_size);
    unsigned get_set_bits() const { return set_bits; }
    unsigned get_cache_size() const { return cache_size; }
    uint8    get_type() const { return ( uint8) type; }

};

class Node
{
    uint32 number;
    uint64 tag;
    Node*  next;

public:

    Node( uint64 it_tag = 0, Node* it_next = NULL, uint32 num = 0)
    {
      tag = it_tag;
      next = it_next;
      number = num;
    }
    ~Node() {}

    uint64 get_tag()  const { return tag; }
    Node*  get_next() const { return next; }
    uint32 get_number() const { return number; }

    void   set_tag( uint64 it_tag) { tag = it_tag; }
    void   set_next( Node* it_next) { next = it_next; }
    void   set_number( uint32 it_num) { number = it_num; }
};

class LruQue
{
    Node* head;
    Node* last;
    uint32 counter;

public:

    LruQue( Node* it_head = NULL, Node* it_last = NULL)
    {
      head = it_head;
      last = it_last;
      counter = 0;
    }
    ~LruQue();

    Node*   find( uint64 tag, Node** prev);
    void    add( uint64 tag, uint32 num);
    void    move_to_head( uint64 tag);
    uint32  extract_min_value( uint64* tag_ptr);
    uint32  get_counter() const { return counter; }
    void    inc_counter() { counter++; }

};

class FullAssosiativeCache : public CacheTagArray
{
    uint64*  cache;
    LruQue   queue;
public:
    FullAssosiativeCache( unsigned int size_in_bytes,
                          unsigned int ways = 0,
                          unsigned short block_size_in_bytes = 4,
                          unsigned short addr_size_in_bits = 32);
    virtual ~FullAssosiativeCache();
    virtual bool read( uint64 addr);
    virtual void write( uint64 addr);
};

class DirectMappedCache : public CacheTagArray
{
    uint64* cache;
public:
    DirectMappedCache( unsigned int size_in_bytes,
                       unsigned int ways = 1,
                       unsigned short block_size_in_bytes = 4,
                       unsigned short addr_size_in_bits = 32);
    virtual ~DirectMappedCache();
    virtual bool read( uint64 addr);
    virtual void write( uint64 addr);
};

class MultiWayCache : public CacheTagArray
{
    uint64** cache;
    LruQue*  que_array;
public:
    MultiWayCache( unsigned int size_in_bytes,
                   unsigned int ways,
                   unsigned short block_size_in_bytes = 4,
                   unsigned short addr_size_in_bits = 32);
    virtual ~MultiWayCache();
    virtual bool read( uint64 addr);
    virtual void write( uint64 addr);
};
#endif // CACHE_TAG_ARRAY_H_INCLUDED
