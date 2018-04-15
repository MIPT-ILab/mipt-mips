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
    using RegDstUInt = typename ISA::RegDstUInt;
    static const constexpr bool HAS_WIDE_DST = bitwidth<RegDstUInt> > 32;

    std::array<RegisterUInt, Register::MAX_REG> array = {};

    auto& get_value( Register num) { return array.at( num.to_size_t()); }
    const auto& get_value( Register num) const { return array.at( num.to_size_t()); }

    // We have to put a separate function with 'if constexpr' here as Visual Studio
    // produces a false positive warning in a case of RegDstUInt == uint32
    // (shifting uint32 right by 32 is an undefined behavior)
    // See: https://developercommunity.visualstudio.com/content/problem/225040/c4293-false-positive-on-unreacheable-code.html
    static RegDstUInt get_hi_part( RegDstUInt value)
    {
        // Clang-Tidy generates a false positive 'misc-suspicious-semicolon' warning
        // on `if constexpr ()` with template
        // LLVM bug 35824: https://bugs.llvm.org/show_bug.cgi?id=35824
        if constexpr( HAS_WIDE_DST)
            return value >> 32u; // NOLINT(misc-suspicious-semicolon)

        // GCC bug 81676 https://gcc.gnu.org/bugzilla/show_bug.cgi?id=81676
        // Wrong warning with unused-but-set-parameter within 'if constexpr'
        (void)(value);
        return 0;
    }

protected:
    auto read( Register num) const
    {
        assert( !num.is_mips_hi_lo());
        return get_value( num);
    }

    template <typename U = RegDstUInt>
    std::enable_if_t<HAS_WIDE_DST, U> read_hi_lo() const
    {
        const auto hi = static_cast<RegDstUInt>( read( Register::mips_hi));
        const auto lo = static_cast<RegDstUInt>( read( Register::mips_lo)) & ((RegDstUInt(1) << 32u) - 1);
        return (hi << 32u) | lo;
    }

    // Makes no sense if output is 32 bit
    template <typename U = RegDstUInt>
    std::enable_if_t<!HAS_WIDE_DST, U> read_hi_lo() const
    {
        assert( false);
        return 0u;
    }

    void write( Register num, RegDstUInt val, int8 is_accumulating_instr = 0, int8 loadlr = 0, RegDstUInt mask = 0x0)
    {
        if ( num.is_zero())
            return;

        // Hacks for MIPS madds/msubs
        if ( is_accumulating_instr == 1)
            val = read_hi_lo() + val;
        else if ( is_accumulating_instr == -1)
            val = read_hi_lo() - val;

        if ( loadlr == 1) // lwr
        {
            // Shift value to match it with beginning of mask
            val >>= count_leading_zeroes( static_cast<uint32>( mask));
            // Combine old and new values
            val = ( val & mask) | ( static_cast<RegDstUInt>(get_value( num)) & ~mask);
        }
        /*
            else
            {
                // lwl execute
            }*/

        // Hacks for MIPS multiplication register
        if ( num.is_mips_hi_lo()) {
            write( Register::mips_hi, get_hi_part( val));
            write( Register::mips_lo, val);
            return;
        }

        // No hacks
        get_value( num) = val;
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
        auto accumulating_instr = instr.is_accumulating_instr();
        auto loadlr = instr.is_loadlr();
        if ( !reg_num.is_zero() && writes_dst)
            write( reg_num, instr.get_v_dst(), accumulating_instr, loadlr, instr.get_lwrl_mask());
        else
            write( reg_num, read(reg_num));
    }
};

#endif
