/*
 * rf.h - register file
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2015-2020 MIPT-MIPS
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
        write( num, apply_mask( num, val, mask));
    }

    void write( Register num, RegisterUInt val, RegisterUInt mask, int8 accumulation)
    {
        const auto [new_val, do_carry] = accumulate( num, val, accumulation);
        if ( num.is_mips_lo() && do_carry)
            carry( mask, accumulation);

        write( num, new_val, mask);
    }

    inline void read_source( FuncInstr* instr, size_t index) const
    {
        instr->set_v_src(  read( instr->get_src( index)), index);
    }

    inline void read_sources( FuncInstr* instr) const
    {
        read_source( instr, 0);
        read_source( instr, 1);
    }

    inline void write_dst( const FuncInstr& instr)
    {
        write( instr.get_dst( 0), instr.get_v_dst( 0), instr.get_mask(), instr.get_accumulation_type());
        write( instr.get_dst( 1), instr.get_v_dst( 1), all_ones<RegisterUInt>(), instr.get_accumulation_type());
    }

private:
    std::array<RegisterUInt, Register::MAX_REG> array = {};

    auto& get_value( Register num) { return array.at( num.to_rf_index()); }
    const auto& get_value( Register num) const { return array.at( num.to_rf_index()); }

    RegisterUInt apply_mask( Register num, RegisterUInt val, RegisterUInt mask) const
    {
        return (get_value( num) & ~mask) | ( val & mask);
    }

    static std::pair<RegisterUInt, bool> addition( uint32 x, uint32 y)
    {
        return std::pair{ x + y, MAX_VAL32 - x < y};
    }

    static std::pair<RegisterUInt, bool> subtraction( uint32 x, uint32 y)
    {
        return std::pair{ x - y, x < y};
    }

    std::pair<RegisterUInt, bool> accumulate( Register num, RegisterUInt val, int8 accumulation) const
    {
        switch (accumulation) {
        case +1: return addition( narrow_cast<uint32>( get_value( num)), narrow_cast<uint32>( val));
        case -1: return subtraction( narrow_cast<uint32>( get_value( num)), narrow_cast<uint32>( val));
        default: return std::pair{val, false};
        }
    }

    void carry( RegisterUInt mask, int8 accumulation)
    {
        auto num = Register::mips_hi();
        write( num, accumulate( num, 1, accumulation).first, mask);
    }
};

#endif
