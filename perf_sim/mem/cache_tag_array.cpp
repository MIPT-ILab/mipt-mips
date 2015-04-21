/**
 * cache_tag_array.cpp - Module implementing simple cache model
 * @author Alexander Pronin <alexander.pronin.mipt@gmail.com>
 * Copyright 2015 MIPT-MIPS
 */

#include <cache_tag_array.h>
#include <iostream>

// init LRU array
LRUlist::LRUlist( unsigned int ways) {
    num_of_ways = ways;
    lru_ways = new unsigned int [ num_of_ways];
    last = 0;
}

// find way in array and mark it as just used
void LRUlist::find( unsigned int way) {
    for ( unsigned int i = 0; i < num_of_ways; i++)
        if ( way == lru_ways[ i]) {
            update( i);
            return;
        }
}

// add new elem
void LRUlist::insert() {
    if ( last != num_of_ways) {
        lru_ways[ last] = last;           // last == num of next free way in this case
        last++;
    }

    update( last - 1);
}

// mark way in current position as just used
void LRUlist::update( unsigned int pos) {
    unsigned int buffer = lru_ways[ pos];
    for ( unsigned int i = pos; i > 0; i--)
        lru_ways[ i] = lru_ways[ i - 1];
    lru_ways[ 0] = buffer;
}

// free allocated for LRU array memory
LRUlist::~LRUlist() {
    delete [] lru_ways;
}

// init current set
Set::Set( unsigned short offset_bits, unsigned int ways) {
    num_of_ways = ways;

    tag = new uint64 [ num_of_ways];

    LRU  = new LRUlist( num_of_ways);
    init = new bool   [ num_of_ways];
    for ( int i = 0; i < num_of_ways; i++)
        init[ i] = false;
}

// free allocated memory
Set::~Set() {
    delete LRU;
    delete [] init;
    delete [] tag;
}

// init Cache
CacheTagArray::CacheTagArray( unsigned int size_in_bytes,
                              unsigned int ways,
                              unsigned short block_size_in_bytes, 
                              unsigned short addr_size_in_bits) {
    num_of_ways = ways;
    set_num = ways ? size_in_bytes / ways / block_size_in_bytes : 1;                           // if ways == 0 the cache is full associative
    set_bits = 0;                                                                              // in this way the set_mask will be 0 and set_num will be 1
    offset_bits = 0;                                                                           // so we will init 1 set and cur_set will always be 0
    offset_num = block_size_in_bytes;

    set_mask    = set_num - 1;
    offset_mask = block_size_in_bytes - 1;
    uint64 set_num_buff = set_num;

    while ( set_num > 1) {
        set_num /= 2;
        set_bits++;
        assert ( !( (set_num % 2) ^ (set_num == 1)));                                          // check that set_num is some power of 2
    }
    set_num = set_num_buff;

    while ( block_size_in_bytes > 1) {
        block_size_in_bytes /= 2;
        offset_bits++;
        assert ( !( ( block_size_in_bytes % 2) ^ ( block_size_in_bytes == 1)));                // check that block_size_in_bits is some power of 2
    }

    assert( addr_size_in_bits >= offset_bits + set_bits);

    set_mask = set_mask << offset_bits;
    addr_mask = 1;
    for ( int i = 0; i < addr_size_in_bits; i++)
        addr_mask *= 2;
    addr_mask--;

    if ( !num_of_ways)                                                                          // if we have full associative cache we init one set and many ways
        num_of_ways = ( size_in_bytes / offset_num);
    
    set = new Set* [ set_num];
    for ( uint64 i = 0; i < set_num; i++) {
        set[ i] = new Set( offset_bits, num_of_ways);
    }
}

CacheTagArray::~CacheTagArray() {
    for( uint64 i = 0; i < set_num; i++)
        delete set[ i];
    delete [] set;
}

// search for addr in cache
bool CacheTagArray::read( uint64 addr) {
    uint64 cur_set    = ( addr & set_mask) >> offset_bits;
    uint64 cur_tag    = ( addr & ( addr_mask & ( ~( offset_mask + set_mask)))) >> ( offset_bits + set_bits);

    for ( int i = 0; i < num_of_ways; i++) {
        if ( ( set[ cur_set]->init[ i]) && ( set[ cur_set]->tag[ i] == cur_tag)) {
            set[ cur_set]->LRU->find( i);
            return true;
        }
    }

    return false;
}

// mark addr is stored in the cache
void CacheTagArray::write( uint64 addr) {
    uint64 cur_set    = ( addr & set_mask) >> offset_bits;
    uint64 cur_tag    = ( addr & ( addr_mask & ( ~( offset_mask + set_mask)))) >> ( offset_bits + set_bits);

    set[ cur_set]->LRU->insert();
    unsigned short way_to_write = set[ cur_set]->LRU->lru_ways[ 0];
    set[ cur_set]->tag[ way_to_write] = cur_tag;

    set[ cur_set]->init[ way_to_write] = true;
}