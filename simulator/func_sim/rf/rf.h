/*
 * rf.h - register file
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2015-2018 MIPT-MIPS
 */

#ifndef RF_H
#define RF_H

#include <array>
#include <infra/types.h>
#include <infra/macro.h>

#include <cassert>

template<typename ISA>
class RF
{
    using FuncInstr = typename ISA::FuncInstr;
    using Register = typename ISA::Register;
    using RegisterUInt = typename ISA::RegisterUInt;

    std::array<RegisterUInt, Register::MAX_REG> array = {};

    auto& get_value( Register num) { return array.at( num.to_size_t()); }
    const auto& get_value( Register num) const { return array.at( num.to_size_t()); }

    static uint32 carry( uint32 x, uint32 y, int8 accumulation)
    {
        return (accumulation == +1 && MAX_VAL32 - x < y) || (accumulation == -1 && x < y) ? 1 : 0;
    }

protected:

    auto read( Register num) const
    {
        assert( !num.is_mips_hi_lo());
        return get_value( num);
    }

    void write( Register num, RegisterUInt val, RegisterUInt mask = all_ones<RegisterUInt>(), int8 accumulation = 0)
    {
        if ( num.is_zero())
            return;

        if ( accumulation != 0) {
            assert( num.is_mips_hi() || num.is_mips_lo());
            const auto old_val = get_value( num);
            // Handle carry to HI register
            if ( num.is_mips_lo())
                write ( Register::mips_hi, carry(old_val, val, accumulation), mask, accumulation);

            if (accumulation == 1)
                val = old_val + val;
            else
                val = old_val - val;
        }

        get_value( num) &= ~mask;      // Clear old bits
        get_value( num) |= val & mask; // Set new bits
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
        if (instr.get_dst_num().is_mips_hi_lo()) {
            write( Register::mips_lo, instr.get_v_dst(),  instr.get_mask(), instr.get_accumulation_type());
            write( Register::mips_hi, instr.get_v_dst2(), instr.get_mask(), instr.get_accumulation_type());
        }
        else {
            write( instr.get_dst_num(), instr.get_v_dst(), instr.get_mask());
        }
    }
};

#endif
