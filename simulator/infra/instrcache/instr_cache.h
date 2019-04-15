/**
 * A cache for decoded instructions in functional simulator
 * @author Denis Los
*/

#ifndef INSTRCACHE_H
#define INSTRCACHE_H

#include <bitset>
#include <cassert>
#include <memory>
#include <type_traits>
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
            data.reserve( CAPACITY);
            storage.allocate( CAPACITY);
        }

        ~InstrCache()
        {
            for ( size_t i = 0; i < CAPACITY; ++i)
                if ( valid_elements[i])
                    storage.destroy( i);
        }

        InstrCache(const InstrCache&) = delete;
        InstrCache(InstrCache&&) = delete;
        InstrCache& operator=(const InstrCache&) = delete;
        InstrCache& operator=(InstrCache&&) = delete;

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
            auto data_it = data.find( key);
            if ( data_it != data.end())
            {
                storage.destroy( data_it->second);
                valid_elements[ data_it->second] = false;
                data.erase( data_it);
                lru_module->set_to_erase( key);
            }
        }

    private:
        void allocate( const Key& key, const Value& value)
        {
            if ( size() == CAPACITY)
                // FIXME(pikryukov): think about this.
                erase( narrow_cast<Key>( lru_module->update()));

            // Add a new element
            auto index = get_first_empty();
            storage.emplace( index, value);
            valid_elements[index] = true;
            data.emplace( key, index);
            lru_module->allocate( key);
        }

        auto get_first_empty() const noexcept
        {
            size_t i = 0;
            for ( ; i < CAPACITY; ++i)
                if ( valid_elements[i])
                    continue;

            return i;
        }

        std::unordered_map<Key, size_t> data{};
        Arena<Value> storage{};
        std::bitset<CAPACITY> valid_elements{};
        std::unique_ptr<CacheReplacementInterface> lru_module;
};

#endif // INSTRCACHE_H

