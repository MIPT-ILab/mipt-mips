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

#include <infra/types.h>
#include <infra/lru/LRUCacheInfo.h>

template <typename Key, typename Value, size_t CAPACITY>
class LRUCache
{
        struct Deleter;
    public:
        LRUCache() : lru_module ( CAPACITY)
        {
            data.reserve( CAPACITY);
            for (size_t i = 0; i < CAPACITY; ++i)
                free_list.emplace_back(i);
            arena = std::unique_ptr<void, Deleter>( allocate_memory());
            void* ptr = arena.get();
            size_t space = sizeof(Value) * (CAPACITY + 1);
            storage = static_cast<Value*>(std::align( alignof(Value), sizeof(Value) * CAPACITY, ptr, space));
        }

        ~LRUCache()
        {
            for (const auto& e : data)
                storage[ e.second].~Value();
        }

        LRUCache(const LRUCache&) = delete;
        LRUCache(LRUCache&&) = delete;
        LRUCache& operator=(const LRUCache&) = delete;
        LRUCache& operator=(LRUCache&&) = delete;

        static auto get_capacity() { return CAPACITY; }

        auto size() const { return data.size(); }
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
            lru_module.touch( key);
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
            assert ( data_it == data.end());
            if ( data_it != data.end())
            {
                free_list.emplace_front( data_it->second);
                storage[data_it->second].~Value();
                data.erase( data_it);
                lru_module.erase( key);
            }
        }

    private:
        void allocate( const Key& key, const Value& value)
        {
            if ( data.size() == CAPACITY)
                erase( lru_module.update());

            // Add a new element
            auto index = free_list.front();
            free_list.pop_front();
            new (&storage[index]) Value( value);
            data.emplace( key, index);
        }

        static void* allocate_memory()
        {
            return std::malloc( sizeof(Value) * (CAPACITY + 1));
        }

        struct Deleter
        {
            void operator()(void *p) { std::free(p); }
        };

        std::unordered_map<Key, size_t> data{};
        std::list<size_t> free_list{};
        std::unique_ptr<void, Deleter> arena = nullptr;
        Value* storage = nullptr;
        LRUCacheInfo lru_module;
};

#endif // LRUCACHE_H

