/**
 * A cache for decoded instructions in functional simulator
 * @author Denis Los
*/

#ifndef INSTR_CACHE_H
#define INSTR_CACHE_H

#include <infra/instrcache/LRUCache.h>
#include <infra/types.h>

#include <memory/memory.h>

template<typename FuncInstr>
class InstrMemoryIface
{
protected:
    std::shared_ptr<ReadableMemory> mem;
public:
    void set_memory( std::shared_ptr<ReadableMemory> m) { mem = std::move( m); }
    virtual FuncInstr fetch_instr( Addr PC) = 0;
};

template<typename ISA>
class InstrMemory : public InstrMemoryIface<typename ISA::FuncInstr>
{
public:
    using Instr = typename ISA::FuncInstr;
    auto fetch( Addr pc) const { return this->mem->template read<uint32, Instr::endian>( pc); }
    Instr fetch_instr( Addr PC) override { return ISA::create_instr( fetch( PC), PC); }
};

#ifndef INSTR_CACHE_CAPACITY
#define INSTR_CACHE_CAPACITY 8192
#endif

template<typename ISA>
class InstrMemoryCached : public InstrMemory<ISA>
{
    using Instr = typename ISA::FuncInstr;
    LRUCache<Addr, Instr, INSTR_CACHE_CAPACITY> instr_cache{};
public:
    Instr fetch_instr( Addr PC) final
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
