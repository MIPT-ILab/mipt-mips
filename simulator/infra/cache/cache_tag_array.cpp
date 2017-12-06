/**
 * cache_tag_array.cpp
 * Implementation of the cache tag array model.
 * @author Oleg Ladin, Denis Los
 * Copyright 2014-2017 MIPT-MIPS
 */

#include <cassert>

// MIPT-MIPS includes
#include "infra/cache/cache_tag_array.h"

LRUCacheInfo::LRUCacheInfo( std::size_t ways)
    : ways( ways)
{
    assert( ways != 0u);

    lru_hash.reserve( ways);

    for ( std::size_t i = 0; i < ways; i++)
    {
        auto ptr = lru_list.insert( lru_list.begin(), i);
        lru_hash.emplace( i, ptr);
    }
}

void LRUCacheInfo::touch( std::size_t way)
{
    const auto lru_it = lru_hash.find( way);
    assert( lru_it != lru_hash.end());

    // Put the way to the head of the list
    lru_list.splice( lru_list.begin(), lru_list, lru_it->second);
}

std::size_t LRUCacheInfo::update()
{
    // remove the least recently used element from the tail
    std::size_t lru_elem = lru_list.back();
    lru_list.pop_back();

    // put it to the head
    auto ptr = lru_list.insert( lru_list.begin(), lru_elem);
    lru_hash.insert_or_assign( lru_elem, ptr);

    return lru_elem;
}

CacheTagArrayCheck::CacheTagArrayCheck( uint32 size_in_bytes,
                                        uint32 ways,
                                        uint32 line_size,
                                        uint32 addr_size_in_bits)
    : Log( false)
    , size_in_bytes( size_in_bytes)
    , number_of_ways( ways)
    , line_size( line_size)
    , addr_size_in_bits( addr_size_in_bits)
{
    if ( size_in_bytes == 0)
        serr << "ERROR: Wrong arguments! Cache size should be greater than zero"
             << std::endl << critical;

    if ( ways == 0)
        serr << "ERROR: Wrong arguments! Num of ways should be greater than zero"
             << std::endl << critical;

    if ( line_size == 0)
        serr << "ERROR: Wrong argument! Size of the line should be greater than zero"
             << std::endl << critical;

    if ( addr_size_in_bits == 0)
        serr << "ERROR: Wrong argument! Address size should be greater than zero"
             << std::endl << critical;

    if ( addr_size_in_bits > 32)
        serr << "ERROR: Wrong arguments! Address size should be less or equal than 32"
             << std::endl << critical;

    if ( size_in_bytes < ways * line_size)
        serr << "ERROR: Wrong arguments! Cache size should be greater"
             << "than the number of ways multiplied by line size"
             << std::endl << critical;

    if ( !is_power_of_two( size_in_bytes))
        serr << "ERROR: Wrong argumets! Cache size should be a power of 2"
             << std::endl << critical;

    if ( !is_power_of_two( line_size))
        serr << "ERROR: Wrong arguments! Block size should be a power of 2"
             << std::endl << critical;

    if ( size_in_bytes % ( line_size * ways) != 0)
        serr << "ERROR: Wrong arguments! Cache size should be multiple of"
             << "the number of ways and line size"
             << std::endl << critical;
}

CacheTagArray::CacheTagArray(
    uint32 size_in_bytes,
    uint32 ways,
    uint32 line_size,
    uint32 addr_size_in_bits)
    : CacheTagArrayCheck( size_in_bytes, ways, line_size, addr_size_in_bits)
    , number_of_sets( size_in_bytes / ( ways * line_size))
    , addr_mask( bitmask<Addr>( addr_size_in_bits))
    , tags( number_of_sets, std::vector<Tag>( number_of_ways))
    , lookup_helper( number_of_sets, std::unordered_map<Addr, uint32>{})
    , lru_module( number_of_sets, number_of_ways)
{
    for ( auto& map_of_ways : lookup_helper)
        map_of_ways.reserve( number_of_ways);
}

std::pair<bool, uint32> CacheTagArray::read( Addr addr)
{
    const auto lookup_result = read_no_touch( addr);
    const auto&[ is_hit, num_way] = lookup_result;

    if ( is_hit)
    {
        uint32 num_set = set( addr);
        lru_module.touch( num_set, num_way);
    }

    return lookup_result;
}

std::pair<bool, uint32> CacheTagArray::read_no_touch( Addr addr) const
{
    const uint32 num_set = set( addr);
    const Addr   num_tag = tag( addr);

    const auto& result = lookup_helper[ num_set].find( num_tag);
    return ( result != lookup_helper[ num_set].end())
           ? std::make_pair( true, result->second)
           : std::make_pair( false, NO_VAL32);
}

uint32 CacheTagArray::write( Addr addr)
{
    const Addr new_tag = tag( addr);

    // get cache coordinates
    const uint32 num_set = set( addr);
    const uint32 num_way = lru_module.update( num_set);

    // get an old tag
    const auto& old_tag = tags[ num_set][ num_way];

    // Remove old tag from lookup helper and add a new tag
    if ( old_tag.is_valid)
        lookup_helper[ num_set].erase( old_tag.tag);
    lookup_helper[ num_set].emplace( new_tag, num_way);

    // Update tag array
    tags[ num_set][ num_way].tag = new_tag;
    tags[ num_set][ num_way].is_valid = true;

    return num_way;
}

uint32 CacheTagArray::set( Addr addr) const
{
    return ( ( addr & addr_mask) / line_size) & (number_of_sets - 1);
}

Addr CacheTagArray::tag( Addr addr) const
{
    return ( addr & addr_mask) / line_size;
}
