/**
 * CacheTagArray
 * @author Oleg Ladin, Denis Los
 */

#ifndef CACHE_TAG_ARRAY_H
#define CACHE_TAG_ARRAY_H


#include <infra/cache/LRUTagCache.h>
#include <infra/types.h>
#include <infra/log.h>

#include <vector>
#include <map>
#include <utility>


class CacheTagArray : private Log
{
    public:
        CacheTagArray( uint32 size, uint32 ways)
            : Log( false)
            , number_of_sets( check_arguments( size, ways))
            , number_of_ways( ways)
            , cache( number_of_sets, LRUTagCache<Addr, uint32>( number_of_ways))  
        { }

        std::pair<Addr, uint32> read( Addr addr);
        std::pair<Addr, uint32> read_no_touch( Addr addr) const;
        uint32 write( Addr addr);

        uint32 set( Addr addr) const { return addr & (number_of_sets - 1); }
    
    private:
        // If arguments are correct, it returns the number of sets
        uint32 check_arguments( uint32 size, uint32 ways); 

    private:
        const uint32 number_of_sets;
        const uint32 number_of_ways;

        std::vector<LRUTagCache<Addr, uint32>> cache;
};

#endif // CACHE_TAG_ARRAY_H