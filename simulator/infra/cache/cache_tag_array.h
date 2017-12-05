/**
 * cache_tag_array.h
 * Header for the cache tag array model.
 * @author Oleg Ladin, Denis Los
 */

#ifndef CACHE_TAG_ARRAY_H
#define CACHE_TAG_ARRAY_H


#include <infra/types.h>
#include <infra/log.h>

#include <vector>
#include <unordered_map>
#include <utility>
#include <list>



// Replacement algorithm modules (LRU)
template <typename Key>
class  LRUCacheInfo
{
    public:
        explicit LRUCacheInfo( std::size_t capacity)
            : CAPACITY( capacity)
        {
            assert( capacity != 0u);

            data.reserve( CAPACITY);
            lru_hash.reserve( CAPACITY);
        }

        auto get_capacity() const { return CAPACITY; }
        auto size() const { return number_of_elements; }
        bool empty() const { return size() == 0u; }


        // if an element with a given key is present in the lru module
        // or the number of elements is less than CAPACITY,
        // a passed key is returned as the first value in a pair

        // However, if the number of elements is equal to CAPACITY
        // and an element with a given key is not present in the lru module
        // a key of the least recently used element is returned 
        // as the first value in a pair
        std::pair<Key, std::size_t> update( const Key& key)
        {
            std::pair<Key, std::size_t> return_value;

            auto data_it = data.find( key);
            auto lru_it  = lru_hash.find( key);
            assert( ( data_it == data.end()) == ( lru_it == lru_hash.end()));

            if ( data_it == data.end())
                return_value = allocate( key);
            else
            {
                lru_list.splice( lru_list.begin(), lru_list, lru_it->second);
                return_value = std::pair<Key, std::size_t>( key, data_it->second);
            }

            return return_value;    
        }


    private:
        // if the number of elements is equal to CAPACITY,
        // a key of the least recently used elements is returned 
        // as the first value in a pair

        // if the number of elements is less than CAPACITY,
        // a passed key is returned as the first value in a pair
        std::pair<Key, std::size_t> allocate( const Key& key)
        {
            std::size_t tag_value = 0;
            Key key_value = key;

            if ( number_of_elements == CAPACITY)
            {
                // Delete least recently used element and save its key and value
                const auto& lru_elem = lru_list.back();
                
                tag_value = data.find( lru_elem)->second;
                key_value = lru_elem;
                
                lru_hash.erase( lru_elem);
                data.erase( lru_elem);
                lru_list.pop_back();
            }
            else
            {
                tag_value = number_of_elements;
                number_of_elements++;
            }

            data.emplace( key, tag_value);
            auto ptr = lru_list.insert( lru_list.begin(), key);
            lru_hash.emplace( key, ptr);

            return std::pair<Key, std::size_t>( key_value, tag_value);
        }

    private:
        std::unordered_map<Key, std::size_t> data{};

        std::list<Key> lru_list{};
        std::unordered_map<Key, typename std::list<Key>::const_iterator> lru_hash{};

        std::size_t number_of_elements = 0u;
        std::size_t CAPACITY;
};



class LRUModule
{
    public:
        LRUModule( std::size_t number_of_sets, std::size_t number_of_ways)
            : lru_info( number_of_sets, LRUCacheInfo<Addr>( number_of_ways))
        { }

        std::pair<Addr, uint32> update( uint32 num_set, Addr addr) 
        { 
            return lru_info[ num_set].update( addr);
        }

    private:
        std::vector<LRUCacheInfo<Addr>> lru_info;
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
            , data( number_of_sets, std::unordered_map<Addr, uint32>{})
            , lru_module( number_of_sets, number_of_ways) 
        { 
            for ( auto& map_of_ways : data)
                map_of_ways.reserve( number_of_ways);
        }

        // lookup the cache and update LRU info
        std::pair<bool, uint32> read( Addr addr);
        // find in the cache but do not update LRU info
        std::pair<bool, uint32> read_no_touch( Addr addr) const;
        // create new entry in cache
        uint32 write( Addr addr);

        uint32 set( Addr addr) const { return (addr / line_size) & (number_of_sets - 1); }
        uint32 tag( Addr addr) const { return (addr / line_size); }

    private:
        const uint32 number_of_sets;
        const uint32 number_of_ways;
        const uint32 line_size;

        std::vector<std::unordered_map<Addr, uint32>> data; // tags
        LRUModule lru_module; // LRU algorithm module
};

#endif // CACHE_TAG_ARRAY_H