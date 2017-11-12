/**
 * A cache for decoded instructions in functional simulator
 * @author Denis Los
*/


#ifndef INSTR_CACHE_H
#define INSTR_CACHE_H

#include <infra/types.h>

#include <unordered_map>

#include <infra/instrcache/LRUCache.h>

template <typename V>
class InstrCache : public LRUCache<Addr, V>
{
    public:
        InstrCache() : LRUCache<Addr, V>(CAPACITY) {}
        
        static const std::size_t CAPACITY = 8192;
};



#endif // INSTR_CACHE_H