/**
 * cache_tag_array.cpp
 * Implementation of the cache tag array model.
 * @author Oleg Ladin, Denis Los
 * Copyright 2014-2017 MIPT-MIPS
 */

#include <cassert>

// MIPT-MIPS includes
#include "infra/cache/cache_tag_array.h"

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
    , line_bits( find_first_set( line_size))
    , sets( size_in_bytes / ( ways * line_size))
    , set_bits( find_first_set( sets) + line_bits)
    , addr_mask( bitmask<Addr>( addr_size_in_bits))
{ }

uint32 CacheTagArraySize::set( Addr addr) const
{
    return ( ( addr & addr_mask) >> line_bits) & (sets - 1);
}

Addr CacheTagArraySize::tag( Addr addr) const
{
    return ( addr & addr_mask) >> set_bits;
}

CacheTagArray::CacheTagArray(
    uint32 size_in_bytes,
    uint32 ways,
    uint32 line_size,
    uint32 addr_size_in_bits)
    : CacheTagArraySize( size_in_bytes, ways, line_size, addr_size_in_bits)
    , tags( sets, std::vector<Tag>( ways))
    , lookup_helper( sets, google::dense_hash_map<Addr, uint32>( ways))
    , replacement_module( sets, ways)
{
    //theese are spicial dense_hash_map requirements
    for (uint32 i = 0; i < sets; i++) {
        lookup_helper[i].set_empty_key( impossible_key);
        lookup_helper[i].set_deleted_key( impossible_key - 1);
    }
}

std::pair<bool, uint32> CacheTagArray::read( Addr addr)
{
    const auto lookup_result = read_no_touch( addr);
    const auto&[ is_hit, way] = lookup_result;

    if ( is_hit)
    {
        uint32 num_set = set( addr);
        replacement_module.touch( num_set, way);
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
    const auto way = narrow_cast<uint32>( replacement_module.update( num_set));

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

ReplacementModule::ReplacementModule( std::size_t number_of_sets, std::size_t number_of_ways, std::string replacement_policy)
{
    for ( std::size_t i = 0; i < number_of_sets; i++)
        replacement_info.emplace_back( create_cache_replacement( replacement_policy, number_of_ways));
}
