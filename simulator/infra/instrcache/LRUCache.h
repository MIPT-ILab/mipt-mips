/**
 * A cache for decoded instructions in functional simulator
 * @author Denis Los
*/

#ifndef LRUCACHE_H
#define LRUCACHE_H


#include <list>
#include <unordered_map>
#include <utility>


template <typename Key, typename Value>
class LRUCache
{
    public:
        explicit LRUCache( std::size_t capacity) 
            : CAPACITY( capacity) 
        { 
            mp.reserve( capacity);
        }
        
        auto begin() const { return mp.cbegin(); }
        auto end() const { return mp.cend(); }
        auto find( const Key& key) const { return mp.find( key); }
        
        std::size_t size() const { return number_of_elements; }
        bool empty() const { return number_of_elements == 0u; }

        void update( const Key& key, const Value& value)
        {
            auto it = mp.find( key);
            if ( it == mp.end())
            {
                if ( number_of_elements == CAPACITY)
                {
                    mp.erase( cache.back().first);
                    cache.pop_back();
                    mp.emplace( key, cache.insert( cache.begin(), std::make_pair( key, value)));
                }
                else
                {
                    mp.emplace( key, cache.insert( cache.begin(), std::make_pair( key, value)));
                    number_of_elements++;
                }
            }
            else
            {
                (*it -> second).second = value;
                cache.splice( cache.begin(), cache, it -> second);
            }
        }

        void erase( const Key& key)
        {
            auto it = mp.find( key);
            if ( it != mp.end())
            {
                cache.erase( it -> second);
                mp.erase( it);

                number_of_elements--;
            }
        }

    private:
        std::list<std::pair<Key, Value>> cache{};
        std::unordered_map<Key, typename std::list<std::pair<Key, Value>>::iterator> mp{};

        std::size_t number_of_elements = 0u;
        const std::size_t CAPACITY;
};


#endif // LRUCACHE_H

