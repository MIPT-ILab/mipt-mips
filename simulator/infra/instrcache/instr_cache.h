/**
 * A cache for decoded instructions in functional simulator
 * @author Denis Los
*/

#ifndef INSTRCACHE_H
#define INSTRCACHE_H

#include <bitset>
#include <cassert>
#include <memory>
#include <unordered_map>
#include <utility>

#include <infra/arena.h>
#include <infra/types.h>
#include <infra/replacement/cache_replacement.h>

template <typename Key, typename Value, size_t CAPACITY>
class InstrCache
{
    public:
        InstrCache()
        {
            lru_module = create_cache_replacement( "LRU", CAPACITY);
            keys.resize( CAPACITY);
            pointers.reserve( CAPACITY);
            storage.allocate( CAPACITY);
        }

        ~InstrCache()
        {
            for ( const auto& k : pointers)
                 storage.destroy( pointers.second);
        }

        InstrCache(const InstrCache&) = delete;
        InstrCache(InstrCache&&) = delete;
        InstrCache& operator=(const InstrCache&) = delete;
        InstrCache& operator=(InstrCache&&) = delete;

        static auto get_capacity() { return CAPACITY; }

        auto size() const { return pointers.size(); }
        bool empty() const { return size() == 0; }

        // First return value is true if and only if the value was found
        // Second return value is dereferenceable only if first value if 'true'
        auto find( const Key& key) const
        {
            auto result = pointers.find( key);
            bool found = result != pointers.end();
            size_t index = found ? result->second : CAPACITY;
            return std::pair<bool, const Value&>(found, storage[index]);
        }

        void touch( const Key& key)
        {
            lru_module->touch( key);
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
            auto data_it = pointers.find( key);
            if ( data_it != pointers.end())
            {
                auto index = data_it->second;
                storage.destroy( index);
                keys[index] = Key{};
                data.erase( data_it);
                lru_module->set_to_erase( index);
            }
        }

    private:
        void allocate( const Key& key, const Value& value)
        {
            size_t index = lru_module->update();
            erase( keys[index]);

            storage.emplace( index, value);
            keys[index] = key;
            pointers.emplace( key, index);
            lru_module->allocate( index);
        }

        std::vector<Key> keys{};
        std::unordered_map<Key, size_t> pointers{};
        Arena<Value> storage{};
        std::unique_ptr<CacheReplacementInterface> lru_module;
};

#endif // INSTRCACHE_H

