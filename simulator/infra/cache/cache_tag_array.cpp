/**
 * CacheTagArray
 * @author Oleg Ladin, Denis Los
 */

#include <cassert>


#include <infra/cache/cache_tag_array.h>
#include <infra/macro.h>



uint32 CacheTagArray::check_arguments( uint32 size, uint32 ways)
{
    if ( size == 0)
        serr << "ERROR: Wrong arguments! Cache size should be greater than zero"
             << std::endl << critical;
    
    if ( ways == 0)
        serr << "ERROR: Wrong arguments! Num of ways should be greater than zero"
             << std::endl << critical;
    
    if ( size < ways)
        serr << "ERROR: Wrong arguments! Cache size should be greater"
             << "than the number of ways" 
             << std::endl << critical;

    if ( !is_power_of_two( size))
        serr << "ERROR: Wrong argumets! Cache size should be a power of 2"
             << std::endl << critical;         
    
    if ( size % ways != 0)
        serr << "ERROR: Wrong arguments! Cache size should be multiple of"
             << "the number of ways"
             << std::endl << critical;

    return size / ways;
}



std::pair<Addr, uint32> CacheTagArray::read( Addr addr)
{
    uint32 num_set = set( addr);

    auto result = cache[ num_set].find( addr);
    if ( result.first)
        cache[ num_set].update( addr); // update LRU if it's a hit

    return result;    
}



std::pair<Addr, uint32> CacheTagArray::read_no_touch( Addr addr) const
{
    uint32 num_set = set( addr);
    return cache[ num_set].find( addr);
}



uint32 CacheTagArray::write( Addr addr)
{
    uint32 num_set = set( addr);

    cache[ num_set].update( addr);
    const auto&[ is_hit, value] = cache[ num_set].find( addr);
    assert( is_hit);

    return value;
}