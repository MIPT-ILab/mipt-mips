/**
 * cache_tag_array.cpp
 * Implementation of the cache tag array model.
 * MIPT-MIPS Assignment 5.
 * Ladin Oleg.
 */

/* C libraries. */
#include <cstdlib>

/* C++ libraries. */
#include <iostream>

/* Simulator modules. */
#include "cache_tag_array.h"

LRUInfo::LRUInfo( unsigned int ways, unsigned int sets) : lru( sets)
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
void LRUInfo::touch( int set, unsigned int way)
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
int LRUInfo::update( int set)
{
    auto& list = lru[ set];
    list.splice( list.begin(), list, std::prev( list.end()));
    return list.front();
}

CacheTagArray::CacheTagArray( unsigned int size_in_bytes,
                              unsigned int ways,
                              unsigned short block_size_in_bytes,
                              unsigned short addr_size_in_bits) :
                              Log(false),                       //cache should not say anything but error info
                              size_in_bytes( size_in_bytes),
                              ways( ways),
                              block_size_in_bytes( block_size_in_bytes),
                              addr_size_in_bits( addr_size_in_bits)
{
    /* Check is it possiable to create the cache. */
    checkArgs( size_in_bytes, ways, block_size_in_bytes, addr_size_in_bits);
    /* Allocate memory for cache sets and LRU module. */
    set = new CacheSet* [ ways];
    for ( unsigned int i = 0; i < ways; ++i)
    {
        set[ i] = new CacheSet [ size_in_bytes / ( ways * block_size_in_bytes)];
    }
    lru = new LRUInfo( ways, size_in_bytes / ( ways * block_size_in_bytes));
}

CacheTagArray::~CacheTagArray()
{
    /* Free memory used by cache sets and LRU module. */
    for ( unsigned int i = 0; i < ways; ++i)
    {
        delete [] set[ i];
    }
    delete [] set;
    delete lru;
}

bool CacheTagArray::read( uint64 addr)
{
    const auto set_num = getSetNum( addr);
    const auto tag_num = getTagNum( addr);
    for ( unsigned int i = 0; i < ways; ++i) // search into each way
    {
        const auto& entry = set[ i][ set_num];
        if ( entry.is_valid && entry.line == tag_num) // check tag
        {
	    lru->touch( set_num, i); // update LRU info
            return true; // hit
        }
    }
    return false; // miss (no data)
}

void CacheTagArray::write( uint64 addr)
{
    unsigned int set_num = getSetNum( addr);
    unsigned int way_num = lru->update( set_num); // get l.r.u. way
    set[ way_num][ set_num].line = getTagNum( addr); // write it
    set[ way_num][ set_num].is_valid = true; // this set is valid now
}

void CacheTagArray::checkArgs( unsigned int size_in_bytes,
                               unsigned int ways,
                               unsigned short block_size_in_bytes,
                               unsigned short addr_size_in_bits)
{
    /* All args are not less than zero because of "unsigned" keyword. */
    if ( ( size_in_bytes == 0) ||
         ( ways == 0) ||
         ( block_size_in_bytes == 0) ||
         ( addr_size_in_bits == 0))
    {
        serr << "ERROR: Wrong arguments! All arguments should be greater "
                  << "than zero." << critical;
    }
    /*
     * It also checks "size_in_bytes < block_size_in_bytes" and "size_in_bytes
     * < ways".
     */
    if ( size_in_bytes / ways < block_size_in_bytes)
    {
        serr << "ERROR: Wrong arguments! Size of each way should be not "
                  << "less than size of block (size in bytes of cache should "
                  << "be not less than number of ways and size of block in "
                  << "bytes)." << critical;
    }
    /*
     * It also checks "size_in_bytes % block_size_in_bytes != 0" and
     * "size_in_bytes % ways != 0".
     */
    if ( ( size_in_bytes % ( block_size_in_bytes * ways)) != 0)
    {
        serr << "ERROR: Wrong arguments! Size of cache should be a "
                  << "multiple of block size in bytes and number of ways."
                  << critical;
    }
    /* The next two use: "2^a=b"<=>"b=100...000[2]"<=>"(b&(b-1))=0". */
    if ( ( ( size_in_bytes / ( ways * block_size_in_bytes)) &
           ( size_in_bytes / ( ways * block_size_in_bytes) - 1)) != 0)
    {
        serr << "ERROR: Wrong arguments! Number of sets should be a power"
                  << " of 2." << critical;
    }
    if ( ( block_size_in_bytes & ( block_size_in_bytes - 1)) != 0)
    {
        serr << "ERROR: Wrong arguments! Block size should be a power of "
                  << "2." << critical;
    }
}

unsigned int CacheTagArray::getSetNum( uint64 addr)
{
    /* Cut "logbin(block_size_in_bytes)" bits from the end. */
    int set_num = addr / block_size_in_bytes;
    /* Get bits that are responsible for set number. */
    set_num &= ( ( size_in_bytes / ( ways * block_size_in_bytes)) - 1);
    return set_num;
}

uint64 CacheTagArray::getTagNum( uint64 addr)
{
    /* Cut "logbin(block_size_in_bytes)" bits from the end. */
    return ( addr / block_size_in_bytes);
}
