/*
 * cache_tag_array.h - header for implementation of Tag Array for 
 * multy way cache
 * @author Mikhail Lyubogoschev lyubogoshchev@phystech.edu
 * Copyright 2015 MIPT-MIPS 
 */

#ifndef CACHE_TAG_ARRAY_H
#define CACHE_TAG_ARRAY_H

#include <iostream>
#include <types.h>
#include <math.h>
#include <list>
using std::list;
using std::exit;
using std::cout;

class CacheTagArray
{  
private:
    list<uint64>* tag_array;
    uint64 ways_number;
    uint64 cache_size;
    uint64 block_size;
    uint64 sets_in_way;
    uint64 addr_bits;
    uint64 tag_bits;
    uint64 set_bits;
    uint64 offset_bits;
    uint64 tag_mask;
    uint64 set_mask;
    uint64 offset_mask;

    inline uint16 countDegree( uint64 input); // "Fast log2" for degrees of 2
    uint64 get_set( uint64 addr) { return ( addr & set_mask) >> offset_bits;};
    uint64 get_tag( uint64 addr) { return ( addr & tag_mask) >> ( offset_bits 
                                                                 + set_bits);};
   
public:
    /**
     * Constructor params:
     *
     * 1) size_in_bytes is a number of data bytes that can be stored in the cache,
     *    i.e. if the block size is 16 Bytes then the number of data blocks in the cache is size_in_bytes/16.
     *
     * 2) ways is a number of associative ways in a set, i.e. how many blocks are referred by the same index.
     *
     * 3) block_size_in_bytes is a number of Bytes in a data block
     *
     * 4) addr_size_in_bit is a number of bits in the physical address.
     */
    CacheTagArray( unsigned int size_in_bytes,
                   uint64 ways,
                   unsigned short block_size_in_bytes, 
                   unsigned short addr_size_in_bits);
    /**
     * Return true if the byte with the given address is stored in the cache,
     * otherwise, return false.
     *
     * Note that his method updates the LRU information.
     */
    bool read( uint64 addr);
    
    /**
     * Mark that the block containing the byte with the given address
     * is stored in the cache.
     *
     * Note: in order to put the given address inside the tags it is needed
     * to select a way where it will be written in.
     * This selection is being done according to LRU (Least Recently Used)
     * policy.
     */
    void write( uint64 addr);
};








#endif