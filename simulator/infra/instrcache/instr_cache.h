/**
 * A cache for decoded instructions in functional simulator
 * @author Denis Los
*/

#ifndef INSTRCACHE_H
#define INSTRCACHE_H

#include <infra/arena.h>
#include <infra/replacement/cache_replacement.h>
#include <infra/types.h>

#include <sparsehash/dense_hash_map.h>

#include <memory>
#include <utility>
#include <vector>

template <typename Key, typename Value, size_t CAPACITY, Key INVALID_KEY, Key DELETED_KEY>
class InstrCache
{
    public:
        InstrCache() : pointers( CAPACITY)
        {
            pointers.set_empty_key( INVALID_KEY);
            pointers.set_deleted_key( DELETED_KEY);
            lru_module = create_cache_replacement( "LRU", CAPACITY);
            // Touch everything to initialize order
            for (size_t i = 0; i < CAPACITY; ++i)
                lru_module->touch( i);

            keys.resize( CAPACITY);
            storage.allocate( CAPACITY);
        }

        ~InstrCache()
        {
            for ( const auto& k : pointers)
                storage.destroy( k.second);
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
            lru_module->touch( pointers[key]);
        }

        void update( const Key& key, const Value& value)
        {
            if ( pointers.find( key) == pointers.end())
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
                pointers.erase( data_it);
                lru_module->set_to_erase( index);
            }
        }

    private:
        void allocate( const Key& key, const Value& value)
        {
            const size_t index = lru_module->update();
            erase( keys[index]);

            storage.emplace( index, value);
            keys[index] = key;
            pointers.emplace( key, index);
            lru_module->touch( index);
        }

        std::vector<Key> keys{};
        google::dense_hash_map<Key, size_t> pointers{};
        Arena<Value> storage{};
        std::unique_ptr<CacheReplacement> lru_module;
};

#endif // INSTRCACHE_H
