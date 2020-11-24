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

template<size_t N, typename T>
T align_up(T value) { return ((value + bitmask<T>(N)) >> N) << N; }

template<typename T> static T bit_shuffle( T value, size_t level)
{
    const auto maskL = shuffle_mask<T, 2>( level);
    const auto maskR = shuffle_mask<T, 1>( level);
    const auto shamt = size_t{ 1} << ( level);
    T result = value & ~( maskL | maskR);
    result |= ( ( value << shamt) & maskL) | ( ( value >> shamt) & maskR);
    return result;
}

template <typename Instr>
struct ALU
{
    // Generic
    using Predicate = bool (*)( const Instr*);
    using Execute = void (*)( Instr*);
    using RegisterUInt = typename Instr::RegisterUInt;
    using RegisterSInt = typename Instr::RegisterSInt;
    
    static constexpr size_t XLEN = bitwidth<RegisterUInt>;

    static size_t shamt_imm( const Instr* instr) { return narrow_cast<size_t>( instr->v_imm); }
    static size_t shamt_imm_32( const Instr* instr) { return narrow_cast<size_t>( instr->v_imm) + 32U; }
    template<typename T> static size_t shamt_v_src2( const Instr* instr) { return narrow_cast<size_t>( instr->v_src[1] & bitmask<size_t>(log_bitwidth<T>)); }
    static void move( Instr* instr)   { instr->v_dst[0] = instr->v_src[0]; }
    template<Predicate p> static void set( Instr* instr)  { instr->v_dst[0] = p( instr); }

    static void check_halt_trap( Instr* instr) {
        // Handles 'goto nullptr;' and 'while (1);' cases
        if ( instr->new_PC == 0 || instr->new_PC == instr->PC)
            instr->trap = Trap::HALT;
    }

    static void load_addr( Instr* instr)  { addr( instr); }

    static void store_addr( Instr* instr) {
        addr( instr);
        instr->mask = bitmask<RegisterUInt>(instr->mem_size * 8);
    }

    static void load_addr_aligned( Instr* instr) {
        load_addr( instr);
        if ( instr->mem_addr % instr->mem_size != 0)
            instr->trap = Trap::UNALIGNED_LOAD;
    }

    static void load_addr_right32( Instr* instr) {
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

    static void load_addr_left32( Instr* instr) {
        // std::endian specific
        load_addr( instr);
        /* switch (instr->mem_addr % 4) {
           case 0: return 0xFF00'0000;
           case 1: return 0xFFFF'0000;
           case 2: return 0xFFFF'FF00;
           case 3: return 0xFFFF'FFFF;
           }
         */
        instr->mask = bitmask<RegisterUInt>( ( 1 + instr->mem_addr % 4) * 8) << ( ( 3 - instr->mem_addr % 4) * 8);
        // Actually we read a word LEFT to effective address
        instr->mem_addr -= 3;
    }

    // store functions done by analogy with loads
    static void store_addr_aligned( Instr* instr) {
        store_addr( instr);
        if ( instr->mem_addr % instr->mem_size != 0)
            instr->trap = Trap::UNALIGNED_STORE;
    }

    static void store_addr_right32( Instr* instr) {
        store_addr( instr);
        instr->mask = bitmask<uint32>( ( 4 - instr->mem_addr % 4) * 8);
    }

    static void store_addr_left32( Instr* instr) {
        store_addr( instr);
        instr->mask = bitmask<RegisterUInt>( ( 1 + instr->mem_addr % 4) * 8) << ( ( 3 - instr->mem_addr % 4) * 8);
        instr->mem_addr -= 3;
    }

    static void addr( Instr* instr) { instr->mem_addr = narrow_cast<Addr>( instr->v_src[0] + instr->v_imm); }

    // Predicate helpers - unary
    static bool lez( const Instr* instr) { return narrow_cast<RegisterSInt>( instr->v_src[0]) <= 0; }
    static bool gez( const Instr* instr) { return narrow_cast<RegisterSInt>( instr->v_src[0]) >= 0; }
    static bool ltz( const Instr* instr) { return narrow_cast<RegisterSInt>( instr->v_src[0]) < 0; }
    static bool gtz( const Instr* instr) { return narrow_cast<RegisterSInt>( instr->v_src[0]) > 0; }

    // Predicate helpers - binary
    static bool eq( const Instr* instr)  { return instr->v_src[0] == instr->v_src[1]; }
    static bool ne( const Instr* instr)  { return instr->v_src[0] != instr->v_src[1]; }
    static bool geu( const Instr* instr) { return instr->v_src[0] >= instr->v_src[1]; }
    static bool ltu( const Instr* instr) { return instr->v_src[0] <  instr->v_src[1]; }
    static bool ge( const Instr* instr)  { return narrow_cast<RegisterSInt>( instr->v_src[0]) >= narrow_cast<RegisterSInt>( instr->v_src[1]); }
    static bool lt( const Instr* instr)  { return narrow_cast<RegisterSInt>( instr->v_src[0]) <  narrow_cast<RegisterSInt>( instr->v_src[1]); }

    // Predicate helpers - immediate
    static bool eqi( const Instr* instr) { return instr->v_src[0] == instr->v_imm; }
    static bool nei( const Instr* instr) { return instr->v_src[0] != instr->v_imm; }
    static bool lti( const Instr* instr) { return narrow_cast<RegisterSInt>( instr->v_src[0]) < narrow_cast<RegisterSInt>( instr->v_imm); }
    static bool gei( const Instr* instr) { return narrow_cast<RegisterSInt>( instr->v_src[0]) >= narrow_cast<RegisterSInt>( instr->v_imm); }
    static bool ltiu( const Instr* instr) { return instr->v_src[0] < instr->v_imm; }
    static bool geiu( const Instr* instr) { return instr->v_src[0] >= instr->v_imm; }

    // General addition
    template<typename T> static void addition( Instr* instr)     { instr->v_dst[0] = narrow_cast<T>( instr->v_src[0]) + narrow_cast<T>( instr->v_src[1]); }
    template<typename T> static void subtraction( Instr* instr)  { instr->v_dst[0] = narrow_cast<T>( instr->v_src[0]) - narrow_cast<T>( instr->v_src[1]); }
    template<typename T> static void riscv_addition( Instr* instr)     { instr->v_dst[0] = sign_extension<bitwidth<T>, RegisterUInt>(narrow_cast<T>( instr->v_src[0]) + narrow_cast<T>( instr->v_src[1])); }
    template<typename T> static void riscv_subtraction( Instr* instr)  { instr->v_dst[0] = sign_extension<bitwidth<T>, RegisterUInt>(narrow_cast<T>( instr->v_src[0]) - narrow_cast<T>( instr->v_src[1])); }
    template<typename T> static void addition_imm( Instr* instr) { instr->v_dst[0] = narrow_cast<T>( instr->v_src[0]) + narrow_cast<T>( instr->v_imm); }

    template<typename T> static
    void addition_overflow( Instr* instr)
    {
        const auto [result, overflow] = test_addition_overflow<T>( instr->v_src[0], instr->v_src[1]);
        if ( overflow)
            instr->trap = Trap::INTEGER_OVERFLOW;
        else
            instr->v_dst[0] = result;
    }

    template<typename T> static
    void addition_overflow_imm( Instr* instr)
    {
        const auto [result, overflow] = test_addition_overflow<T>( instr->v_src[0], instr->v_imm);
        if ( overflow)
            instr->trap = Trap::INTEGER_OVERFLOW;
        else
            instr->v_dst[0] = result;
    }


    template<typename T> static
    void subtraction_overflow( Instr* instr)
    {
        const auto [result, overflow] = test_subtraction_overflow<T>( instr->v_src[0], instr->v_src[1]);
        if ( overflow)
            instr->trap = Trap::INTEGER_OVERFLOW;
        else
            instr->v_dst[0] = result;
    }

    // Shifts
    template<typename T> static void sll( Instr* instr)  { instr->v_dst[0] = sign_extension<bitwidth<T>>( ( instr->v_src[0] & all_ones<T>()) << shamt_imm( instr)); }
    template<typename T> static void srl( Instr* instr)  { instr->v_dst[0] = sign_extension<bitwidth<T>>( ( instr->v_src[0] & all_ones<T>()) >> shamt_imm( instr)); }
    template<typename T> static void sra( Instr* instr)  { instr->v_dst[0] = arithmetic_rs( sign_extension<bitwidth<T>>( instr->v_src[0]), shamt_imm( instr)); }
    template<typename T> static void sllv( Instr* instr) { instr->v_dst[0] = sign_extension<bitwidth<T>>( ( instr->v_src[0] & all_ones<T>()) << shamt_v_src2<T>( instr)); }
    template<typename T> static void srlv( Instr* instr) { instr->v_dst[0] = sign_extension<bitwidth<T>>( ( instr->v_src[0] & all_ones<T>()) >> shamt_v_src2<T>( instr)); }
    template<typename T> static void srav( Instr* instr) { instr->v_dst[0] = arithmetic_rs( sign_extension<bitwidth<T>>( instr->v_src[0]), shamt_v_src2<T>( instr)); }
    template<typename T> static void slo( Instr* instr)  { instr->v_dst[0] = ones_ls( sign_extension<bitwidth<T>>( instr->v_src[0]), shamt_v_src2<T>( instr)); }
    template<typename T> static void sloi( Instr* instr) { instr->v_dst[0] = ones_ls( sign_extension<bitwidth<T>>( instr->v_src[0]), shamt_imm( instr)); }
    template<typename T> static void sro( Instr* instr)  { instr->v_dst[0] = ones_rs( instr->v_src[0], shamt_v_src2<T>( instr)); }
    static void sroi( Instr* instr) { instr->v_dst[0] = ones_rs( instr->v_src[0], shamt_imm( instr)); }

    // Circular shifts
    static void rol( Instr* instr) { instr->v_dst[0] = circ_ls( sign_extension<XLEN>( instr->v_src[0]), shamt_v_src2<RegisterUInt>( instr)); }
    static void ror( Instr* instr) { instr->v_dst[0] = circ_rs( sign_extension<XLEN>( instr->v_src[0]), shamt_v_src2<RegisterUInt>( instr)); }
    static void rori( Instr* instr) { instr->v_dst[0] = circ_rs( sign_extension<XLEN>( instr->v_src[0]), shamt_imm( instr)); }

    // MIPS extra shifts
    static void dsll32( Instr* instr) { instr->v_dst[0] = instr->v_src[0] << shamt_imm_32( instr); }
    static void dsrl32( Instr* instr) { instr->v_dst[0] = instr->v_src[0] >> shamt_imm_32( instr); }
    static void dsra32( Instr* instr) { instr->v_dst[0] = arithmetic_rs( instr->v_src[0], shamt_imm_32( instr)); }
    template<size_t N> static void upper_immediate( Instr* instr)  { instr->v_dst[0] = instr->v_imm << N; }
    static void auipc( Instr* instr) { upper_immediate<12>( instr); instr->v_dst[0] += instr->PC; }

    // Leading zero/ones
    template<typename T> static void clo( Instr* instr)  { instr->v_dst[0] = count_leading_ones<T>( instr->v_src[0]); }
    template<typename T> static void clz( Instr* instr)  { instr->v_dst[0] = count_leading_zeroes<T>( instr->v_src[0]); }
    template<typename T> static void ctz( Instr* instr)  { instr->v_dst[0] = count_trailing_zeroes<T>( instr->v_src[0]); }
    template<typename T> static void pcnt( Instr* instr) { instr->v_dst[0] = narrow_cast<T>( popcount( instr->v_src[0])); }

    // Logic
    static void andv( Instr* instr)  { instr->v_dst[0] = instr->v_src[0] & instr->v_src[1]; }
    static void orv( Instr* instr)   { instr->v_dst[0] = instr->v_src[0] | instr->v_src[1]; }
    static void xorv( Instr* instr)  { instr->v_dst[0] = instr->v_src[0] ^ instr->v_src[1]; }
    static void nor( Instr* instr)   { instr->v_dst[0] = ~(instr->v_src[0] | instr->v_src[1]); }
    static void andi( Instr* instr)  { instr->v_dst[0] = instr->v_src[0] & instr->v_imm; }
    static void ori( Instr* instr)   { instr->v_dst[0] = instr->v_src[0] | instr->v_imm; }
    static void xori( Instr* instr)  { instr->v_dst[0] = instr->v_src[0] ^ instr->v_imm; }
    static void orn( Instr* instr)   { instr->v_dst[0] = instr->v_src[0] | ~instr->v_src[1]; }
    static void xnor( Instr* instr)  { instr->v_dst[0] = instr->v_src[0] ^ ~instr->v_src[1]; }

    // Bit permutation
    static void grev( Instr* instr) { instr->v_dst[0] = gen_reverse( instr->v_src[0], shamt_v_src2<RegisterUInt>( instr)); }
    static void riscv_unshfl( Instr* instr)
    {
        auto dst_value = instr->v_src[0];
        constexpr size_t limit = log_bitwidth<RegisterUInt> - 1;
        for ( size_t i = 0; i < limit; ++i)
            if ( ( instr->v_src[1] >> i) & 1U)
                dst_value = bit_shuffle( dst_value, i);
        instr->v_dst[0] = dst_value;
    }

    static void riscv_shfl( Instr* instr)
    {
        auto dst_value = instr->v_src[0];
        constexpr size_t limit = log_bitwidth<RegisterUInt> - 1;
        for ( size_t i = limit ; i > 0; --i)
            if( ( instr->v_src[1] >> (i - 1)) & 1U)
                dst_value = bit_shuffle( dst_value, i - 1);
        instr->v_dst[0] = dst_value;
    }

    // Generalized OR-Combine
    template<typename T> static void gorc( Instr* instr) { instr->v_dst[0] = gen_or_combine( instr->v_src[0], shamt_v_src2<T>( instr)); }
    static void gorci( Instr* instr) { instr->v_dst[0] = gen_or_combine( instr->v_src[0], shamt_imm( instr)); }

    // Conditional moves
    static void movn( Instr* instr)  { move( instr); if (instr->v_src[1] == 0) instr->mask = 0; }
    static void movz( Instr* instr)  { move( instr); if (instr->v_src[1] != 0) instr->mask = 0; }

    // Bit manipulations
    template<typename T> static void sbinv( Instr* instr) { instr->v_dst[0] = instr->v_src[0] ^ ( lsb_set<T>() << shamt_v_src2<T>( instr)); }
    template<typename T> static void sbext( Instr* instr) { instr->v_dst[0] = 1U & ( instr->v_src[0] >> shamt_v_src2<T>( instr)); }

    static void max( Instr* instr)  { instr->v_dst[0] = instr->v_src[ge( instr) ? 0 : 1]; }
    static void maxu( Instr* instr) { instr->v_dst[0] = std::max( instr->v_src[0], instr->v_src[1]); }
    static void min( Instr* instr)  { instr->v_dst[0] = instr->v_src[lt( instr) ? 0 : 1]; }
    static void minu( Instr* instr) { instr->v_dst[0] = std::min( instr->v_src[0], instr->v_src[1]); }

    template<typename T> static
    void clmul( Instr* instr)
    {
        instr->v_dst[0] = 0;
        for ( std::size_t index = 0; index < bitwidth<T>; index++)
            if ( ( instr->v_src[1] >> index) & 1U)
                instr->v_dst[0] ^= instr->v_src[0] << index;
    }

    // Bit manipulations
    template<typename T> static
    void pack( Instr* instr)
    {
        auto pack_width = half_bitwidth<T>;
        instr->v_dst[0] = ( instr->v_src[0] & bitmask<T>( pack_width)) | ( instr->v_src[1] << pack_width);
    }

    template<typename T> static
    void packu( Instr* instr)
    {
        auto pack_width = half_bitwidth<T>;
        instr->v_dst[0] = ( (instr->v_src[0] >> pack_width) | (instr->v_src[1] & (bitmask<T>(pack_width) << pack_width)));
    }

    // Branches
    template<Predicate p> static
    void branch( Instr* instr)
    {
        instr->is_taken_branch = p( instr);
        if ( instr->is_taken_branch) {
            instr->new_PC = instr->get_decoded_target();
            check_halt_trap( instr);
        }
    }

    static void jump( Instr* instr, Addr target)
    {
        instr->is_taken_branch = true;
        instr->new_PC = target;
        check_halt_trap( instr);
    }

    static void j( Instr* instr) { jump(instr, instr->get_decoded_target()); }
    static void riscv_jr( Instr* instr) { jump( instr, narrow_cast<Addr>( instr->v_src[0] + instr->v_imm));; }

    static void jr( Instr* instr) {
        if (instr->v_src[0] % 4 != 0)
            instr->trap = Trap::UNALIGNED_FETCH;
        jump( instr, align_up<2>(instr->v_src[0]));
    }

    template<Execute j> static
    void jump_and_link( Instr* instr)
    {
        instr->v_dst[0] = narrow_cast<RegisterUInt>( instr->new_PC); // link
        j( instr);   // jump
    }

    template<Predicate p> static
    void branch_and_link( Instr* instr)
    {
        instr->is_taken_branch = p( instr);
        instr->v_dst[0] = narrow_cast<RegisterUInt>( instr->new_PC);
        if ( instr->is_taken_branch)
        {
            instr->new_PC = instr->get_decoded_target();
            check_halt_trap( instr);
        }
    }

    static void eret( Instr* instr)
    {
        // FIXME(pikryukov): That should behave differently for ErrorEPC
        jump( instr, instr->v_src[0]);
        instr->v_dst[0] &= instr->v_src[1] & ~(1U << 2);
    }

    // Traps
    static void breakpoint( Instr* instr)   { instr->trap = Trap::BREAKPOINT; }
    static void syscall   ( Instr* instr)   { instr->trap = Trap::SYSCALL;    }
    static void halt( Instr* instr)   { instr->trap = Trap::HALT; }
    template<Predicate p> static void trap( Instr* instr) { if (p( instr)) instr->trap = Trap::EXPLICIT_TRAP; }
    static void unknown_instruction( Instr* instr) { instr->trap = Trap::UNKNOWN_INSTRUCTION; }

    static void csrrw( Instr* instr)
    {
        instr->v_dst[0]  = instr->v_src[0]; // CSR <- RS1
        instr->v_dst[1] = instr->v_src[1]; // RD  <- CSR
    }

    static void csrrs( Instr* instr)
    {
        instr->mask   = instr->v_src[0];
        instr->v_dst[0]  = all_ones<RegisterUInt>(); // CSR <- 0xffff & RS1
        instr->v_dst[1] = instr->v_src[1]; // RD <- CSR
    }

    static void csrrwi( Instr* instr)
    {
        instr->v_dst[0]  = instr->v_imm;  // CSR <- RS1
        instr->v_dst[1] = instr->v_src[1]; // RD  <- CSR
    }

    template<typename T> static
    void riscv_addition_imm( Instr* instr)
    {
        instr->v_dst[0] = sign_extension<bitwidth<T>>( instr->v_src[0] + instr->v_imm);
    }

    static void bit_field_place( Instr* instr)
    {
        size_t len = ( narrow_cast<size_t>( instr->v_src[1]) >> 24) & 15U;
        len = len > 0 ? len : 16;
        size_t off = ( narrow_cast<size_t>( instr->v_src[1]) >> 16) & ( XLEN-1);
        auto mask = bitmask<RegisterUInt>( len) << off;
        auto data = instr->v_src[1] << off;
        instr->v_dst[0] = ( data & mask) | ( instr->v_src[0] & ~mask);
    }
};

#endif
