/*
 * alu.h - implementation of all execution units
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2014-2017 MIPT-MIPS
 */

#ifndef ALU_H
#define ALU_H

#include "trap_types.h"

#include <infra/macro.h>

#include <tuple>

template<size_t N, typename T>
T align_up(T value) { return ((value + ((1ull << N) - 1)) >> N) << N; }

template<typename T>
auto mips_multiplication(T x, T y) {
    using UT = unsign_t<T>;
    using T2 = doubled_t<T>;
    using UT2 = unsign_t<T2>;
    auto value = narrow_cast<UT2>(T2{ x} * T2{ y});
    // With Boost < 1.68.0, result of narrowing cast of uint128 is undefined
    // if the value does not fit to the built-in target type (e.g. uint64)
    // To workaround that, we mask the value with full-ones mask first.
    auto lo = narrow_cast<UT>( value & all_ones<UT>());
    auto hi = narrow_cast<UT>( value >> bitwidth<T>);
    return std::make_pair( lo, hi);
}

template<typename T>
auto mips_division(T x, T y) {
    using ReturnType = std::pair<unsign_t<T>, unsign_t<T>>;
    if ( y == 0)
        return ReturnType{};

    if constexpr( !std::is_same_v<T, unsign_t<T>>) // signed type NOLINTNEXTLINE(bugprone-suspicious-semicolon)
        if ( y == -1 && x == narrow_cast<T>(msb_set<unsign_t<T>>())) // x86 has an exception here
            return ReturnType{};

    return ReturnType(x / y, x % y);
}

struct ALU
{
    template<typename I> static auto sign_extend( const I* instr) {
        using T = typename I::RegisterSInt;
        return T{ narrow_cast<int16>(instr->v_imm)};
    }

    template<typename I> static auto sign_extend_12( const I* instr) {
        return sign_extension<12, typename I::RegisterUInt>( instr->v_imm);
    }

    template<typename I> static auto zero_extend( const I* instr) {
        using T = typename I::RegisterUInt;
        return T{ narrow_cast<uint16>(instr->v_imm)};
    }

    template<typename I> static size_t shamt_imm( const I* instr) {
        return narrow_cast<size_t>( instr->v_imm);
    }

    template<typename I> static size_t shamt_imm_32( const I* instr) {
        return narrow_cast<size_t>( instr->v_imm) + 32U;
    }

    template<typename T, typename I> static size_t shamt_v_src2( const I* instr) {
        return narrow_cast<size_t>( instr->v_src2 & bitmask<size_t>(log_bitwidth<T>));
    }

    template<typename I>
    using Predicate = bool (*)( const I*);

    template<typename I>
    using Execute = void (*)( I*);

    template<typename I> static
    void check_halt_trap( I* instr) {
        if ( instr->new_PC == 0)
            instr->trap = Trap::HALT;
    }

    template<typename I> static
    void load_addr( I* instr)  { addr( instr); }

    template<typename I> static
    void store_addr( I* instr) {
        addr( instr);
        instr->mask = bitmask<typename I::RegisterUInt>(instr->mem_size * 8);
    }

    template<typename I> static
    void load_addr_aligned( I* instr) {
        load_addr( instr);
        if ( instr->mem_addr % 4 != 0)
            instr->trap = Trap::UNALIGNED_ADDRESS;
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
        if ( instr->mem_addr % 4 != 0)
            instr->trap = Trap::UNALIGNED_ADDRESS;
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

    template<typename I> static
    void addr( I* instr) { instr->mem_addr = instr->v_src1 + sign_extend( instr); }

    template<typename I> static
    void riscv_addr( I* instr) { instr->mem_addr = narrow_cast<Addr>( instr->v_src1 + sign_extend_12( instr)); }

    template<typename I> static
    void riscv_store_addr( I* instr) {
        riscv_addr( instr);
        instr->mask = bitmask<typename I::RegisterUInt>(instr->mem_size * 8);
    }

    // Predicate helpers - unary
    template<typename I> static
    bool lez( const I* instr) { return narrow_cast<typename I::RegisterSInt>( instr->v_src1) <= 0; }

    template<typename I> static
    bool gez( const I* instr) { return narrow_cast<typename I::RegisterSInt>( instr->v_src1) >= 0; }

    template<typename I> static
    bool ltz( const I* instr) { return narrow_cast<typename I::RegisterSInt>( instr->v_src1) < 0; }

    template<typename I> static
    bool gtz( const I* instr) { return narrow_cast<typename I::RegisterSInt>( instr->v_src1) > 0; }

    // Predicate helpers - binary
    template<typename I> static
    bool eq( const I* instr)  { return instr->v_src1 == instr->v_src2; }

    template<typename I> static
    bool ne( const I* instr)  { return instr->v_src1 != instr->v_src2; }

    template<typename I> static
    bool geu( const I* instr) { return instr->v_src1 >= instr->v_src2; }

    template<typename I> static
    bool ltu( const I* instr) { return instr->v_src1 <  instr->v_src2; }

    template<typename I> static
    bool ge( const I* instr)  { return narrow_cast<typename I::RegisterSInt>( instr->v_src1) >= narrow_cast<typename I::RegisterSInt>( instr->v_src2); }

    template<typename I> static
    bool lt( const I* instr)  { return narrow_cast<typename I::RegisterSInt>( instr->v_src1) <  narrow_cast<typename I::RegisterSInt>( instr->v_src2); }

    // Predicate helpers - immediate
    template<typename I> static
    bool eqi( const I* instr) { return narrow_cast<typename I::RegisterSInt>( instr->v_src1) == sign_extend( instr); }

    template<typename I> static
    bool nei( const I* instr) { return narrow_cast<typename I::RegisterSInt>( instr->v_src1) != sign_extend( instr); }

    template<typename I> static
    bool lti( const I* instr) { return narrow_cast<typename I::RegisterSInt>( instr->v_src1) <  sign_extend( instr); }

    template<typename I> static
    bool lti_riscv( const I* instr) { return narrow_cast<typename I::RegisterSInt>( instr->v_src1) <  narrow_cast<typename I::RegisterSInt>( sign_extend_12( instr)); }

    template<typename I> static
    bool gei( const I* instr) { return narrow_cast<typename I::RegisterSInt>( instr->v_src1) >= sign_extend( instr); }

    // Predicate helpers - immediate unsigned
    template<typename I> static
    bool ltiu( const I* instr) { return instr->v_src1 <  narrow_cast<typename I::RegisterUInt>(sign_extend( instr)); }

    template<typename I> static
    bool ltiu_riscv( const I* instr) { return instr->v_src1 <  narrow_cast<typename I::RegisterUInt>(sign_extend_12( instr)); }

    template<typename I> static
    bool geiu( const I* instr) { return instr->v_src1 >= narrow_cast<typename I::RegisterUInt>(sign_extend( instr)); }

    template<typename I, typename T> static
    void addition( I* instr)     { instr->v_dst = narrow_cast<T>( instr->v_src1) + narrow_cast<T>( instr->v_src2); }

    template<typename I, typename T> static
    void subtraction( I* instr)  { instr->v_dst = narrow_cast<T>( instr->v_src1) - narrow_cast<T>( instr->v_src2); }

    template<typename I, typename T> static
    void riscv_addition( I* instr)     { instr->v_dst = sign_extension<bitwidth<T>, typename I::RegisterUInt>(narrow_cast<T>( instr->v_src1) + narrow_cast<T>( instr->v_src2)); }

    template<typename I, typename T> static
    void riscv_subtraction( I* instr)  { instr->v_dst = sign_extension<bitwidth<T>, typename I::RegisterUInt>(narrow_cast<T>( instr->v_src1) - narrow_cast<T>( instr->v_src2)); }

    template<typename I, typename T> static
    void addition_imm( I* instr) { instr->v_dst = narrow_cast<T>( instr->v_src1) + narrow_cast<T>( sign_extend( instr)); }

    template<typename I, typename T> static
    void addition_overflow( I* instr)
    {
        auto x = narrow_cast<T>( instr->v_src1);
        auto y = narrow_cast<T>( instr->v_src2);
        instr->v_dst = x + y;
//      if ( add_overflow( x, y))
//          instr->trap = Trap::INTEGER_OVERFLOW;
    }

    template<typename I, typename T> static
    void subtraction_overflow( I* instr)
    {
        auto x = narrow_cast<T>( instr->v_src1);
        auto y = narrow_cast<T>( instr->v_src2);
        instr->v_dst = x - y;
//      if ( sub_overflow( x, y))
//          instr->trap = Trap::INTEGER_OVERFLOW;
    }

    template<typename I, typename T> static
    void addition_overflow_imm( I* instr)
    {
        auto x = narrow_cast<T>( instr->v_src1);
        auto y = narrow_cast<T>( sign_extend( instr));
        instr->v_dst = x + y;
//      if ( add_overflow( x, y))
//          instr->trap = Trap::INTEGER_OVERFLOW;
    }

    template<typename I, typename T> static
    void multiplication( I* instr) { std::tie(instr->v_dst, instr->v_dst2) = mips_multiplication<T>(instr->v_src1, instr->v_src2); }

    template<typename I, typename T> static
    void division( I* instr) { std::tie(instr->v_dst, instr->v_dst2) = mips_division<T>(instr->v_src1, instr->v_src2); }

    template<typename I> static
    void move( I* instr)   { instr->v_dst = instr->v_src1; }

    template<typename I, typename T> static
    void sll( I* instr)   { instr->v_dst = sign_extension<bitwidth<T>>( ( instr->v_src1 & all_ones<T>()) << shamt_imm( instr)); }

    template<typename I, typename T> static
    void srl( I* instr) { instr->v_dst = sign_extension<bitwidth<T>>( ( instr->v_src1 & all_ones<T>()) >> shamt_imm( instr)); }

    template<typename I, typename T> static
    void sra( I* instr)   { instr->v_dst = arithmetic_rs( sign_extension<bitwidth<T>>( instr->v_src1), shamt_imm( instr)); }

    template<typename I, typename T> static
    void sllv( I* instr)   { instr->v_dst = sign_extension<bitwidth<T>>( ( instr->v_src1 & all_ones<T>()) << shamt_v_src2<T>( instr)); }

    template<typename I, typename T> static
    void srlv( I* instr)   { instr->v_dst = sign_extension<bitwidth<T>>( ( instr->v_src1 & all_ones<T>()) >> shamt_v_src2<T>( instr)); }

    template<typename I, typename T> static
    void srav( I* instr)   { instr->v_dst = arithmetic_rs( sign_extension<bitwidth<T>>( instr->v_src1), shamt_v_src2<T>( instr)); }

    template<typename I> static
    void dsll32( I* instr) { instr->v_dst = instr->v_src1 << shamt_imm_32( instr); }

    template<typename I> static
    void dsrl32( I* instr) { instr->v_dst = instr->v_src1 >> shamt_imm_32( instr); }

    template<typename I> static
    void dsra32( I* instr) { instr->v_dst = arithmetic_rs( instr->v_src1, shamt_imm_32( instr)); }

    template<typename I> static
    void mips_upper_immediate( I* instr) { instr->v_dst = narrow_cast<typename I::RegisterUInt>( sign_extend( instr)) << 16ULL; }

    template<typename I> static
    void riscv_upper_immediate( I* instr) { instr->v_dst = sign_extension<20, typename I::RegisterUInt>( instr->v_imm) << 12ULL; }

    template<typename I> static
    void andv( I* instr)   { instr->v_dst = instr->v_src1 & instr->v_src2; }

    template<typename I> static
    void orv( I* instr)    { instr->v_dst = instr->v_src1 | instr->v_src2; }

    template<typename I> static
    void xorv( I* instr)   { instr->v_dst = instr->v_src1 ^ instr->v_src2; }

    template<typename I> static
    void nor( I* instr)   { instr->v_dst = ~(instr->v_src1 | instr->v_src2); }

    template<typename I> static
    void andi( I* instr)  { instr->v_dst = instr->v_src1 & zero_extend( instr); }

    template<typename I> static
    void ori( I* instr)   { instr->v_dst = instr->v_src1 | zero_extend( instr); }

    template<typename I> static
    void xori( I* instr)  { instr->v_dst = instr->v_src1 ^ zero_extend( instr); }

    template<typename I> static
    void riscv_andi( I* instr)  { instr->v_dst = instr->v_src1 & sign_extend_12( instr); }

    template<typename I> static
    void riscv_ori( I* instr)   { instr->v_dst = instr->v_src1 | sign_extend_12( instr); }

    template<typename I> static
    void riscv_xori( I* instr)  { instr->v_dst = instr->v_src1 ^ sign_extend_12( instr); }

    template<typename I> static
    void movn( I* instr)  { move( instr); if (instr->v_src2 == 0) instr->mask = 0; }
   
    template<typename I> static
    void movz( I* instr)  { move( instr); if (instr->v_src2 != 0) instr->mask = 0; }

    // Function-templated method is a little-known feature of C++, but useful here
    template<typename I, Predicate<I> p> static
    void set( I* instr) { instr->v_dst = p( instr); }

    template<typename I, Predicate<I> p> static
    void trap( I* instr) { if (p( instr)) instr->trap = Trap::EXPLICIT_TRAP; }

    template<typename I, Predicate<I> p> static
    void branch( I* instr)
    {
        instr->is_taken_branch = p( instr);
        if ( instr->is_taken_branch) {
            instr->new_PC = instr->get_decoded_target();
            check_halt_trap( instr);
        }
        else {
            instr->new_PC = instr->PC + 4 * (1 + instr->get_delayed_slots());
        }
    }

    template<typename I> static
    void clo( I* instr)  { instr->v_dst = narrow_cast<uint32>( count_leading_ones<uint32>( instr->v_src1)); }

    template<typename I> static
    void dclo( I* instr) { instr->v_dst = narrow_cast<uint32>( count_leading_ones<uint64>( instr->v_src1)); }

    template<typename I> static
    void clz( I* instr)  { instr->v_dst = narrow_cast<uint32>( count_leading_zeroes<uint32>(  instr->v_src1)); }

    template<typename I> static
    void dclz( I* instr) { instr->v_dst = narrow_cast<uint32>( count_leading_zeroes<uint64>(  instr->v_src1)); }

    template<typename I> static
    void jump( I* instr, Addr target)
    {
        instr->is_taken_branch = true;
        instr->new_PC = target;
        check_halt_trap( instr);
    }

    template<typename I> static
    void j( I* instr)  { jump(instr, instr->get_decoded_target()); }

    template<typename I> static
    void jr( I* instr) {
        if (instr->v_src1 % 4 != 0)
            instr->trap = Trap::UNALIGNED_ADDRESS;
        jump( instr, align_up<2>(instr->v_src1));
    }

    template<typename I, Execute<I> j> static
    void jump_and_link( I* instr)
    {
        instr->v_dst = instr->PC + 4 * (1 + instr->get_delayed_slots()); // link
        j( instr);   // jump
    }

    template<typename I, Predicate<I> p> static
    void branch_and_link( I* instr)
    {
        instr->is_taken_branch = p( instr);
        if ( instr->is_taken_branch)
        {
            instr->v_dst = instr->PC + 4 * (1 + instr->get_delayed_slots());
            instr->new_PC = instr->get_decoded_target();
            check_halt_trap( instr);
        }
    }

    template<typename I> static
    void breakpoint( I* instr)   { instr->trap = Trap::BREAKPOINT; }

    template<typename I> static
    void halt( I* instr)   { instr->trap = Trap::HALT; }

    template<typename I> static
    void unknown_instruction( I* instr) { instr->trap = Trap::UNKNOWN_INSTRUCTION; }

    template<typename I> static
    void riscv_jump_and_link( I* instr)
    {
        instr->v_dst = instr->PC + 4;
        jump( instr, instr->get_decoded_target());
    }

    template<typename I> static
    void riscv_jump_and_link_register( I* instr)
    {
        instr->v_dst = instr->PC + 4;
        jump( instr, narrow_cast<Addr>( instr->v_src1 + sign_extend_12( instr)));
    }

    template<typename I> static
    void auipc( I* instr)
    {
        riscv_upper_immediate( instr);
        instr->v_dst += instr->PC;
    }

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
        instr->v_dst = sign_extension<bitwidth<T>>( instr->v_src1 + sign_extend_12( instr));
    }
};

#endif
