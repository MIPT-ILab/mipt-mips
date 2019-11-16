/*
 * rf.h - register file
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2015-2018 MIPT-MIPS
 */

#ifndef RF_H
#define RF_H

#include <infra/macro.h>
#include <infra/types.h>

#include <array>
#include <cassert>

template<typename FuncInstr>
class RF
{
    using Register = typename FuncInstr::Register;
    using RegisterUInt = typename FuncInstr::RegisterUInt;

    std::array<RegisterUInt, Register::MAX_REG> array = {};

    auto& get_value( Register num) { return array.at( num.to_rf_index()); }
    const auto& get_value( Register num) const { return array.at( num.to_rf_index()); }

    static uint32 get_carry( uint32 x, uint32 y, int8 accumulation)
    {
        return (accumulation == +1 && MAX_VAL32 - x < y) || (accumulation == -1 && x < y) ? 1 : 0;
    }

public:
    RF() = default;

    const auto& read( Register num) const
    {
        return get_value( num);
    }

    void write( Register num, RegisterUInt val)
    {
        if ( !num.is_zero())
            get_value( num) = val;
    }

    void write( Register num, RegisterUInt val, RegisterUInt mask)
    {
        const auto new_val = ( get_value( num) & ~mask) | ( val & mask);
        write( num, new_val);
    }

    void write( Register num, RegisterUInt val, RegisterUInt mask, int8 accumulation)
    {
        if ( accumulation != 0) {
            assert( num.is_mips_hi() || num.is_mips_lo());
            const auto old_val = get_value( num);
            // Handle carry to HI register
            if ( num.is_mips_lo()) {
                auto carry = get_carry( narrow_cast<uint32>( old_val), narrow_cast<uint32>( val), accumulation);
                write( Register::mips_hi(), carry, mask, accumulation);
            }

            if (accumulation == 1)
                val = narrow_cast<uint32>(old_val) + narrow_cast<uint32>(val);
            else
                val = narrow_cast<uint32>(old_val) - narrow_cast<uint32>(val);
        }

        write( num, val, mask);
    }

    inline void read_source( FuncInstr* instr, size_t index) const
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
        write( instr.get_dst_num(),  instr.get_v_dst(),  instr.get_mask(), instr.get_accumulation_type());
        write( instr.get_dst2_num(), instr.get_v_dst2(), all_ones<RegisterUInt>(), instr.get_accumulation_type());
    }
};

#endif
