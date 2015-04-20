/*
 * cache_tag_array.cpp - implementation of Tag Array for 
 * multy way cache
 * @author Mikhail Lyubogoschev lyubogoshchev@phystech.edu
 * Copyright 2015 MIPT-MIPS 
 */

#include <cache_tag_array.h>


CacheTagArray::CacheTagArray( unsigned int size_in_bytes,
                   uint64 ways,
                   unsigned short block_size_in_bytes, 
                   unsigned short addr_size_in_bits):
                   addr_bits( addr_size_in_bits),
                   ways_number(ways),
                   cache_size( size_in_bytes),
                   block_size( block_size_in_bytes)

{
    sets_in_way = size_in_bytes / block_size_in_bytes / ways;
    offset_bits = countDegree( block_size_in_bytes);
    if ( sets_in_way != 1)
        set_bits = countDegree( sets_in_way);
    else set_bits = 0;
    tag_bits = addr_bits - set_bits - offset_bits;
// creating masks for easy access to the parts of an address
    offset_mask = ( 1 << offset_bits) - 1;
    set_mask = (( 1 << set_bits) - 1) << offset_bits;
    tag_mask = (( 1 << tag_bits) - 1) << ( set_bits + offset_bits);
    
    tag_array = new list<uint64>[ sets_in_way];
    for ( int i = 0; i < sets_in_way; i++)
        tag_array[i] = list<uint64>();
}


inline uint16 CacheTagArray::countDegree( uint64 input)
{
    uint16 i = 0;
    for ( i = 0; input > 1; i++)
        input = input >> 1;
    return i;
}

bool CacheTagArray::read( uint64 addr)
{
    uint64 set = get_set( addr);
    uint64 tag = get_tag( addr); // test for fully associative cache
    list<uint64>::iterator it;
    for ( it = tag_array[ set].begin(); it != tag_array[ set].end(); it++)
        if ( *it == tag) break;
    if ( it == tag_array[ set].end())
    {
        write( addr);
        return false;
    }
    tag_array[ set].push_front( *it);
    tag_array[ set].erase( it);
    return true;
}



void CacheTagArray::write( uint64 addr)
{
    uint64 set = get_set( addr);
    uint64 tag = get_tag( addr); 
    //uint32 ways_used = tag_array[ set].size();
/*     list<uint64>::iterator it;
   for ( it = tag_array[ set].begin(); it != tag_array[ set].end(); it++)
    {
        ways_used++;
        if ( *it == tag)
        {
            tag_array[ set].push_front( *it);
            tag_array[ set].erase( it);
            return;
        }
    }*/
    if ( ways_number == tag_array[ set].size())
        tag_array[ set].pop_back();
    tag_array[ set].push_front( tag);
}