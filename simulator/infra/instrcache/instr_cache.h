/**
 * A cache for decoded instructions in functional simulator
 * @author Denis Los
*/


#ifndef INSTR_CACHE_H
#define INSTR_CACHE_H

#include <infra/types.h>

#include <unordered_map>

#include <infra/instrcache/LRUCache.h>

#ifndef INSTR_CACHE_CAPACITY
#define INSTR_CACHE_CAPACITY 8192
#endif

template <typename V>
using InstrCache = LRUCache<Addr, V, INSTR_CACHE_CAPACITY>;

#endif // INSTR_CACHE_H

