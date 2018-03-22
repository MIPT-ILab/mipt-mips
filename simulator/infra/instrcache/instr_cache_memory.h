/**
 * A cache for decoded instructions in functional simulator
 * @author Denis Los
*/

#ifndef INSTR_CACHE_H
#define INSTR_CACHE_H

#include <infra/types.h>
#include <infra/instrcache/LRUCache.h>
#include <infra/memory/memory.h>

#ifndef INSTR_CACHE_CAPACITY
#define INSTR_CACHE_CAPACITY 8192
#endif

template<typename Instr>
class InstrMemory : private FuncMemory
{
    private:
        LRUCache<Addr, Instr, INSTR_CACHE_CAPACITY> instr_cache{};

    public:
        explicit InstrMemory( const std::string& tr) : FuncMemory( tr) { }

        using FuncMemory::startPC;

        uint32 fetch( Addr pc) const { return read( pc); }

        Instr fetch_instr( Addr PC)
        {
            // NOLINTNEXTLINE(clang-analyzer-deadcode) https://bugs.llvm.org/show_bug.cgi?id=36283
            const auto [found, value] = instr_cache.find( PC);
            Instr instr = found ? value : Instr( fetch( PC), PC);
            instr_cache.update( PC, instr);
            return instr;
        }

        void load( Instr* instr) const
        {
            instr->set_v_dst(read(instr->get_mem_addr(), instr->get_mem_size()));
        }

        void store( const Instr& instr)
        {
            instr_cache.erase( instr.get_mem_addr());
            // potential bug for RISCV128
            write(static_cast<uint64>(instr.get_v_src2()), instr.get_mem_addr(), instr.get_mem_size());
        }

        void load_store(Instr* instr)
        {
            if (instr->is_load())
                load(instr);
            else if (instr->is_store())
                store(*instr);
        }
};

#endif // INSTR_CACHE_H

