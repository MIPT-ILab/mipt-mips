/*
 * Cache_Prop_Table - implementation of a structure
 * for testing caches
 * @author Mikhail Lyubogoschev lyubogoshchev@phystech.edu
 * Copyright 2015 MIPT-MIPS 
 */

#ifndef PROP_TABLE_H
#define PROP_TABLE_H

#include <types.h>
#include <cache_tag_array.h>

struct CacheProperties
{
    uint64 size;
    uint64 associativity;
    uint16 block_size;
    uint16 addr_size;
    uint64 miss_num;
    double miss_rate;
    CacheTagArray* cache;
    CacheProperties( uint16 block_size_bytes = 4,
                    uint16 addr_size_bits = 32);
    CacheProperties( uint64 size, uint64 ways, 
                    uint16 block_size_bytes = 4,
                    uint16 addr_size_bits = 32);
    ~CacheProperties();

};

CacheProperties::CacheProperties( uint64 size_in_bytes, uint64 ways, 
                                 uint16 block_size_bytes,
                                 uint16 addr_size_bits):
                size( size_in_bytes),
                associativity( ways),
                block_size( block_size_bytes),
                addr_size( addr_size_bits),
                miss_num( 0),
                miss_rate( 0)
                
{
    cache = new CacheTagArray( size, associativity, block_size, addr_size);
}

CacheProperties::CacheProperties( uint16 block_size_bytes,
                                 uint16 addr_size_bits):
                block_size( block_size_bytes),
                addr_size( addr_size_bits),
                miss_num( 0),
                miss_rate( 0)
{}

CacheProperties::~CacheProperties()
{
    delete [] cache;
}

#endif