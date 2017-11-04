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

#include <map>

template <typename K, typename V>
class InstrCache
{
    public:
        using iterator = typename std::map<K, V>::iterator;

        iterator begin() { return cache.begin(); }
        iterator end() { return cache.end(); }

        iterator find( const K& key) { return cache.find( key); }
        void update( const K& key, const V& value) 
        {
            // it is possible to change values or add new ones
            // only if number of elements is not greater than capacity
            if (number_of_elements < CAPACITY)
            {
                auto result{ cache.insert_or_assign( key, value)};

                if ( result.second)
                    number_of_elements++;
            }
        } 

        std::size_t size() const { return number_of_elements; }
        bool empty() const { return !number_of_elements; }
        
        static const std::size_t CAPACITY = 8192;

        
    private:
        std::map<K, V> cache{};
        std::size_t number_of_elements = 0;

};



#endif // INSTR_CACHE_H