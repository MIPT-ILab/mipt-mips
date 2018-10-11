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

template<typename Instr>
class InstrMemory : public FuncMemory
{
public:
        using DstType = decltype(std::declval<Instr>().get_v_dst());

        auto fetch( Addr pc) const { return read<uint32>( pc); }
        auto fetch_instr( Addr PC) { return Instr( fetch( PC), PC); }

        void load( Instr* instr) const
        {
            auto mask = bitmask<DstType>(instr->get_mem_size() * CHAR_BIT);
            auto value = read<DstType>(instr->get_mem_addr(), mask);
            instr->set_v_dst( value);
        }

        void store( const Instr& instr)
        {
            if (instr.get_mem_addr() == 0)
                throw Exception("Store data to zero is an unhandled trap");

            if (~instr.get_mask() == 0)
                write<DstType>( instr.get_v_src2(), instr.get_mem_addr());
            else
                write<DstType>( instr.get_v_src2(), instr.get_mem_addr(), instr.get_mask());
        }

        void load_store(Instr* instr)
        {
            if (instr->is_load())
                load(instr);
            else if (instr->is_store())
                store(*instr);
        }
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
            auto true_bytes = this->fetch( PC);
            if ( found && value.is_same_bytes( true_bytes)) {
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
