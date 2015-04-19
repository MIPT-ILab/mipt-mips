/**
 * cach_tag_array.cpp - the module implementing different
 * types of cache.
 * @author Dmitry Ermilov <ermilov@phistech.edu>
 * For MIPT MIPS 2015
 */

// Generic C
#include <cstring>
#include <cassert>

// Generic C++
#include <iostream>

// uArchSim modules
#include "cache_tag_array.h"
//#include "types.h"

//=============== class CacheTagArray ===============
CacheTagArray::CacheTagArray( unsigned int size_in_bytes,
                              unsigned int ways,
                              unsigned short block_size_in_bytes,
                              unsigned short addr_size_in_bits)
{
    type = ( CacheType) ways;
    cache_size = size_in_bytes;
    line_bits = culc_line_bits( block_size_in_bytes);
    set_bits = culc_set_bits();
    tag_bits = addr_size_in_bits - set_bits - line_bits;
    set_mask = ( ( 1 << set_bits) - 1) << line_bits;
    tag_mask = ( ( 1 << tag_bits) - 1) << ( set_bits + line_bits);
}

unsigned  CacheTagArray::culc_way_size() const
{
    int ways = type;
    if  ( ways)
        return ( cache_size / ways);
    else
        return cache_size;
}

unsigned  CacheTagArray::culc_set_bits() const
{
    unsigned way_size = culc_way_size();
    if  ( type != FULL_ASSOSIATIVE) {
        unsigned i = 0;
        for ( ; way_size > 1; i++, way_size = way_size >> 1);
        return ( i - line_bits);
    } else
        return 0;
}
unsigned  CacheTagArray::culc_line_bits( unsigned block_size)
{
    unsigned i = 0;
    for ( ; block_size > 1; i++, block_size = block_size >> 1);
        return i;
}

uint64  CacheTagArray::get_tag( uint64 address)
{
    return ( ( address & tag_mask) >> ( line_bits + set_bits));
}

uint32  CacheTagArray::get_set( uint64 address)
{
    assert( set_bits != 0);
    return ( ( address & set_mask) >> line_bits);

}
//=============== class FullAssosiativeCache ===============
FullAssosiativeCache::FullAssosiativeCache( unsigned int size_in_bytes,
                                            unsigned int ways,
                                            unsigned short block_size_in_bytes,
                                            unsigned short addr_size_in_bits)
    : CacheTagArray( size_in_bytes,
                     ways,
                     block_size_in_bytes,
                     addr_size_in_bits)
{
    cache = new uint64[get_cache_size()];
    memset( cache, 0xcc, sizeof( uint64) * get_cache_size());
}

FullAssosiativeCache::~FullAssosiativeCache()
{
    delete[] cache;
}

bool  FullAssosiativeCache::read( uint64 addr)
{
    uint64 tag = get_tag( addr);
    unsigned i = 0;
    unsigned counter = queue.get_counter();
    for ( ; ( i < counter) && ( tag != cache[i]); i++);
    if  ( i != queue.get_counter() ) {
        queue.move_to_head( tag);
        return true;
    } else {
        return false;
    }
}

void  FullAssosiativeCache::write( uint64 addr)
{
    uint64 tag = get_tag( addr);
    if  ( queue.get_counter() < get_cache_size()) {
        cache[queue.get_counter()] = tag;
        queue.add( tag, queue.get_counter());
        queue.inc_counter();
    } else  {
        uint64 tmp_tag;
        uint32 tmp_num = queue.extract_min_value( &tmp_tag);
        assert( cache[tmp_num] == tmp_tag);
        cache[tmp_num] = tag;
        queue.add( tag, tmp_num);
    }
}

//=============== class DirectMappedCache ===============
DirectMappedCache::DirectMappedCache( unsigned int size_in_bytes,
                                      unsigned int ways,
                                      unsigned short block_size_in_bytes,
                                      unsigned short addr_size_in_bits)
    : CacheTagArray( size_in_bytes,
                     ways,
                     block_size_in_bytes,
                     addr_size_in_bits)
{
    cache = new uint64[get_cache_size()];
    memset( cache, 0xcc, sizeof( uint64) * get_cache_size());
}

DirectMappedCache::~DirectMappedCache()
{
    delete[] cache;
}

bool  DirectMappedCache::read( uint64 addr)
{
    if  ( cache[get_set( addr)] == get_tag( addr))
        return true;
    else
        return false;
}

void  DirectMappedCache::write( uint64 addr)
{
    cache[get_set( addr)] = get_tag( addr);
}

//=============== class MultiWayCache ===============
MultiWayCache::MultiWayCache( unsigned int size_in_bytes,
                              unsigned int ways,
                              unsigned short block_size_in_bytes,
                              unsigned short addr_size_in_bits)
    : CacheTagArray( size_in_bytes,
                     ways,
                     block_size_in_bytes,
                     addr_size_in_bits)
{
    cache =  new uint64*[ways];
    que_array = new LruQue[1 << get_set_bits()];
    unsigned way_size = culc_way_size();
    for ( unsigned i = 0; i < ways; i++) {
        cache[i] = new uint64[way_size];
        memset( cache[i], 0xcc, sizeof( uint64) * way_size);
    }
}

MultiWayCache::~MultiWayCache()
{
    uint32 ways = get_type();
    for ( uint32 i = 0; i < ways; i++)
        delete[] cache[i];

    delete[] cache;
    delete[] que_array;
}

bool  MultiWayCache::read( uint64 addr)
{
    uint32 set = get_set( addr);
    uint64 tag = get_tag( addr);
    uint32 ways = get_type();
    uint32 i = 0;
    for ( ; ( i < ways) && ( cache[i][set] != tag); i++);
    if  ( i != ways) {
        que_array[set].move_to_head( tag);
        return true;
    }
    else
        return false;
}

void  MultiWayCache::write( uint64 addr)
{
    uint64 tag = get_tag( addr);
    uint32 set = get_set( addr);
    if  ( que_array[set].get_counter() < get_type()) {
        unsigned counter = que_array[set].get_counter();
        cache[counter][set] = tag;
        que_array[set].add( tag, counter);
        que_array[set].inc_counter();
    } else  {
        uint64 tmp_tag;
        uint32 tmp_num = que_array[set].extract_min_value( &tmp_tag);
        assert( cache[tmp_num][set] == tmp_tag);
        cache[tmp_num][set] = tag;
        que_array[set].add( tag, tmp_num);
    }
}

//=============== class LruOue ===============
LruQue::~LruQue()
{
    Node* tmp_node;
    if  ( head) {
        if  ( head->get_next()) {
            tmp_node = head->get_next();
            delete  head;
        } else {
            delete  head;
            return;
        }

        Node* prev_node = tmp_node;
        while ( tmp_node->get_next()) {
            prev_node = tmp_node;
            tmp_node = tmp_node->get_next();
            delete  prev_node;
        }
        delete  tmp_node;

        head = NULL;
        last = NULL;
    }
}

void  LruQue::add( uint64 tag, uint32 num)
{
    Node* tmp_node = new Node( tag, head, num);
    head = tmp_node;
    if  ( !last)
        last = tmp_node;
    tmp_node = NULL;
}

uint32  LruQue::extract_min_value( uint64* tag_ptr)
{
    assert( counter > 1);
    Node* tmp_node = head->get_next();
    Node* prev_node = head;

    while ( tmp_node->get_next() != NULL) {
        prev_node = tmp_node;
        tmp_node = tmp_node->get_next();
    }

    uint32 tmp_num = tmp_node->get_number();
    *tag_ptr = tmp_node->get_tag();
    delete tmp_node;
    last = prev_node;
    prev_node->set_next( NULL);
    prev_node = NULL;
    return tmp_num;
}

Node*  LruQue::find( uint64 tag, Node** prev)
{
    *prev = NULL;
    Node* tmp_node;
    Node* prev_node;

    if  ( head)
        prev_node = head;
    else
        return NULL;

    if  ( head->get_tag() == tag)
        return head;
    if  ( head->get_next())
        tmp_node = head->get_next();
    else
        return NULL;

    while ( ( tmp_node->get_next() != NULL) && ( tmp_node->get_tag() != tag)) {
        prev_node = tmp_node;
        tmp_node = tmp_node->get_next();
    }

    if  ( tmp_node->get_tag() == tag) {
        *prev = prev_node;
        return tmp_node;
    } else
        return NULL;
}

void  LruQue::move_to_head( uint64 tag)
{
    Node* prev_node;
    Node* tmp_node = find( tag, &prev_node);
    if  ( tmp_node == NULL)
            assert( !"Can't find right tag");
    else  if  ( prev_node == NULL)
        return;
    else {
        prev_node->set_next( tmp_node->get_next());
        tmp_node->set_next( head);
        head = tmp_node;
        if  ( last == tmp_node)
            last = prev_node;
    }
}
