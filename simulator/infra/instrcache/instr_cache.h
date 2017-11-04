/*
* This is an open source non-commercial project. Dear PVS-Studio, please check it.
* PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
*/
/**
 * A cache for decoded instructions in functional simulator
 * @author Denis Los
*/


#ifndef INSTR_CACHE_H
#define INSTR_CACHE_H

#include <infra/types.h>

#include <unordered_map>
#include <utility>

template <typename V>
class InstrCache
{
    public:
        using value_type = typename std::pair<bool, V>;
        using iterator = typename std::unordered_map<Addr, value_type>::iterator;
        
        auto begin() const { return cache.cbegin(); }
        auto end() const { return cache.cend(); }

        iterator find( Addr key) { return cache.find( key); }
        void update( Addr key, const V& value) 
        {
            // it is possible to change values or add new ones
            // only if number of elements is not greater than capacity
            if (number_of_elements < CAPACITY)
            {
                auto result{ cache.insert_or_assign( key, std::make_pair( true, value))};

                if ( result.second)
                    number_of_elements++;
            }
        } 
        void mark_invalid( Addr key)
        {
            iterator it = cache.find( key);
            if ( it != cache.end())
                it -> second.first = false;
        }

        std::size_t size() const { return number_of_elements; }
        bool empty() const { return number_of_elements == 0u; }
        
        static const std::size_t CAPACITY = 8192;

        
    private:
        std::unordered_map<Addr, value_type> cache{};
        std::size_t number_of_elements = 0;
};



#endif // INSTR_CACHE_H