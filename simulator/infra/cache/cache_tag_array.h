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
        CacheTagArray( uint32 size_in_bytes, 
                       uint32 ways,
                       uint32 size_of_line = 4,
                       uint32 addr_size_in_bits = 32)
            : Log( false)
            , number_of_sets( check_arguments( size_in_bytes, 
                                               ways,
                                               size_of_line,
                                               addr_size_in_bits))
            , number_of_ways( ways)
            , line_size( size_of_line)
            , cache( number_of_sets, LRUTagCache<Addr>( number_of_ways))  
        { }

        std::pair<bool, uint32> read( Addr addr);
        std::pair<bool, uint32> read_no_touch( Addr addr) const;
        uint32 write( Addr addr);

        uint32 set( Addr addr) const { return (addr / line_size) & (number_of_sets - 1); }
        uint32 tag( Addr addr) const { return (addr / line_size); }
    
    private:
        // If arguments are correct, it returns the number of sets
        uint32 check_arguments( uint32 size_in_bytes, 
                                uint32 ways,
                                uint32 size_of_line,
                                uint32 addr_size_in_bits); 

    private:
        const uint32 number_of_sets;
        const uint32 number_of_ways;
        const uint32 line_size;

        std::vector<LRUTagCache<Addr>> cache;
};

#endif // CACHE_TAG_ARRAY_H