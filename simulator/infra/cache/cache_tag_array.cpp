/**
 * cache_tag_array.cpp
 * Implementation of the cache tag array model.
 * @author Oleg Ladin, Denis Los
 */

#include <cassert>


#include <infra/cache/cache_tag_array.h>
#include <infra/macro.h>




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
    uint32 num_set = set( addr);
    Addr   num_tag = tag( addr);

    auto result = read_no_touch( addr);

    if ( result.first)
        lru_module.touch( num_set, num_tag); // update LRU if it's a hit

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

    const auto&[ lru_num_tag, num_way]  = lru_module.update( num_set, num_tag);
    if ( num_tag != lru_num_tag)
        data[ num_set].erase( lru_num_tag);

    data[ num_set].emplace( num_tag, num_way);

    return num_way;
}