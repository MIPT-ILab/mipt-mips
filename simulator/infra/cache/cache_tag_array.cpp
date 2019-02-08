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
        lru_list.push_front( i);
        lru_hash.emplace( i, lru_list.begin());
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

CacheTagArraySizeCheck::CacheTagArraySizeCheck(
    uint32 size_in_bytes,
    uint32 ways,
    uint32 line_size,
    uint32 addr_size_in_bits)
    : Log( false)
    , size_in_bytes( size_in_bytes)
    , ways( ways)
    , line_size( line_size)
    , addr_size_in_bits( addr_size_in_bits)
{
    if ( size_in_bytes == 0)
        throw CacheTagArrayInvalidSizeException("Cache size should be greater than zero");

    if ( ways == 0)
        throw CacheTagArrayInvalidSizeException("Num of ways should be greater than zero");

    if ( line_size == 0)
        throw CacheTagArrayInvalidSizeException("Size of the line should be greater than zero");

    if ( addr_size_in_bits == 0)
        throw CacheTagArrayInvalidSizeException("Address size should be greater than zero");

    if ( addr_size_in_bits > 32)
        throw CacheTagArrayInvalidSizeException("Address size should be less or equal than 32");

    if ( size_in_bytes < ways * line_size)
        throw CacheTagArrayInvalidSizeException("Cache size should be greater"
            "than the number of ways multiplied by line size");

    if ( !is_power_of_two( size_in_bytes))
        throw CacheTagArrayInvalidSizeException("Cache size should be a power of 2");

    if ( !is_power_of_two( line_size))
        throw CacheTagArrayInvalidSizeException("Block size should be a power of 2");

    if ( size_in_bytes % ( line_size * ways) != 0)
        throw CacheTagArrayInvalidSizeException("Cache size should be multiple of");
}

CacheTagArraySize::CacheTagArraySize(
    uint32 size_in_bytes,
    uint32 ways,
    uint32 line_size,
    uint32 addr_size_in_bits)
    : CacheTagArraySizeCheck( size_in_bytes, ways, line_size, addr_size_in_bits)
    , line_bits ( find_first_set( line_size))
    , sets( size_in_bytes / ( ways * line_size))
    , addr_mask( bitmask<Addr>( addr_size_in_bits))
{ }

uint32 CacheTagArraySize::set( Addr addr) const
{
    return ( ( addr & addr_mask) >> line_bits) & (sets - 1);
}

Addr CacheTagArraySize::tag( Addr addr) const
{
    return ( addr & addr_mask) >> line_bits;
}

CacheTagArray::CacheTagArray(
    uint32 size_in_bytes,
    uint32 ways,
    uint32 line_size,
    uint32 addr_size_in_bits)
    : CacheTagArraySize( size_in_bytes, ways, line_size, addr_size_in_bits)
    , tags( sets, std::vector<Tag>( ways))
    , lookup_helper( sets, std::unordered_map<Addr, uint32>( ways))
    , lru_module( sets, ways)
{ }

std::pair<bool, uint32> CacheTagArray::read( Addr addr)
{
    const auto lookup_result = read_no_touch( addr);
    const auto&[ is_hit, way] = lookup_result;

    if ( is_hit)
    {
        uint32 num_set = set( addr);
        lru_module.touch( num_set, way);
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
    const uint32 way = lru_module.update( num_set);

    // get an old tag
    auto& entry = tags[ num_set][ way];
    const Addr old_tag = entry.tag;

    // Remove old tag from lookup helper and add a new tag
    if ( entry.is_valid)
        lookup_helper[ num_set].erase( old_tag);
    lookup_helper[ num_set].emplace( new_tag, way);

    // Update tag array
    entry.tag = new_tag;
    entry.is_valid = true;

    return way;
}
