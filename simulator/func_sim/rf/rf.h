/*
 * rf.h - register file
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2015-2018 MIPT-MIPS
 */

#ifndef RF_H
#define RF_H

#include <array>
#include <infra/types.h>

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
    auto read( Register num) const
    {
        assert( !num.is_mips_hi_lo());
        return get_entry( num).value;
    }

    void write( Register num, uint64 val)
    {
        if ( num.is_zero())
            return;
        if ( num.is_mips_hi_lo()) {
            write( Register::mips_hi, val >> 32);
            write( Register::mips_lo, val);
            return;
        }            

        auto& entry = get_entry(num);
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
        if ( !reg_num.is_zero() && writes_dst)
            write( reg_num, instr.get_v_dst());
        else
            write( reg_num, read(reg_num));
    }
};

#endif

