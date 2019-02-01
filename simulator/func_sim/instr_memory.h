/**
 * A cache for decoded instructions in functional simulator
 * @author Denis Los
*/

#ifndef INSTR_CACHE_H
#define INSTR_CACHE_H

#include <infra/instrcache/LRUCache.h>
#include <infra/types.h>

#include <memory/memory.h>

template<typename ISA>
class InstrMemory
{
    std::shared_ptr<ReadableMemory> mem;
public:
    void set_memory( std::shared_ptr<ReadableMemory> m) { mem = std::move( m); }
    auto fetch( Addr pc) const { return mem->read<uint32, ISA::FuncInstr::endian>( pc); }
    auto fetch_instr( Addr PC) { return ISA::create_instr( fetch( PC), PC); }
};

#ifndef INSTR_CACHE_CAPACITY
#define INSTR_CACHE_CAPACITY 8192
#endif

template<typename ISA>
class InstrMemoryCached : public InstrMemory<ISA>
{
    LRUCache<Addr, typename ISA::FuncInstr, INSTR_CACHE_CAPACITY> instr_cache{};
public:
    auto fetch_instr( Addr PC)
    {
        const auto [found, value] = instr_cache.find( PC);
        if ( found && value.is_same_bytes( this->fetch( PC))) {
            instr_cache.touch( PC);
            return value;
        }
        if ( found)
            instr_cache.erase( PC);

        auto instr = InstrMemory<ISA>::fetch_instr( PC);
        instr_cache.update( PC, instr);
        return instr;
    }
};

#endif // INSTR_CACHE_H
