/*
 * alu.h - implementation of all execution units
 * Copyright 2014-2020 MIPT-MIPS
 */

#ifndef ALU_H
#define ALU_H

#include "alu_primitives.h"
#include "multiplication.h"
#include "traps/trap.h"

#include <infra/macro.h>
#include <infra/uint128.h>

#include <array>
#include <tuple>

template<size_t N>
auto align_up(Unsigned auto value) { return ((value + bitmask<decltype(value)>(N)) >> N) << N; }

static auto bit_shuffle( Unsigned auto value, size_t level)
{
    using T = decltype(value);
    const auto maskL = shuffle_mask<T, 2>( level);
    const auto maskR = shuffle_mask<T, 1>( level);
    const auto shamt = size_t{ 1} << ( level);
    T result = value & ~( maskL | maskR);
    result |= ( ( value << shamt) & maskL) | ( ( value >> shamt) & maskR);
    return result;
}

struct ALU
{
    // Generic
    static void move( Executable auto* instr)   { instr->v_dst[0] = instr->v_src[0]; }

    static size_t shamt_imm( const Executable auto* instr) { return narrow_cast<size_t>( instr->v_imm); }
    static size_t shamt_imm_32( const Executable auto* instr) { return narrow_cast<size_t>( instr->v_imm) + 32U; }

    template<Unsigned T> static
    size_t shamt_v_src2( const Executable auto* instr) {
        return narrow_cast<size_t>( instr->v_src[1] & bitmask<size_t>(log_bitwidth<T>));
    }

    template<Executable Instr, bool (Instr::* p)() const> static void set( Instr* instr)  { instr->v_dst[0] = (instr->*p)(); }

    static void check_halt_trap( Executable auto* instr) {
        // Handles 'goto nullptr;' and 'while (1);' cases
        if ( instr->new_PC == 0 || instr->new_PC == instr->PC)
            instr->trap = Trap::HALT;
    }

    static void load_addr( Executable auto* instr)  { addr( instr); }

    static void store_addr( Executable auto* instr) {
        addr( instr);
        instr->mask = bitmask<std::decay_t<decltype(instr->v_src[0])>>(instr->mem_size * 8);
    }

    static void load_addr_aligned( Executable auto* instr) {
        load_addr( instr);
        if ( instr->mem_addr % instr->mem_size != 0)
            instr->trap = Trap::UNALIGNED_LOAD;
    }

    static void load_addr_right32( Executable auto* instr) {
        // std::endian specific
        load_addr( instr);
        /* switch (instr->mem_addr % 4) {
           case 0: return 0xFFFF'FFFF;
           case 1: return 0x00FF'FFFF;
           case 2: return 0x0000'FFFF;
           case 3: return 0x0000'00FF;
           }
         */
        instr->mask = bitmask<uint32>( ( 4 - instr->mem_addr % 4) * 8);
    }

    static void load_addr_left32( Executable auto* instr) {
        // std::endian specific
        load_addr( instr);
        /* switch (instr->mem_addr % 4) {
           case 0: return 0xFF00'0000;
           case 1: return 0xFFFF'0000;
           case 2: return 0xFFFF'FF00;
           case 3: return 0xFFFF'FFFF;
           }
         */
        instr->mask = bitmask<std::decay_t<decltype(instr->v_src[0])>>( ( 1 + instr->mem_addr % 4) * 8) << ( ( 3 - instr->mem_addr % 4) * 8);
        // Actually we read a word LEFT to effective address
        instr->mem_addr -= 3;
    }

    // store functions done by analogy with loads
    static void store_addr_aligned( Executable auto* instr) {
        store_addr( instr);
        if ( instr->mem_addr % instr->mem_size != 0)
            instr->trap = Trap::UNALIGNED_STORE;
    }

    static void store_addr_right32( Executable auto* instr) {
        store_addr( instr);
        instr->mask = bitmask<uint32>( ( 4 - instr->mem_addr % 4) * 8);
    }

    static void store_addr_left32( Executable auto* instr) {
        store_addr( instr);
        instr->mask = bitmask<std::decay_t<decltype(instr->v_src[0])>>( ( 1 + instr->mem_addr % 4) * 8) << ( ( 3 - instr->mem_addr % 4) * 8);
        instr->mem_addr -= 3;
    }

    static void addr( Executable auto* instr) { instr->mem_addr = narrow_cast<Addr>( instr->v_src[0] + instr->v_imm); }

    // General addition
    template<Executable I, Unsigned T> static void addition( I* instr) { instr->v_dst[0] = narrow_cast<T>( instr->v_src[0]) + narrow_cast<T>( instr->v_src[1]); }
    template<Executable I, Unsigned T> static void subtraction( I* instr) { instr->v_dst[0] = narrow_cast<T>( instr->v_src[0]) - narrow_cast<T>( instr->v_src[1]); }
    template<Executable I, Unsigned T> static void riscv_addition( I* instr) {
        using UInt = std::decay_t<decltype(instr->v_src[0])>;
        instr->v_dst[0] = sign_extension<bitwidth<T>, UInt>(narrow_cast<T>( instr->v_src[0]) + narrow_cast<T>( instr->v_src[1]));
    }

    template<Executable I, Unsigned T> static void riscv_subtraction( I* instr) {
        using UInt = std::decay_t<decltype(instr->v_src[0])>;
        instr->v_dst[0] = sign_extension<bitwidth<T>, UInt>(narrow_cast<T>( instr->v_src[0]) - narrow_cast<T>( instr->v_src[1]));
    }

    template<Executable I, Unsigned T> static void addition_imm( I* instr) {
        instr->v_dst[0] = narrow_cast<T>( instr->v_src[0]) + narrow_cast<T>( instr->v_imm);
    }

    template<Executable I, Unsigned T> static
    void addition_overflow( I* instr)
    {
        const auto [result, overflow] = test_addition_overflow<T>( instr->v_src[0], instr->v_src[1]);
        if ( overflow)
            instr->trap = Trap::INTEGER_OVERFLOW;
        else
            instr->v_dst[0] = result;
    }

    template<Executable I, Unsigned T> static
    void addition_overflow_imm( I* instr)
    {
        const auto [result, overflow] = test_addition_overflow<T>( instr->v_src[0], instr->v_imm);
        if ( overflow)
            instr->trap = Trap::INTEGER_OVERFLOW;
        else
            instr->v_dst[0] = result;
    }


    template<Executable I, Unsigned T> static
    void subtraction_overflow( I* instr)
    {
        const auto [result, overflow] = test_subtraction_overflow<T>( instr->v_src[0], instr->v_src[1]);
        if ( overflow)
            instr->trap = Trap::INTEGER_OVERFLOW;
        else
            instr->v_dst[0] = result;
    }

    // Shifts
    template<Executable I, Unsigned T> static void sll( I* instr)  { instr->v_dst[0] = sign_extension<bitwidth<T>>( ( instr->v_src[0] & all_ones<T>()) << shamt_imm( instr)); }
    template<Executable I, Unsigned T> static void srl( I* instr)  { instr->v_dst[0] = sign_extension<bitwidth<T>>( ( instr->v_src[0] & all_ones<T>()) >> shamt_imm( instr)); }
    template<Executable I, Unsigned T> static void sra( I* instr)  { instr->v_dst[0] = arithmetic_rs( sign_extension<bitwidth<T>>( instr->v_src[0]), shamt_imm( instr)); }
    template<Executable I, Unsigned T> static void sllv( I* instr) { instr->v_dst[0] = sign_extension<bitwidth<T>>( ( instr->v_src[0] & all_ones<T>()) << shamt_v_src2<T>( instr)); }
    template<Executable I, Unsigned T> static void srlv( I* instr) { instr->v_dst[0] = sign_extension<bitwidth<T>>( ( instr->v_src[0] & all_ones<T>()) >> shamt_v_src2<T>( instr)); }
    template<Executable I, Unsigned T> static void srav( I* instr) { instr->v_dst[0] = arithmetic_rs( sign_extension<bitwidth<T>>( instr->v_src[0]), shamt_v_src2<T>( instr)); }
    template<Executable I, Unsigned T> static void slo( I* instr)  { instr->v_dst[0] = ones_ls( sign_extension<bitwidth<T>>( instr->v_src[0]), shamt_v_src2<T>( instr)); }
    template<Executable I, Unsigned T> static void sloi( I* instr) { instr->v_dst[0] = ones_ls( sign_extension<bitwidth<T>>( instr->v_src[0]), shamt_imm( instr)); }
    template<Executable I, Unsigned T> static void sro( I* instr)  { instr->v_dst[0] = ones_rs( instr->v_src[0], shamt_v_src2<T>( instr)); }
    static void sroi( Executable auto* instr) { instr->v_dst[0] = ones_rs( instr->v_src[0], shamt_imm( instr)); }

    // Circular shifts
    static void rol( Executable auto* instr) {
        using UInt = std::decay_t<decltype(instr->v_src[0])>;
        instr->v_dst[0] = circ_ls( instr->v_src[0], shamt_v_src2<UInt>( instr));
    }
    static void ror( Executable auto* instr) {
        using UInt = std::decay_t<decltype(instr->v_src[0])>;
        instr->v_dst[0] = circ_rs( instr->v_src[0], shamt_v_src2<UInt>( instr));
    }
    static void rori( Executable auto* instr) {
        instr->v_dst[0] = circ_rs( instr->v_src[0], shamt_imm( instr));
    }

    // MIPS extra shifts
    static void dsll32( Executable auto* instr) { instr->v_dst[0] = instr->v_src[0] << shamt_imm_32( instr); }
    static void dsrl32( Executable auto* instr) { instr->v_dst[0] = instr->v_src[0] >> shamt_imm_32( instr); }
    static void dsra32( Executable auto* instr) { instr->v_dst[0] = arithmetic_rs( instr->v_src[0], shamt_imm_32( instr)); }
    template<size_t N> static void upper_immediate( Executable auto* instr)  { instr->v_dst[0] = instr->v_imm << N; }
    static void auipc( Executable auto* instr) { upper_immediate<12>( instr); instr->v_dst[0] += instr->PC; }

    // Leading zero/ones
    template<Executable I, Unsigned T> static void clo( I* instr)  { instr->v_dst[0] = count_leading_ones<T>( instr->v_src[0]); }
    template<Executable I, Unsigned T> static void clz( I* instr)  { instr->v_dst[0] = count_leading_zeroes<T>( instr->v_src[0]); }
    template<Executable I, Unsigned T> static void ctz( I* instr)  { instr->v_dst[0] = count_trailing_zeroes<T>( instr->v_src[0]); }
    template<Executable I, Unsigned T> static void pcnt( I* instr) { instr->v_dst[0] = narrow_cast<T>( popcount( instr->v_src[0])); }

    // Logic
    static void andv( Executable auto* instr)  { instr->v_dst[0] = instr->v_src[0] & instr->v_src[1]; }
    static void orv( Executable auto* instr)   { instr->v_dst[0] = instr->v_src[0] | instr->v_src[1]; }
    static void xorv( Executable auto* instr)  { instr->v_dst[0] = instr->v_src[0] ^ instr->v_src[1]; }
    static void nor( Executable auto* instr)   { instr->v_dst[0] = ~(instr->v_src[0] | instr->v_src[1]); }
    static void andi( Executable auto* instr)  { instr->v_dst[0] = instr->v_src[0] & instr->v_imm; }
    static void ori( Executable auto* instr)   { instr->v_dst[0] = instr->v_src[0] | instr->v_imm; }
    static void xori( Executable auto* instr)  { instr->v_dst[0] = instr->v_src[0] ^ instr->v_imm; }
    static void orn( Executable auto* instr)   { instr->v_dst[0] = instr->v_src[0] | ~instr->v_src[1]; }
    static void xnor( Executable auto* instr)  { instr->v_dst[0] = instr->v_src[0] ^ ~instr->v_src[1]; }

    // Bit permutation
    static void riscv_unshfl( Executable auto* instr)
    {
        using UInt = std::decay_t<decltype(instr->v_src[0])>;
        auto dst_value = instr->v_src[0];
        constexpr size_t limit = log_bitwidth<UInt> - 1;
        for ( size_t i = 0; i < limit; ++i)
            if ( ( instr->v_src[1] >> i) & 1U)
                dst_value = bit_shuffle( dst_value, i);
        instr->v_dst[0] = dst_value;
    }

    static void riscv_shfl( Executable auto* instr)
    {
        using UInt = std::decay_t<decltype(instr->v_src[0])>;
        auto dst_value = instr->v_src[0];
        constexpr size_t limit = log_bitwidth<UInt> - 1;
        for ( size_t i = limit ; i > 0; --i)
            if( ( instr->v_src[1] >> (i - 1)) & 1U)
                dst_value = bit_shuffle( dst_value, i - 1);
        instr->v_dst[0] = dst_value;
    }

    // Generalized OR-Combine and reverse
    static void gorc( Executable auto* instr)
    {
        using UInt = std::decay_t<decltype(instr->v_src[0])>;
        instr->v_dst[0] = gen_or_combine( instr->v_src[0], shamt_v_src2<UInt>( instr));
    }

    static void grev( Executable auto* instr)
    {
        using UInt = std::decay_t<decltype(instr->v_src[0])>;
        instr->v_dst[0] = gen_reverse( instr->v_src[0], shamt_v_src2<UInt>( instr));
    }

    // OR Combine
    static constexpr size_t gorci_orc_b_shamt = 4 | 2 | 1;
    static void orc_b( Executable auto* instr ) { instr->v_dst[0] = gen_or_combine( instr->v_src[0], gorci_orc_b_shamt); }

    // Conditional moves
    static void movn( Executable auto* instr)  { move( instr); if (instr->v_src[1] == 0) instr->mask = 0; }
    static void movz( Executable auto* instr)  { move( instr); if (instr->v_src[1] != 0) instr->mask = 0; }

    // Bit manipulations
    template<Executable I, Unsigned T> static void sbinv( I* instr) { instr->v_dst[0] = instr->v_src[0] ^ ( lsb_set<T>() << shamt_v_src2<T>( instr)); }
    template<Executable I, Unsigned T> static void sbext( I* instr) { instr->v_dst[0] = 1U & ( instr->v_src[0] >> shamt_v_src2<T>( instr)); }

    static void max( Executable auto* instr)  { instr->v_dst[0] = instr->v_src[ instr->ge()  ? 0 : 1]; }
    static void maxu( Executable auto* instr) { instr->v_dst[0] = instr->v_src[ instr->geu() ? 0 : 1]; }
    static void min( Executable auto* instr)  { instr->v_dst[0] = instr->v_src[ instr->lt()  ? 0 : 1]; }
    static void minu( Executable auto* instr) { instr->v_dst[0] = instr->v_src[ instr->ltu() ? 0 : 1]; }

    template<Executable I, Unsigned T> static
    void clmul( I* instr)
    {
        instr->v_dst[0] = 0;
        for ( std::size_t index = 0; index < bitwidth<T>; index++)
            if ( ( instr->v_src[1] >> index) & 1U)
                instr->v_dst[0] ^= instr->v_src[0] << index;
    }

    template<Executable I, Unsigned T> static void add_uw( I* instr) { instr->v_dst[0] = instr->v_src[1] + ( bitmask<T>(32) & instr->v_src[0]); }
  
    template<Executable I, Unsigned T> static void bclr( I* instr) { instr->v_dst[0] = instr->v_src[0] & ~( lsb_set<T>() << shamt_v_src2<T>( instr)); }
    template<Executable I, Unsigned T> static void bseti( I* instr)
    {
        using UInt = std::decay_t<decltype(instr->v_src[0])>;
        static constexpr auto XLEN = bitwidth<UInt>;
        instr->v_dst[0] = instr->v_src[0] | (lsb_set<T>() << (shamt_imm (instr) & (XLEN - 1)));
    }
    template<Executable I, Unsigned T> static void sext_b( I* instr) { instr->v_dst[0] = sign_extension<T>(instr->v_src[0], bitwidth<char>); }

    // Bit manipulations
    template<Executable I, Unsigned T> static
    void pack( I* instr)
    {
        auto pack_width = half_bitwidth<T>;
        instr->v_dst[0] = ( instr->v_src[0] & bitmask<T>( pack_width)) | ( instr->v_src[1] << pack_width);
    }

    template<Executable I, Unsigned T> static
    void packu( I* instr)
    {
        auto pack_width = half_bitwidth<T>;
        instr->v_dst[0] = ( (instr->v_src[0] >> pack_width) | (instr->v_src[1] & (bitmask<T>(pack_width) << pack_width)));
    }

    static void bclri( Executable auto* instr )
    {
        using UInt = std::decay_t<decltype(instr->v_src[0])>;
        static constexpr auto XLEN = bitwidth<UInt>;
        auto index = shamt_imm( instr) & (XLEN - 1);
        auto mask = ~(lsb_set<UInt>() << index);
        instr->v_dst[0] = instr->v_src[0] & mask;
    }

    static void bset( Executable auto* instr)
    {
        using UInt = std::decay_t<decltype(instr->v_src[0])>;
        instr->v_dst[0] = instr->v_src[0] | (lsb_set<UInt>() << shamt_v_src2<UInt>( instr));
    }

    // Branches
    template<Executable Instr, bool (Instr::* p)() const> static
    void branch( Instr* instr)
    {
        instr->is_taken_branch = (instr->*p)();
        if ( instr->is_taken_branch) {
            instr->new_PC = instr->get_decoded_target();
            check_halt_trap( instr);
        }
    }

    static void jump( Executable auto* instr, Addr target)
    {
        instr->is_taken_branch = true;
        instr->new_PC = target;
        check_halt_trap( instr);
    }

    static void j( Executable auto* instr) { jump(instr, instr->get_decoded_target()); }
    static void riscv_jr( Executable auto* instr) { jump( instr, narrow_cast<Addr>( instr->v_src[0] + instr->v_imm));; }

    static void jr( Executable auto* instr) {
        if (instr->v_src[0] % 4 != 0)
            instr->trap = Trap::UNALIGNED_FETCH;
        jump( instr, align_up<2>(instr->v_src[0]));
    }

    template<Executable I, void (*j)(I* instr)> static
    void jump_and_link( I* instr)
    {
        instr->v_dst[0] = narrow_cast<typename I::RegisterUInt>( instr->new_PC); // link
        (*j)( instr);   // jump
    }

    template<Executable I, bool (I::*p)() const> static
    void branch_and_link( I* instr)
    {
        instr->is_taken_branch = (instr->*p)();
        instr->v_dst[0] = narrow_cast<typename I::RegisterUInt>( instr->new_PC);
        if ( instr->is_taken_branch)
        {
            instr->new_PC = instr->get_decoded_target();
            check_halt_trap( instr);
        }
    }

    static void eret( Executable auto* instr)
    {
        // FIXME(pikryukov): That should behave differently for ErrorEPC
        jump( instr, instr->v_src[0]);
        instr->v_dst[0] &= instr->v_src[1] & ~(1U << 2);
    }

    // Traps
    static void breakpoint( Executable auto* instr)   { instr->trap = Trap::BREAKPOINT; }
    static void syscall   ( Executable auto* instr)   { instr->trap = Trap::SYSCALL;    }
    static void halt( Executable auto* instr)   { instr->trap = Trap::HALT; }

    template<Executable I, bool (I::* p)() const> static
    void trap( I* instr)
    {
        if ((instr->*p)())
            instr->trap = Trap::EXPLICIT_TRAP;
    }

    static void unknown_instruction( Executable auto* instr) { instr->trap = Trap::UNKNOWN_INSTRUCTION; }

    static void csrrw( Executable auto* instr)
    {
        instr->v_dst[0] = instr->v_src[0]; // CSR <- RS1
        instr->v_dst[1] = instr->v_src[1]; // RD  <- CSR
    }

    static void csrrs( Executable auto* instr)
    {
        instr->mask   = instr->v_src[0];
        instr->v_dst[0] = all_ones<std::decay_t<decltype(instr->v_src[0])>>(); // CSR <- 0xffff & RS1
        instr->v_dst[1] = instr->v_src[1]; // RD <- CSR
    }

    static void csrrwi( Executable auto* instr)
    {
        instr->v_dst[0] = instr->v_imm;  // CSR <- RS1
        instr->v_dst[1] = instr->v_src[1]; // RD  <- CSR
    }

    template<Executable I, Unsigned T> static
    void riscv_addition_imm( I* instr)
    {
        instr->v_dst[0] = sign_extension<bitwidth<T>>( instr->v_src[0] + instr->v_imm);
    }

    static void bit_field_place( Executable auto* instr)
    {
        using UInt = std::decay_t<decltype(instr->v_src[0])>;
        static constexpr auto XLEN = bitwidth<UInt>;
        size_t len = ( narrow_cast<size_t>( instr->v_src[1]) >> 24) & 15U;
        len = len > 0 ? len : 16;
        size_t off = ( narrow_cast<size_t>( instr->v_src[1]) >> 16) & ( XLEN-1);
        auto mask = bitmask<UInt>( len) << off;
        auto data = instr->v_src[1] << off;
        instr->v_dst[0] = ( data & mask) | ( instr->v_src[0] & ~mask);
    }
};

#endif
