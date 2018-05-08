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

    static const constexpr bool HAS_WIDE_DST = bitwidth<RegDstUInt> > 64;
    static const constexpr bool HAS_32_BIT_DST = bitwidth<RegDstUInt> > 32;
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
            return value >> 64u; // NOLINT(misc-suspicious-semicolon)

        // GCC bug 81676 https://gcc.gnu.org/bugzilla/show_bug.cgi?id=81676
        // Wrong warning with unused-but-set-parameter within 'if constexpr'
        (void)(value);
        return 0;
    }

    static RegDstUInt get_hi_part_accum( RegDstUInt value)
    {
        // Clang-Tidy generates a false positive 'misc-suspicious-semicolon' warning
        // on `if constexpr ()` with template
        // LLVM bug 35824: https://bugs.llvm.org/show_bug.cgi?id=35824
        if constexpr( HAS_32_BIT_DST)
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

    // Used only by accumulating instructions
    // According to MIPS64 ISA, result is unpredictable
    // if upper 32 bits of HI or LO are not trivial
    template <typename U = RegDstUInt>
    std::enable_if_t<HAS_WIDE_DST, U> read_hi_lo() const
    {
        const auto hi = static_cast<RegDstUInt>( read( Register::mips_hi)) & bitmask<RegDstUInt>(32);
        const auto lo = static_cast<RegDstUInt>( read( Register::mips_lo)) & bitmask<RegDstUInt>(32);
        return (hi << 32u) | lo;
    }

    // Makes no sense if output is 32 bit
    template <typename U = RegDstUInt>
    std::enable_if_t<!HAS_WIDE_DST, U> read_hi_lo() const
    {
        assert( false);
        return 0u;
    }

    static uint64 get_hi_lo_64(const uint128& val) 
    {
        const auto lo = static_cast<uint64>(val);
        const auto hi = static_cast<uint64>(val >> 64u);
        return (hi << 32u) | lo;
    }

    void write( Register num, RegDstUInt val, RegisterUInt mask = all_ones<RegisterUInt>(), int8 accumulation = 0)
    {
        if ( num.is_zero())
            return;

        // Hacks for MIPS madds/msubs
        if ( accumulation == 1)
            val = read_hi_lo() + get_hi_lo_64(val);
        else if ( accumulation == -1)
            val = read_hi_lo() - get_hi_lo_64(val);

        // Hacks for MIPS multiplication register
        if ( num.is_mips_hi_lo()) {
            if (accumulation != 0) {
                write( Register::mips_hi, get_hi_part_accum( val), bitmask<RegisterUInt>(32));
                write( Register::mips_lo, val,                     bitmask<RegisterUInt>(32));
                return;
            }
            write( Register::mips_hi, get_hi_part( val), bitmask<RegisterUInt>(64));
            write( Register::mips_lo, val,               bitmask<RegisterUInt>(64));
            return;
        }

        // No hacks
        get_value( num) &= ~mask;         // Clear old bits
        get_value( num) |= static_cast<RegisterUInt>( val) & mask; // Set new bits
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
        write( instr.get_dst_num(),
               instr.get_v_dst(),
               instr.get_mask(),
               instr.get_accumulation_type());
    }
};

#endif
