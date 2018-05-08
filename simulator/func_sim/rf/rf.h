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

    // Used only by accumulation instructions which work only in 32-bit mode
    static uint64 merge_hi_lo(const RegisterUInt& lo, const RegisterUInt& hi) 
    {
        return (uint64(hi) << 32u) | uint64(lo);
    }

protected:

    auto read( Register num) const
    {
        assert( !num.is_mips_hi_lo());
        return get_value( num);
    }

    // Used only by accumulation instructions which work only in 32-bit mode
    auto read_hi_lo() const
    {
        return merge_hi_lo( read( Register::mips_lo), read( Register::mips_hi));
    }
    
    void write( Register num, RegisterUInt val, RegisterUInt mask = all_ones<RegisterUInt>())
    {
        if ( num.is_zero())
            return;

        assert( !num.is_mips_hi_lo());

        // No hacks
        get_value( num) &= ~mask;         // Clear old bits
        get_value( num) |= val & mask; // Set new bits
    }

    void write_hi_lo( RegisterUInt lo, RegisterUInt hi, int8 accumulation = 0)
    {
        // Hacks for MIPS madds/msubs
        if ( accumulation == 1) {
            auto val = read_hi_lo() + merge_hi_lo(lo, hi);
            hi = val >> 32;
            lo = val & bitmask<RegisterUInt>(32);
        }
        else if ( accumulation == -1) {
            auto val = read_hi_lo() - merge_hi_lo(lo, hi);
            hi = val >> 32;
            lo = val & bitmask<RegisterUInt>(32);
        }

        write( Register::mips_hi, hi);
        write( Register::mips_lo, lo);
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
        if (instr.get_dst_num().is_mips_hi_lo())
            write_hi_lo( instr.get_v_dst(), instr.get_v_dst2(), instr.get_accumulation_type());
        else
            write( instr.get_dst_num(), instr.get_v_dst(), instr.get_mask());
    }
};

#endif
