/**
 * A cache for decoded instructions in functional simulator
 * @author Denis Los
*/


#ifndef INSTR_CACHE_H
#define INSTR_CACHE_H

#include <infra/types.h>

#include <unordered_map>

template <typename V>
class InstrCache
{
    public:
        InstrCache() { cache.reserve( CAPACITY); }

        auto begin() const { return cache.cbegin(); }
        auto end() const { return cache.cend(); }
        auto find( Addr key) const { return cache.find( key); }

        void update( Addr key, const V& value) 
        {
            // it is possible to change values or add new ones
            // only if number of elements is not greater than capacity
            if (number_of_elements < CAPACITY)
            {
                auto result = cache.insert_or_assign( key, value);

                if ( result.second)
                    number_of_elements++;
            }
        } 
        void erase( Addr key) 
        {
            if ( cache.erase( key))
                number_of_elements--; 
        }

        std::size_t size() const { return number_of_elements; }
        bool empty() const { return number_of_elements == 0u; }
        
        static const std::size_t CAPACITY = 8192;

        
    private:
        std::unordered_map<Addr, V> cache{};
        std::size_t number_of_elements = 0;
};



#endif // INSTR_CACHE_H