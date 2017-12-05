/**
 * cache_tag_array.cpp
 * Implementation of the cache tag array model.
 * @author Oleg Ladin, Denis Los
 */

#include <cassert>


#include <infra/cache/cache_tag_array.h>
#include <infra/macro.h>


LRUCacheInfo::LRUCacheInfo( std::size_t capacity)
    : CAPACITY( capacity)
{
    assert( CAPACITY != 0u);

    lru_hash.reserve( CAPACITY);

    for ( std::size_t i = 0; i < CAPACITY; i++)
    {
        auto ptr = lru_list.insert( lru_list.begin(), i);
        lru_hash.emplace( i, ptr);
    }
}


void LRUCacheInfo::touch( std::size_t key)
{
    auto lru_it = lru_hash.find( key);
    assert( lru_it != lru_hash.end());

    lru_list.splice( lru_list.begin(), lru_list, lru_it->second);
}



std::size_t LRUCacheInfo::update()
{
    std::size_t lru_elem = lru_list.back();
    lru_list.pop_back(); // remove the least recently used element

    auto ptr = lru_list.insert( lru_list.begin(), lru_elem);
    lru_hash.insert_or_assign( lru_elem, ptr);

    return lru_elem;
}






CacheTagArrayCheck::CacheTagArrayCheck( uint32 size_in_bytes, 
                                        uint32 ways,
                                        uint32 line_size,
                                        uint32 addr_size_in_bits)
    : Log( false)
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
    uint32 num_set = set( addr);
    Addr   num_tag = tag( addr);

    auto result = data[ num_set].find( num_tag);
    return ( result != data[ num_set].end())
           ? std::pair<bool, uint32>( true, result -> second)
           : std::pair<bool, uint32>( false, NO_VAL32);
}



uint32 CacheTagArray::write( Addr addr)
{
    uint32 num_set = set( addr);
    Addr   num_tag = tag( addr);
 
    uint32 num_way = lru_module.update( num_set);

    // convert a num_way to a tag
    const auto&[ is_valid, lru_num_tag] = ways_to_tags[ num_set][ num_way];
    if ( is_valid)
        data[ num_set].erase( lru_num_tag);  // remove the least recently used element
    else 
        ways_to_tags[ num_set][ num_way].first = true; // mark it valid

    data[ num_set].emplace( num_tag, num_way);
    ways_to_tags[ num_set][ num_way].second = num_tag;

    return num_way;
}