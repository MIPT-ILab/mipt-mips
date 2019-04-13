/**
 * A cache for decoded instructions in functional simulator
 * @author Denis Los
*/

#ifndef INSTR_CACHE_H
#define INSTR_CACHE_H

#include <infra/instrcache/InstrCache.h>
#include <infra/types.h>

#include <memory/memory.h>

template<typename FuncInstr>
class InstrMemoryIface
{
protected:
    std::shared_ptr<ReadableMemory> mem = nullptr;
    const Endian endian;
public:
    explicit InstrMemoryIface( Endian e) : endian( e) { }
    auto fetch( Addr pc) const
    {
        return endian == Endian::little
            ? mem->read<uint32, Endian::little>( pc)
            : mem->read<uint32, Endian::big>( pc);
    }

    void set_memory( const std::shared_ptr<ReadableMemory>& m) { mem = m; }
    virtual FuncInstr fetch_instr( Addr PC) = 0;

    virtual ~InstrMemoryIface() = default;
    InstrMemoryIface( const InstrMemoryIface&) = delete;
    InstrMemoryIface( InstrMemoryIface&&) noexcept = default;
    InstrMemoryIface& operator=( const InstrMemoryIface&) = delete;
    InstrMemoryIface& operator=( InstrMemoryIface&&) noexcept = default;
};

template<typename ISA>
class InstrMemory : public InstrMemoryIface<typename ISA::FuncInstr>
{
public:
    using Instr = typename ISA::FuncInstr;
    explicit InstrMemory( Endian endian) : InstrMemoryIface<typename ISA::FuncInstr>( endian) { }
    Instr fetch_instr( Addr PC) override { return ISA::create_instr( this->fetch( PC), this->endian, PC); }
};

#ifndef INSTR_CACHE_CAPACITY
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage) We keep it as a macro to easily it from makefile
#define INSTR_CACHE_CAPACITY 8192
#endif

template<typename ISA>
class InstrMemoryCached : public InstrMemory<ISA>
{
    using Instr = typename ISA::FuncInstr;
    InstrCache<Addr, Instr, INSTR_CACHE_CAPACITY> instr_cache{};
public:
    explicit InstrMemoryCached( Endian endian) : InstrMemory<ISA>( endian) { }
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
