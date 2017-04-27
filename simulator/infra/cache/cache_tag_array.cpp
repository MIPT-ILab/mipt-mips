/*
 * cache_tag_array.cpp
 * Implementation of the cache tag array model.
 * MIPT-MIPS Assignment 5.
 * Ladin Oleg.
 */

/* C generic modules */
#include <cstdlib>

/* C++ generic modules */
#include <iostream>

/* MIPT-MIPS infra */
#include <infra/macro.h>

/* MIPT-MIPS modules */
#include "cache_tag_array.h"

LRUInfo::LRUInfo( uint32 ways, uint32 sets) : lru( sets)
{
    std::list< uint32> l;
    for ( uint32 i = 0; i < ways; ++i)
    {
        l.push_front( i);
    }
    std::fill_n( lru.begin(), sets, l);
}

/* On hit - mark (push front) way that contains the set */
void LRUInfo::touch( uint32 set, uint32 way)
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

/* Get number of the Least Resently Used way and push front it */
uint32 LRUInfo::update( uint32 set)
{
    auto& list = lru[ set];
    list.splice( list.begin(), list, std::prev( list.end()));
    return list.front();
}

CacheTagArrayCheck::CacheTagArrayCheck(
    uint32 size_in_bytes,
    uint32 ways,
    uint32 line_size,
    uint32 addr_size_in_bits) : Log( false),
		      size_in_bytes( size_in_bytes),
		      ways( ways),
		      line_size( line_size),
		      addr_size_in_bits( addr_size_in_bits)

{
    /* All args are not less than zero because of "unsigned" keyword. */
    if ( size_in_bytes == 0)
        serr << "ERROR: Wrong arguments! Cache size should be greater than zero"
             << std::endl << critical;

    if ( ways == 0)
        serr << "ERROR: Wrong arguments! Num of ways should be greater than zero"
             << std::endl << critical;

    if ( line_size == 0)
        serr << "ERROR: Wrong arguments! Line size should be greater than zero"
             << std::endl << critical;

    if ( addr_size_in_bits == 0)
        serr << "ERROR: Wrong arguments! Address size should be greater than zero"
             << std::endl << critical;

    /*
     * It also checks "size_in_bytes < line_size" and "size_in_bytes
     * < ways".
     */
    if ( size_in_bytes / ways < line_size)
        serr << "ERROR: Wrong arguments! Size of each way should be not "
                  << "less than size of block (size in bytes of cache should "
                  << "be not less than number of ways and size of block in "
                  << "bytes)." << critical;

    /*
     * It also checks "size_in_bytes % line_size != 0" and
     * "size_in_bytes % ways != 0".
     */
    if ( ( size_in_bytes % ( line_size * ways)) != 0)
        serr << "ERROR: Wrong arguments! Size of cache should be a "
                  << "multiple of block size in bytes and number of ways."
                  << critical;

    /* The next two use: "2^a=b"<=>"b=100...000[2]"<=>"(b&(b-1))=0". */
    if ( !is_power_of_two( size_in_bytes / ( line_size * ways)))
        serr << "ERROR: Wrong arguments! Number of sets should be a power"
                  << " of 2." << critical;

    if ( !is_power_of_two( line_size))
        serr << "ERROR: Wrong arguments! Block size should be a power of "
                  << "2." << critical;
}

CacheTagArray::CacheTagArray( uint32 size_in_bytes,
                              uint32 ways,
                              uint32 line_size,
                              uint32 addr_size_in_bits) :
                              CacheTagArrayCheck( size_in_bytes, ways,
                                                  line_size, addr_size_in_bits),
                              num_sets( size_in_bytes / ( line_size * ways))
{
    /* Allocate memory for cache sets and LRU module. */
    array.resize( num_sets);
    for ( auto& entry : array)
    {
        entry.resize( ways);
    }
    lru = new LRUInfo( ways, num_sets);
}

CacheTagArray::~CacheTagArray()
{
    /* Free memory used by LRU module. */
    delete lru;
}

std::pair<bool, uint32> CacheTagArray::read( Addr addr)
{
    const auto lookup_result = read_no_touch( addr);

    if ( lookup_result.first)
    {
        const auto set_num = set( addr);
        const auto way_num = lookup_result.second;
        lru->touch( set_num, way_num); // update LRU info
    }

    return lookup_result;
}

std::pair<bool, uint32> CacheTagArray::read_no_touch( Addr addr) const
{
    const auto set_num = set( addr);
    const auto tag_num = tag( addr);

    /* search into each way */
    for ( uint32 i = 0; i < ways; ++i)
    {
        const auto& entry = array[ set_num][ i];

        if ( entry.is_valid && entry.line == tag_num) // hit
            return std::make_pair(true, i);
    }
    return std::make_pair(false, NO_VAL32); // miss (no data)
}

uint32 CacheTagArray::write( Addr addr)
{
    uint32 set_num = set( addr);
    uint32 way_num = lru->update( set_num); // get l.r.u. way

    array[ set_num][ way_num].line = tag( addr); // write it
    array[ set_num][ way_num].is_valid = true; // this set is valid now

    return way_num;
}

uint32 CacheTagArray::set( Addr addr) const
{
    /* Cut "logbin(line_size)" bits from the end. */
    return (addr / line_size) & (num_sets - 1);
}

Addr CacheTagArray::tag( Addr addr) const
{
    /* Cut "logbin(line_size)" bits from the end. */
    return addr / line_size;
}

