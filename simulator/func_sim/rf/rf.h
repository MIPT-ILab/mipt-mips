/*
 * rf.h - register file
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2015-2018 MIPT-MIPS
 */

#ifndef RF_H
#define RF_H

#include <array>
#include <infra/types.h>
#include <infra/wide_types.h>

#include <cassert>

template<typename ISA>
class RF
{
    using FuncInstr = typename ISA::FuncInstr;
    using Register = typename ISA::Register;
    using RegisterUInt = typename ISA::RegisterUInt;

    struct Reg {
        RegisterUInt value = 0u;
    };
    std::array<Reg, Register::MAX_REG> array = {};

    Reg& get_entry( Register num) { return array.at( num.to_size_t()); }
    const Reg& get_entry( Register num) const { return array.at( num.to_size_t()); }

protected:
    RegisterUInt read( Register num) const
    {
        assert( !num.is_mips_hi_lo());
        return static_cast<RegisterUInt>(get_entry( num).value);
    }

    template <typename T>
    void addition( T& new_val, const T& val)
    {
      const auto entry_hi = get_entry( Register::mips_hi);
      const auto entry_lo = get_entry( Register::mips_lo);
      auto hi_lo = static_cast<uint64>( entry_hi.value);
      hi_lo <<= 32;
      hi_lo += static_cast<uint64>( entry_lo.value);
      hi_lo += static_cast<uint64>( val);
      new_val = static_cast<T>( hi_lo);
    }

    template <typename T>
    void subtraction( T& new_val, const T& val)
    {
      const auto entry_hi = get_entry( Register::mips_hi);
      const auto entry_lo = get_entry( Register::mips_lo);
      auto hi_lo = static_cast<uint64>( entry_hi.value);
      hi_lo <<= 32;
      hi_lo += static_cast<uint64>( entry_lo.value);
      hi_lo -= static_cast<uint64>( val);
      new_val = static_cast<T>( hi_lo);
    }

    template <typename T>
    void write( Register num, const T& val, int8 accumulating_instr = 0)
    {
        T new_val = val;
        if ( num.is_zero())
            return;
        if( accumulating_instr == 1)
            addition(new_val, val);
        if( accumulating_instr == -1)
            subtraction(new_val, val);
        if ( num.is_mips_hi_lo()) {
            write( Register::mips_hi, static_cast<uint64>(new_val) >> 32);
            write( Register::mips_lo, new_val);
            return;
        }
        auto& entry = get_entry( num);
        entry.value = val;
    }

public:
    RF() = default;

    inline void read_source( FuncInstr* instr, uint8 index) const
    {
        instr->set_v_src( read( instr->get_src_num( index)), index);
    }

    inline void read_sources( FuncInstr* instr) const
    {
        read_source( instr, 0);
        read_source( instr, 1);
    }

    inline void write_dst( const FuncInstr& instr)
    {
        Register reg_num  = instr.get_dst_num();
        bool writes_dst = instr.get_writes_dst();
        int8 accumulating_instr = instr.is_accumulating_instr();
        if ( !reg_num.is_zero() && writes_dst)
            write( reg_num, instr.get_v_dst(), accumulating_instr);
        else
            write( reg_num, read(reg_num), accumulating_instr);
    }
};

#endif
