/**
 * LRU cache module for CacheTagArray
 * @author Denis Los
*/

#ifndef LRUTAGCACHE_H
#define LRUTAGCACHE_H

#include <cassert>

#include <list>
#include <unordered_map>
#include <utility>


template <typename Key>
class LRUTagCache
{
    public:
        explicit LRUTagCache( std::size_t capacity)
            : CAPACITY( capacity)
        {
            assert( capacity != 0u);

            data.reserve( CAPACITY);
            lru_hash.reserve( CAPACITY);
        }

        auto get_capacity() const { return CAPACITY; }
        auto size() const { return number_of_elements; }
        bool empty() const { return size() == 0u; }

        auto find( const Key& key) const
        {   
            auto result = data.find( key);
            return ( result != data.end()) 
                     ? std::pair<bool, std::size_t>( true, result -> second)
                     : std::pair<bool, std::size_t>( false, 0u);
        }

        void update( const Key& key)
        {
            auto data_it = data.find( key);
            auto lru_it  = lru_hash.find( key);
            assert( ( data_it == data.end()) == ( lru_it == lru_hash.end()));
            
            if ( data_it == data.end())
                allocate( key);
            else
                lru_list.splice( lru_list.begin(), lru_list, lru_it->second);
        }


    private:
        void allocate( const Key& key)
        {
            std::size_t value = 0;

            if ( number_of_elements == CAPACITY)
            {
                // Delete least recently used element and save its value
                const auto& lru_elem = lru_list.back();
                lru_hash.erase( lru_elem);
                value = data.find( lru_elem) -> second; 
                data.erase( lru_elem);
                lru_list.pop_back();
            }
            else 
            {
                value = number_of_elements;
                number_of_elements++;
            }

            // Add a new element
            data.emplace( key, value);
            auto ptr = lru_list.insert( lru_list.begin(), key);
            lru_hash.emplace( key, ptr);
        }

    private:
        std::unordered_map<Key, std::size_t> data{};

        std::list<Key> lru_list{};
        std::unordered_map<Key, typename std::list<Key>::const_iterator> lru_hash{};

        std::size_t number_of_elements = 0u;
        std::size_t CAPACITY;
};


#endif // LRUTAGCACHE_H

