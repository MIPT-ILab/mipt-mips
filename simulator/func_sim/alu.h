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
    using T2 = doubled_t<T>;
    using UT2 = unsign_t<T2>;
    using ReturnType = std::pair<unsign_t<T>, unsign_t<T>>;
    auto value = narrow_cast<UT2>(T2{ x} * T2{ y});
    return ReturnType(value, value >> bitwidth<T>);
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

    template<typename I> static auto zero_extend( const I* instr) {
        using T = typename I::RegisterSInt;
        return T{ narrow_cast<uint16>(instr->v_imm)};
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
    bool gei( const I* instr) { return narrow_cast<typename I::RegisterSInt>( instr->v_src1) >= sign_extend( instr); }

    // Predicate helpers - immediate unsigned
    template<typename I> static
    bool ltiu( const I* instr) { return instr->v_src1 <  narrow_cast<typename I::RegisterUInt>(sign_extend( instr)); }

    template<typename I> static
    bool geiu( const I* instr) { return instr->v_src1 >= narrow_cast<typename I::RegisterUInt>(sign_extend( instr)); }

    template<typename I, typename T> static
    void addition( I* instr)     { instr->v_dst = narrow_cast<unsign_t<T>>( narrow_cast<T>( instr->v_src1) + narrow_cast<T>( instr->v_src2)); }

    template<typename I, typename T> static
    void subtraction( I* instr)  { instr->v_dst = narrow_cast<unsign_t<T>>( narrow_cast<T>( instr->v_src1) - narrow_cast<T>( instr->v_src2)); }

    template<typename I, typename T> static
    void addition_imm( I* instr) { instr->v_dst = narrow_cast<unsign_t<T>>( narrow_cast<T>( instr->v_src1) + narrow_cast<T>( sign_extend( instr))); }

    template<typename I, typename T> static
    void multiplication( I* instr) { std::tie(instr->v_dst, instr->v_dst2) = mips_multiplication<T>(instr->v_src1, instr->v_src2); }

    template<typename I, typename T> static
    void division( I* instr) { std::tie(instr->v_dst, instr->v_dst2) = mips_division<T>(instr->v_src1, instr->v_src2); }

    template<typename I> static
    void move( I* instr)   { instr->v_dst = instr->v_src1; }

    template<typename I, typename T> static
    void sll( I* instr)   { instr->v_dst = narrow_cast<T>( instr->v_src1) << instr->v_imm; }

    template<typename I> static
    void dsll32( I* instr) { instr->v_dst = instr->v_src1 << (instr->v_imm + 32u); }

    template<typename I, typename T> static
    void srl( I* instr)
    {
        // On 64-bit CPUs the result word is sign-extended
        instr->v_dst = narrow_cast<typename I::RegisterUInt>(narrow_cast<typename I::RegisterSInt>(narrow_cast<unsign_t<T>>(narrow_cast<T>(instr->v_src1) >> instr->v_imm)));
    }

    template<typename I> static
    void dsrl32( I* instr) { instr->v_dst = instr->v_src1 >> (instr->v_imm + 32u); }

    template<typename I, typename T> static
    void sra( I* instr)   { instr->v_dst = arithmetic_rs( narrow_cast<T>( instr->v_src1), instr->v_imm); }

    template<typename I> static
    void dsra32( I* instr) { instr->v_dst = arithmetic_rs( instr->v_src1, instr->v_imm + 32); }

    template<typename I, typename T> static
    void sllv( I* instr)   { instr->v_dst = narrow_cast<T>( instr->v_src1) << instr->v_src2; }

    template<typename I, typename T> static
    void srlv( I* instr)   { instr->v_dst = narrow_cast<T>( instr->v_src1) >> instr->v_src2; }

    template<typename I, typename T> static
    void srav( I* instr)   { instr->v_dst = arithmetic_rs( narrow_cast<T>( instr->v_src1), instr->v_src2); }

    template<typename I> static
    void lui( I* instr)    { instr->v_dst = narrow_cast<typename I::RegisterUInt>( sign_extend( instr)) << 0x10u; }

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
        instr->_is_jump_taken = p( instr);
        if ( instr->_is_jump_taken) {
            instr->new_PC += sign_extend( instr) * 4;
            check_halt_trap( instr);
        }
    }

    template<typename I> static
    void clo( I* instr)  { instr->v_dst = count_leading_ones<uint32>( instr->v_src1); }

    template<typename I> static
    void dclo( I* instr) { instr->v_dst = count_leading_ones<uint64>( instr->v_src1); }

    template<typename I> static
    void clz( I* instr)  { instr->v_dst = count_leading_zeroes<uint32>(  instr->v_src1); }

    template<typename I> static
    void dclz( I* instr) { instr->v_dst = count_leading_zeroes<uint64>(  instr->v_src1); }

    template<typename I> static
    void jump( I* instr, Addr target)
    {
        instr->_is_jump_taken = true;
        instr->new_PC = target;
        check_halt_trap( instr);
    }

    template<typename I> static
    void j( I* instr)  { jump(instr, (instr->PC & 0xf0000000) | (instr->v_imm << 2u)); }

    template<typename I> static
    void jr( I* instr) {
        if (instr->v_src1 % 4 != 0)
            instr->trap = Trap::UNALIGNED_ADDRESS;
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
        instr->_is_jump_taken = p( instr);
        if ( instr->_is_jump_taken)
        {
            instr->v_dst = instr->new_PC;
            instr->new_PC += sign_extend( instr) * 4;
            check_halt_trap( instr);
        }
    }

    template<typename I> static
    void syscall( I* instr) { instr->trap = Trap::SYSCALL; }

    template<typename I> static
    void breakpoint( I* instr)   { instr->trap = Trap::BREAKPOINT; }
};

#endif
