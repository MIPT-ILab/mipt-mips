/**
 * CacheTagArray
 * @author Oleg Ladin, Denis Los
 */

#include <cassert>


#include <infra/cache/cache_tag_array.h>
#include <infra/macro.h>



uint32 CacheTagArray::check_arguments( uint32 size_in_bytes, 
                                       uint32 ways,
                                       uint32 size_of_line,
                                       uint32 addr_size_in_bits)
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


    return size_in_bytes / ( size_of_line * ways);
}



std::pair<bool, uint32> CacheTagArray::read( Addr addr)
{
    uint32 num_set = set( addr);
    uint32 num_tag = tag( addr);

    auto result = cache[ num_set].find( num_tag);
    if ( result.first)
        cache[ num_set].update( num_tag); // update LRU if it's a hit

    return result;    
}



std::pair<bool, uint32> CacheTagArray::read_no_touch( Addr addr) const
{
    uint32 num_set = set( addr);
    uint32 num_tag = tag( addr);

    return cache[ num_set].find( num_tag);
}



uint32 CacheTagArray::write( Addr addr)
{
    uint32 num_set = set( addr);
    uint32 num_tag = tag( addr);

    cache[ num_set].update( num_tag);
    const auto&[ is_hit, value] = cache[ num_set].find( num_tag);
    assert( is_hit);

    return value;
}