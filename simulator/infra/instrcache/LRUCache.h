/**
 * A cache for decoded instructions in functional simulator
 * @author Denis Los
*/

#ifndef LRUCACHE_H
#define LRUCACHE_H

#include <cassert>
#include <list>
#include <memory>
#include <unordered_map>
#include <utility>

#include <boost/align/aligned_alloc.hpp>
#include <infra/types.h>

template <typename Key, typename Value, size_t CAPACITY>
class LRUCache
{
        struct Deleter;
    public:
        LRUCache()
        {
            data.reserve( CAPACITY);
            lru_hash.reserve( CAPACITY);
            for (size_t i = 0; i < CAPACITY; ++i)
                free_list.emplace_back(i);
            arena = std::unique_ptr<void, Deleter>( allocate_memory());
            storage = static_cast<Value*>( arena.get());
        }

        static auto get_capacity() { return CAPACITY; }

        auto size() const { return lru_hash.size(); }
        bool empty() const { return size() == 0; }

        // First return value is true if and only if the value was found
        // Second return value is dereferenceable only if first value if 'true'
        auto find( const Key& key) const
        {
            auto result = data.find( key);
            bool found = result != data.end();
            size_t index = found ? result->second : CAPACITY;
            return std::pair<bool, const Value&>(found, storage[index]);
        }

        void touch( const Key& key)
        {
            lru_list.splice( lru_list.begin(), lru_list, lru_hash.find( key)->second);
        }

        void update( const Key& key, const Value& value)
        {
            if ( data.find( key) == data.end())
                allocate( key, value);
            else
                touch( key);
        }

        void erase( const Key& key)
        {
            auto data_it = data.find( key);
            auto lru_it  = lru_hash.find( key);
            assert ( ( data_it == data.end()) == ( lru_it == lru_hash.end()));
            if ( data_it != data.end())
            {
                assert( !empty());
                free_list.emplace_front( data_it->second);
                storage[data_it->second].~Value();
                data.erase( data_it);
                lru_list.erase( lru_it->second);
                lru_hash.erase( lru_it);
            }
        }

    private:
        void allocate( const Key& key, const Value& value)
        {
            if ( lru_hash.size() == CAPACITY)
                erase( lru_list.back());

            // Add a new element
            assert( !free_list.empty());
            auto index = free_list.front();
            free_list.pop_front();
            new (&storage[index]) Value( value);
            data.emplace( key, index);
            auto ptr = lru_list.insert( lru_list.begin(), key);
            lru_hash.emplace( key, ptr);
        }

        static void* allocate_memory()
        {
            return boost::alignment::aligned_alloc( alignof(Value), sizeof(Value) * CAPACITY);
        }

        struct Deleter
        {
            void operator()(void *p) { boost::alignment::aligned_free(p); }
        };

        std::unordered_map<Key, size_t> data{};
        std::list<Key> lru_list{};
        std::list<size_t> free_list{};
        std::unique_ptr<void, Deleter> arena = nullptr;
        Value* storage = nullptr;
        std::unordered_map<Key, typename std::list<Key>::const_iterator> lru_hash{};
};

#endif // LRUCACHE_H

