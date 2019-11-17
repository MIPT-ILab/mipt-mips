/*
 * alu.h - implementation of all execution units
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2014-2017 MIPT-MIPS
 */

#ifndef ALU_H
#define ALU_H

#include "multiplication.h"
#include "traps/trap.h"

#include <infra/macro.h>
#include <tuple>

template<size_t N, typename T>
T align_up(T value) { return ((value + ((1ull << N) - 1)) >> N) << N; }

struct ALU
{
    // Generic  
    template<typename I> using Predicate = bool (*)( const I*);
    template<typename I> using Execute = void (*)( I*);
    template<typename I> static size_t shamt_imm( const I* instr) { return narrow_cast<size_t>( instr->v_imm); }
    template<typename I> static size_t shamt_imm_32( const I* instr) { return narrow_cast<size_t>( instr->v_imm) + 32U; }
    template<typename T, typename I> static size_t shamt_v_src2( const I* instr) { return narrow_cast<size_t>( instr->v_src2 & bitmask<size_t>(log_bitwidth<T>)); }
    template<typename I> static void move( I* instr)   { instr->v_dst = instr->v_src1; }
    template<typename I, Predicate<I> p> static void set( I* instr)  { instr->v_dst = p( instr); }

    template<typename I> static
    void check_halt_trap( I* instr) {
        if ( instr->new_PC == 0)
            instr->trap = Trap::HALT;
    }

    template<typename I> static void load_addr( I* instr)  { addr( instr); }

    template<typename I> static
    void store_addr( I* instr) {
        addr( instr);
        instr->mask = bitmask<typename I::RegisterUInt>(instr->mem_size * 8);
    }

    template<typename I> static
    void load_addr_aligned( I* instr) {
        load_addr( instr);
        if ( instr->mem_addr % instr->mem_size != 0)
            instr->trap = Trap::UNALIGNED_LOAD;
    }

    template<typename I> static
    void load_addr_right32( I* instr) {
        // Endian specific
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

    template<typename I> static
    void load_addr_left32( I* instr) {
        // Endian specific
        load_addr( instr);
        /* switch (instr->mem_addr % 4) {
           case 0: return 0xFF00'0000;
           case 1: return 0xFFFF'0000;
           case 2: return 0xFFFF'FF00;
           case 3: return 0xFFFF'FFFF;
           }
         */
        instr->mask = bitmask<typename I::RegisterUInt>( ( 1 + instr->mem_addr % 4) * 8) << ( ( 3 - instr->mem_addr % 4) * 8);
        // Actually we read a word LEFT to effective address
        instr->mem_addr -= 3;
    }

    // store functions done by analogy with loads
    template<typename I> static
    void store_addr_aligned( I* instr) {
        store_addr( instr);
        if ( instr->mem_addr % instr->mem_size != 0)
            instr->trap = Trap::UNALIGNED_STORE;
    }

    template<typename I> static
    void store_addr_right32( I* instr) {
        store_addr( instr);
        instr->mask = bitmask<uint32>( ( 4 - instr->mem_addr % 4) * 8);
    }

    template<typename I> static
    void store_addr_left32( I* instr) {
        store_addr( instr);
        instr->mask = bitmask<typename I::RegisterUInt>( ( 1 + instr->mem_addr % 4) * 8) << ( ( 3 - instr->mem_addr % 4) * 8);
        instr->mem_addr -= 3;
    }

    template<typename I> static void addr( I* instr) { instr->mem_addr = narrow_cast<Addr>( instr->v_src1 + instr->v_imm); }

    // Predicate helpers - unary
    template<typename I> static bool lez( const I* instr) { return narrow_cast<typename I::RegisterSInt>( instr->v_src1) <= 0; }
    template<typename I> static bool gez( const I* instr) { return narrow_cast<typename I::RegisterSInt>( instr->v_src1) >= 0; }
    template<typename I> static bool ltz( const I* instr) { return narrow_cast<typename I::RegisterSInt>( instr->v_src1) < 0; }
    template<typename I> static bool gtz( const I* instr) { return narrow_cast<typename I::RegisterSInt>( instr->v_src1) > 0; }

    // Predicate helpers - binary
    template<typename I> static bool eq( const I* instr)  { return instr->v_src1 == instr->v_src2; }
    template<typename I> static bool ne( const I* instr)  { return instr->v_src1 != instr->v_src2; }
    template<typename I> static bool geu( const I* instr) { return instr->v_src1 >= instr->v_src2; }
    template<typename I> static bool ltu( const I* instr) { return instr->v_src1 <  instr->v_src2; }
    template<typename I> static bool ge( const I* instr)  { return narrow_cast<typename I::RegisterSInt>( instr->v_src1) >= narrow_cast<typename I::RegisterSInt>( instr->v_src2); }
    template<typename I> static bool lt( const I* instr)  { return narrow_cast<typename I::RegisterSInt>( instr->v_src1) <  narrow_cast<typename I::RegisterSInt>( instr->v_src2); }

    // Predicate helpers - immediate
    template<typename I> static bool eqi( const I* instr) { return instr->v_src1 == instr->v_imm; }
    template<typename I> static bool nei( const I* instr) { return instr->v_src1 != instr->v_imm; }
    template<typename I> static bool lti( const I* instr) { return narrow_cast<typename I::RegisterSInt>( instr->v_src1) < narrow_cast<typename I::RegisterSInt>( instr->v_imm); }
    template<typename I> static bool gei( const I* instr) { return narrow_cast<typename I::RegisterSInt>( instr->v_src1) >= narrow_cast<typename I::RegisterSInt>( instr->v_imm); }
    template<typename I> static bool ltiu( const I* instr) { return instr->v_src1 < instr->v_imm; }
    template<typename I> static bool geiu( const I* instr) { return instr->v_src1 >= instr->v_imm; }

    // General addition
    template<typename I, typename T> static void addition( I* instr)     { instr->v_dst = narrow_cast<T>( instr->v_src1) + narrow_cast<T>( instr->v_src2); }
    template<typename I, typename T> static void subtraction( I* instr)  { instr->v_dst = narrow_cast<T>( instr->v_src1) - narrow_cast<T>( instr->v_src2); }
    template<typename I, typename T> static void riscv_addition( I* instr)     { instr->v_dst = sign_extension<bitwidth<T>, typename I::RegisterUInt>(narrow_cast<T>( instr->v_src1) + narrow_cast<T>( instr->v_src2)); }
    template<typename I, typename T> static void riscv_subtraction( I* instr)  { instr->v_dst = sign_extension<bitwidth<T>, typename I::RegisterUInt>(narrow_cast<T>( instr->v_src1) - narrow_cast<T>( instr->v_src2)); }
    template<typename I, typename T> static void addition_imm( I* instr) { instr->v_dst = narrow_cast<T>( instr->v_src1) + narrow_cast<T>( instr->v_imm); }

    template<typename I, typename T> static
    void addition_overflow( I* instr)
    {
        auto ret = test_addition_overflow<T>( instr->v_src1, instr->v_src2);
        if (ret.second)
            instr->trap = Trap::INTEGER_OVERFLOW;
        else
            instr->v_dst = ret.first;
    }

    template<typename I, typename T> static
    void addition_overflow_imm( I* instr)
    {
        auto ret = test_addition_overflow<T>( instr->v_src1, instr->v_imm);
        if (ret.second)
            instr->trap = Trap::INTEGER_OVERFLOW;
        else
            instr->v_dst = ret.first;
    }


    template<typename I, typename T> static
    void subtraction_overflow( I* instr)
    {
        auto ret = test_subtraction_overflow<T>( instr->v_src1, instr->v_src2);
        if (ret.second)
            instr->trap = Trap::INTEGER_OVERFLOW;
        else
            instr->v_dst = ret.first;
    }

    // RISCV mul/div
    template<typename I, typename T> static void riscv_mult_h_uu( I* instr) { instr->v_dst = riscv_multiplication_high_uu<T>(instr->v_src1, instr->v_src2); }
    template<typename I, typename T> static void riscv_mult_h_ss( I* instr) { instr->v_dst = riscv_multiplication_high_ss<T>(instr->v_src1, instr->v_src2); }
    template<typename I, typename T> static void riscv_mult_h_su( I* instr) { instr->v_dst = riscv_multiplication_high_su<T>(instr->v_src1, instr->v_src2); }
    template<typename I, typename T> static void riscv_mult_l( I* instr) { instr->v_dst = riscv_multiplication_low <T>(instr->v_src1, instr->v_src2); }
    template<typename I, typename T> static void riscv_div( I* instr) { instr->v_dst = riscv_division <T>(instr->v_src1, instr->v_src2); }
    template<typename I, typename T> static void riscv_rem( I* instr) { instr->v_dst = riscv_remainder <T>(instr->v_src1, instr->v_src2); }

    // MIPS mul/div
    template<typename I, typename T> static void multiplication( I* instr)
    {
        const auto& result = mips_multiplication<T>( instr->v_src1, instr->v_src2);
        instr->v_dst  = narrow_cast<typename I::RegisterUInt>( result.first);
        instr->v_dst2 = narrow_cast<typename I::RegisterUInt>( result.second);
    }

    template<typename I, typename T> static void division( I* instr)
    {
        const auto& result = mips_division<T>( instr->v_src1, instr->v_src2);
        instr->v_dst  = narrow_cast<typename I::RegisterUInt>( result.first);
        instr->v_dst2 = narrow_cast<typename I::RegisterUInt>( result.second);
    }

    // Shifts
    template<typename I, typename T> static void sll( I* instr)  { instr->v_dst = sign_extension<bitwidth<T>>( ( instr->v_src1 & all_ones<T>()) << shamt_imm( instr)); }
    template<typename I, typename T> static void srl( I* instr)  { instr->v_dst = sign_extension<bitwidth<T>>( ( instr->v_src1 & all_ones<T>()) >> shamt_imm( instr)); }
    template<typename I, typename T> static void sra( I* instr)  { instr->v_dst = arithmetic_rs( sign_extension<bitwidth<T>>( instr->v_src1), shamt_imm( instr)); }
    template<typename I, typename T> static void sllv( I* instr) { instr->v_dst = sign_extension<bitwidth<T>>( ( instr->v_src1 & all_ones<T>()) << shamt_v_src2<T>( instr)); }
    template<typename I, typename T> static void srlv( I* instr) { instr->v_dst = sign_extension<bitwidth<T>>( ( instr->v_src1 & all_ones<T>()) >> shamt_v_src2<T>( instr)); }
    template<typename I, typename T> static void srav( I* instr) { instr->v_dst = arithmetic_rs( sign_extension<bitwidth<T>>( instr->v_src1), shamt_v_src2<T>( instr)); }
    template<typename I> static void slo( I* instr) { instr->v_dst = ones_ls( sign_extension<bitwidth<typename I::RegisterUInt>>( instr->v_src1), shamt_v_src2<typename I::RegisterUInt>( instr)); }

    // MIPS extra shifts
    template<typename I> static void dsll32( I* instr) { instr->v_dst = instr->v_src1 << shamt_imm_32( instr); }
    template<typename I> static void dsrl32( I* instr) { instr->v_dst = instr->v_src1 >> shamt_imm_32( instr); }
    template<typename I> static void dsra32( I* instr) { instr->v_dst = arithmetic_rs( instr->v_src1, shamt_imm_32( instr)); }
    template<typename I, size_t N> static void upper_immediate( I* instr)  { instr->v_dst = instr->v_imm << N; }
    template<typename I> static void auipc( I* instr) { upper_immediate<I, 12>( instr); instr->v_dst += instr->PC; }

    // Leading zero/ones
    template<typename I, typename T> static void clo( I* instr)  { instr->v_dst = count_leading_ones<T>( instr->v_src1); }
    template<typename I, typename T> static void clz( I* instr)  { instr->v_dst = count_leading_zeroes<T>( instr->v_src1); }

    // Logic
    template<typename I> static void andv( I* instr)  { instr->v_dst = instr->v_src1 & instr->v_src2; }
    template<typename I> static void orv( I* instr)   { instr->v_dst = instr->v_src1 | instr->v_src2; }
    template<typename I> static void xorv( I* instr)  { instr->v_dst = instr->v_src1 ^ instr->v_src2; }
    template<typename I> static void nor( I* instr)   { instr->v_dst = ~(instr->v_src1 | instr->v_src2); }
    template<typename I> static void andi( I* instr)  { instr->v_dst = instr->v_src1 & instr->v_imm; }
    template<typename I> static void ori( I* instr)   { instr->v_dst = instr->v_src1 | instr->v_imm; }
    template<typename I> static void xori( I* instr)  { instr->v_dst = instr->v_src1 ^ instr->v_imm; }
    template<typename I> static void orn( I* instr)   { instr->v_dst = instr->v_src1 | ~instr->v_src2; }

    // Conditional moves
    template<typename I> static void movn( I* instr)  { move( instr); if (instr->v_src2 == 0) instr->mask = 0; }
    template<typename I> static void movz( I* instr)  { move( instr); if (instr->v_src2 != 0) instr->mask = 0; }
    
    // Bit manipulations
    template<typename I> static void sbext( I* instr) { instr->v_dst = 1 & ( instr->v_src1 >> shamt_v_src2<typename I::RegisterUInt>( instr)); }

    // Bit manipulations
    template<typename I, typename T> static void pack( I* instr)  { instr->v_dst = (instr->v_src1 & (bitmask<T>(half_bitwidth<T>))) | (instr->v_src2 << (half_bitwidth<T>)); }

    // Branches
    template<typename I, Predicate<I> p> static
    void branch( I* instr)
    {
        instr->is_taken_branch = p( instr);
        if ( instr->is_taken_branch) {
            instr->new_PC = instr->get_decoded_target();
            check_halt_trap( instr);
        }
    }

    template<typename I> static
    void jump( I* instr, Addr target)
    {
        instr->is_taken_branch = true;
        instr->new_PC = target;
        check_halt_trap( instr);
    }

    template<typename I> static void j( I* instr) { jump(instr, instr->get_decoded_target()); }
    template<typename I> static void riscv_jr( I* instr) { jump( instr, narrow_cast<Addr>( instr->v_src1 + instr->v_imm));; }

    template<typename I> static
    void jr( I* instr) {
        if (instr->v_src1 % 4 != 0)
            instr->trap = Trap::UNALIGNED_FETCH;
        jump( instr, align_up<2>(instr->v_src1));
    }

    template<typename I, Execute<I> j> static
    void jump_and_link( I* instr)
    {
        instr->v_dst = instr->new_PC; // link
        j( instr);   // jump
    }

    template<typename I, Predicate<I> p> static
    void branch_and_link( I* instr)
    {
        instr->is_taken_branch = p( instr);
        instr->v_dst = instr->new_PC;
        if ( instr->is_taken_branch)
        {
            instr->new_PC = instr->get_decoded_target();
            check_halt_trap( instr);
        }
    }

    template<typename I> static
    void eret( I* instr)
    {
        // FIXME(pikryukov): That should behave differently for ErrorEPC
        jump( instr, instr->v_src1);
        instr->v_dst &= instr->v_src2 & ~(1 << 2);
    }

    // Traps
    template<typename I> static void breakpoint( I* instr)   { instr->trap = Trap::BREAKPOINT; }
    template<typename I> static void syscall   ( I* instr)   { instr->trap = Trap::SYSCALL;    }
    template<typename I> static void halt( I* instr)   { instr->trap = Trap::HALT; }
    template<typename I, Predicate<I> p> static void trap( I* instr) { if (p( instr)) instr->trap = Trap::EXPLICIT_TRAP; }
    template<typename I> static void unknown_instruction( I* instr) { instr->trap = Trap::UNKNOWN_INSTRUCTION; }

    template<typename I> static
    void csrrw( I* instr)
    {
        instr->v_dst  = instr->v_src1; // CSR <- RS1
        instr->v_dst2 = instr->v_src2; // RD  <- CSR
    }

    template<typename I> static
    void csrrs( I* instr)
    {
        instr->mask   = instr->v_src1;
        instr->v_dst  = all_ones<typename I::RegisterUInt>(); // CSR <- 0xffff & RS1
        instr->v_dst2 = instr->v_src2; // RD <- CSR
    }

    template<typename I> static
    void csrrwi( I* instr)
    {
        instr->v_dst  = instr->v_imm;  // CSR <- RS1
        instr->v_dst2 = instr->v_src2; // RD  <- CSR
    }

    template<typename I, typename T> static
    void riscv_addition_imm( I* instr)
    {
        instr->v_dst = sign_extension<bitwidth<T>>( instr->v_src1 + instr->v_imm);
    }
};

#endif
