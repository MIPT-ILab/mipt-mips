/**
 * A cache for decoded instructions in functional simulator
 * @author Denis Los
*/

#ifndef INSTR_CACHE_H
#define INSTR_CACHE_H

#include <infra/exception.h>
#include <infra/instrcache/LRUCache.h>
#include <infra/memory/memory.h>
#include <infra/types.h>

#ifndef INSTR_CACHE_CAPACITY
#define INSTR_CACHE_CAPACITY 8192
#endif

template<typename Instr>
class InstrMemory : private FuncMemory
{
    private:
        LRUCache<Addr, Instr, INSTR_CACHE_CAPACITY> instr_cache{};

    public:
        using FuncMemory::startPC;
        using FuncMemory::load_elf_file;

        auto fetch( Addr pc) const { return read<uint32>( pc); }

        Instr fetch_instr( Addr PC)
        {
            const auto [found, value] = instr_cache.find( PC);
            Instr instr = found ? value : Instr( fetch( PC), PC);
            instr_cache.update( PC, instr);
            return instr;
        }

        void load( Instr* instr) const
        {
            using DstType = decltype(instr->get_v_dst());
            instr->set_v_dst(read<DstType>(instr->get_mem_addr(), bitmask<DstType>(instr->get_mem_size() * 8)));
        }

        void store( const Instr& instr)
        {
            if (instr.get_mem_addr() == 0)
                throw Exception("Store data to zero is an unhandled trap");
            instr_cache.erase( instr.get_mem_addr());
            write( instr.get_v_src2(), instr.get_mem_addr(), instr.get_mask());
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
