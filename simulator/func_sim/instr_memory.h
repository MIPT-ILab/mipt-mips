/**
 * A cache for decoded instructions in functional simulator
 * @author Denis Los
*/

#ifndef INSTR_CACHE_H
#define INSTR_CACHE_H

#include <infra/instrcache/instr_cache.h>
#include <infra/types.h>

#include <memory/memory.h>

template<typename FuncInstr>
class InstrMemoryIface
{
public:
    explicit InstrMemoryIface( std::endian e) : endian( e) { }
    auto fetch( Addr pc) const
    {
        return endian == std::endian::little
            ? mem->read<uint32, std::endian::little>( pc)
            : mem->read<uint32, std::endian::big>( pc);
    }
    auto get_endian() const { return endian; }

    void set_memory( const std::shared_ptr<ReadableMemory>& m) { mem = m; }
    virtual FuncInstr fetch_instr( Addr PC) = 0;

    virtual ~InstrMemoryIface() = default;
    InstrMemoryIface( const InstrMemoryIface&) = delete;
    InstrMemoryIface( InstrMemoryIface&&) noexcept = default;
    InstrMemoryIface& operator=( const InstrMemoryIface&) = delete;
    InstrMemoryIface& operator=( InstrMemoryIface&&) noexcept = default;

private:
    std::shared_ptr<ReadableMemory> mem = nullptr;
    const std::endian endian;
};

template<typename ISA>
class InstrMemory : public InstrMemoryIface<typename ISA::FuncInstr>
{
public:
    using Instr = typename ISA::FuncInstr;
    explicit InstrMemory( std::endian endian) : InstrMemoryIface<typename ISA::FuncInstr>( endian) { }
    Instr fetch_instr( Addr PC) override { return ISA::create_instr( this->fetch( PC), this->get_endian(), PC); }
};

#ifndef INSTR_CACHE_CAPACITY
#define INSTR_CACHE_CAPACITY 8192
#endif

template<typename ISA>
class InstrMemoryCached : public InstrMemory<ISA>
{
    using Instr = typename ISA::FuncInstr;
    InstrCache<Addr, Instr, INSTR_CACHE_CAPACITY, 0x0, all_ones<Addr>()> instr_cache{};
public:
    explicit InstrMemoryCached( std::endian endian) : InstrMemory<ISA>( endian) { }
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
        if ( PC != 0)
            instr_cache.update( PC, instr);
        return instr;
    }
};

#endif // INSTR_CACHE_H
