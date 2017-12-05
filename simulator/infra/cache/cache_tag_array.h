/**
 * cache_tag_array.h
 * Header for the cache tag array model.
 * @author Oleg Ladin, Denis Los
 */

#ifndef CACHE_TAG_ARRAY_H
#define CACHE_TAG_ARRAY_H


#include <infra/types.h>
#include <infra/log.h>
#include <infra/macro.h>

#include <vector>
#include <unordered_map>
#include <utility>
#include <list>



// Replacement algorithm modules (LRU)
class  LRUCacheInfo
{
    public:
        explicit LRUCacheInfo( std::size_t capacity)
            : CAPACITY( capacity)
        {
            assert( capacity != 0u);

            lru_hash.reserve( CAPACITY);
        }

        auto get_capacity() const { return CAPACITY; }
        auto size() const { return number_of_elements; }
        bool empty() const { return size() == 0u; }

        void touch( std::size_t key);
        std::size_t update();


    private:
        std::list<std::size_t> lru_list{};
        std::unordered_map<std::size_t, typename std::list<std::size_t>::const_iterator> lru_hash{};

        std::size_t number_of_elements = 0u;
        std::size_t CAPACITY;
};



class LRUModule
{
    public:
        LRUModule( std::size_t number_of_sets, std::size_t number_of_ways)
            : lru_info( number_of_sets, LRUCacheInfo( number_of_ways))
        { }

        void touch( uint32 num_set, uint32 num_way) { lru_info[ num_set].touch( num_way); }
        
        uint32 update( uint32 num_set) { return lru_info[ num_set].update(); }

    private:
        std::vector<LRUCacheInfo> lru_info;
};






// Cache tag array module implementation
class CacheTagArrayCheck : private Log
{
    protected:
        CacheTagArrayCheck( uint32 size_in_bytes,
                            uint32 ways,
                            uint32 size_of_line,
                            uint32 addr_size_in_bits);
};



class CacheTagArray : public CacheTagArrayCheck
{
    public:
        CacheTagArray( uint32 size_in_bytes, 
                       uint32 ways,
                       uint32 size_of_line = 4,
                       uint32 addr_size_in_bits = 32)

            : CacheTagArrayCheck( size_in_bytes, 
                                  ways,
                                  size_of_line,
                                  addr_size_in_bits)
            , number_of_sets( size_in_bytes / ( ways * size_of_line))
            , number_of_ways( ways)
            , line_size( size_of_line)
            , addr_mask( bitmask<Addr>( addr_size_in_bits))
            , ways_to_tags( number_of_sets, std::unordered_map<uint32, Addr>{})
            , data( number_of_sets, std::unordered_map<Addr, uint32>{})
            , lru_module( number_of_sets, number_of_ways) 
        { 
            for ( std::size_t i = 0; i < number_of_sets; i++)
            {
                data[i].reserve( number_of_ways);
                ways_to_tags[i].reserve( number_of_ways);
            }    
        }

        // lookup the cache and update LRU info
        std::pair<bool, uint32> read( Addr addr);
        // find in the cache but do not update LRU info
        std::pair<bool, uint32> read_no_touch( Addr addr) const;
        // create new entry in cache
        uint32 write( Addr addr);

        uint32 set( Addr addr) const 
        { 
            return ( ( addr & addr_mask) / line_size) & (number_of_sets - 1); 
        }
        Addr tag( Addr addr) const { return ( addr & addr_mask) / line_size; }

    private:
        const uint32 number_of_sets;
        const uint32 number_of_ways;
        const uint32 line_size;
        const Addr   addr_mask;

        // maps to convert num_ways to tags
        std::vector<std::unordered_map<uint32, Addr>> ways_to_tags;

        std::vector<std::unordered_map<Addr, uint32>> data; // tags
        LRUModule lru_module; // LRU algorithm module
};

#endif // CACHE_TAG_ARRAY_H