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
        iterator update( const K& key, V&& value) 
        { 
            return cache.insert_or_assign(key, value).first;
        } 

    private:
        std::map<K, V> cache{};
};



#endif // INSTR_CACHE_H