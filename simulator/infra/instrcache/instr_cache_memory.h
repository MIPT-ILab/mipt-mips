/**
 * A cache for decoded instructions in functional simulator
 * @author Denis Los
*/

#ifndef INSTR_CACHE_H
#define INSTR_CACHE_H

#include <infra/instrcache/LRUCache.h>
#include <infra/memory/memory.h>
#include <infra/types.h>

template<typename Instr>
class InstrMemory : public FuncMemory
{
public:
    auto fetch( Addr pc) const { return read<uint32, Instr::endian>( pc); }
    auto fetch_instr( Addr PC) { return Instr( fetch( PC), PC); }
};

#ifndef INSTR_CACHE_CAPACITY
#define INSTR_CACHE_CAPACITY 8192
#endif

template<typename Instr>
class InstrMemoryCached : public InstrMemory<Instr>
{
    LRUCache<Addr, Instr, INSTR_CACHE_CAPACITY> instr_cache{};
public:
    Instr fetch_instr( Addr PC)
    {
        const auto [found, value] = instr_cache.find( PC);
        if ( found && value.is_same_bytes( this->fetch( PC))) {
            instr_cache.touch( PC);
            return value;
        }
        if ( found)
            instr_cache.erase( PC);

        const Instr& instr = InstrMemory<Instr>::fetch_instr( PC);
        instr_cache.update( PC, instr);
        return instr;
    }
};

#endif // INSTR_CACHE_H
