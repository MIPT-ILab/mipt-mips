/**
 * cache_tag_array.cpp
 * Implementation of the cache tag array model.
 * @author Oleg Ladin, Denis Los
 */

#include <cassert>


#include <infra/cache/cache_tag_array.h>
#include <infra/macro.h>



void LRUCacheInfo::touch( std::size_t key)
{
    auto lru_it = lru_hash.find( key);

    if ( lru_it != lru_hash.end())
        lru_list.splice( lru_list.begin(), lru_list, lru_it->second);
}



std::size_t LRUCacheInfo::update()
{
    std::size_t return_value = number_of_elements;

    if ( number_of_elements == CAPACITY)
    {
        // Delete least recently used element and save its key
        const auto& lru_elem = lru_list.back();
                
        return_value = lru_elem;
        
        lru_hash.erase( lru_elem);
        lru_list.pop_back();
    }
    else
        number_of_elements++;

    auto ptr = lru_list.insert( lru_list.begin(), return_value);
    lru_hash.emplace( return_value, ptr);

    return return_value;    
}






CacheTagArrayCheck::CacheTagArrayCheck( uint32 size_in_bytes, 
                                        uint32 ways,
                                        uint32 size_of_line,
                                        uint32 addr_size_in_bits)
    : Log( false)
{
    if ( size_in_bytes == 0)
        serr << "ERROR: Wrong arguments! Cache size should be greater than zero"
             << std::endl << critical;
    
    if ( ways == 0)
        serr << "ERROR: Wrong arguments! Num of ways should be greater than zero"
             << std::endl << critical;
    
    if ( size_of_line == 0)
        serr << "ERROR: Wrong argument! Size of the line should be greater than zero"
             << std::endl << critical;
    
    if ( addr_size_in_bits == 0)
        serr << "ERROR: Wrong argument! Address size should be greater than zero"
             << std::endl << critical;
    
    if ( addr_size_in_bits > 32)
        serr << "ERROR: Wrong arguments! Address size should be less or equal than 32"
             << std::endl << critical;

                 

    if ( size_in_bytes < ways * size_of_line)
        serr << "ERROR: Wrong arguments! Cache size should be greater"
             << "than the number of ways multiplied by line size" 
             << std::endl << critical;

    if ( !is_power_of_two( size_in_bytes))
        serr << "ERROR: Wrong argumets! Cache size should be a power of 2"
             << std::endl << critical;
    
    if ( !is_power_of_two( size_of_line))
        serr << "ERROR: Wrong arguments! Block size should be a power of 2"
             << std::endl << critical;
    
    if ( size_in_bytes % ( size_of_line * ways) != 0)
        serr << "ERROR: Wrong arguments! Cache size should be multiple of"
             << "the number of ways and line size"
             << std::endl << critical;
}



std::pair<bool, uint32> CacheTagArray::read( Addr addr)
{
    auto result = read_no_touch( addr);

    if ( result.first)
    {
        uint32 num_set = set( addr);
        // update LRU passing num_way as the second argument if it's a hit
        lru_module.touch( num_set, result.second);
    }

    return result;    
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
    auto result = ways_to_tags[ num_set].find( num_way);
    if ( result != ways_to_tags[ num_set].end())
        data[ num_set].erase( result->second);  // remove the least recently used element

    data[ num_set].emplace( num_tag, num_way);
    ways_to_tags[ num_set].insert_or_assign( num_way, num_tag);

    return num_way;
}