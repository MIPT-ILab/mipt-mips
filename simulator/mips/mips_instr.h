 /* mips_instr.h - instruction parser for mips
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2014-2017 MIPT-MIPS
 */

/** Edited by Ladin Oleg. */

#ifndef MIPS_INSTR_H
#define MIPS_INSTR_H

#include "mips_register/mips_register.h"
#include "mips_version.h"

// MIPT-MIPS modules
#include <func_sim/trap_types.h>
#include <infra/endian.h>
#include <infra/exception.h>
#include <infra/macro.h>
#include <infra/string_view.h>
#include <infra/types.h>
#include <kryucow_string.h>

// Generic C++
#include <array>
#include <unordered_map>

template<size_t N, typename T>
T align_up(T value) { return ((value + ((1ull << N) - 1)) >> N) << N; }

template<typename T>
auto mips_multiplication(T x, T y) {
    using T2 = doubled_t<T>;
    using UT2 = unsign_t<T2>;
    using ReturnType = std::pair<unsign_t<T>, unsign_t<T>>;
    auto value = static_cast<UT2>(static_cast<T2>(x) * static_cast<T2>(y));
    return ReturnType(value, value >> bitwidth<T>);
}

template<typename T>
auto mips_division(T x, T y) {
    using ReturnType = std::pair<unsign_t<T>, unsign_t<T>>;
    if ( y == 0)
        return ReturnType();

    if constexpr( !std::is_same_v<T, unsign_t<T>>) // signed type NOLINTNEXTLINE(bugprone-suspicious-semicolon)
        if ( y == -1 && x == static_cast<T>(msb_set<unsign_t<T>>())) // x86 has an exception here
            return ReturnType();

    return ReturnType(x / y, x % y);
}

struct UnknownMIPSInstruction final : Exception
{
    explicit UnknownMIPSInstruction(const std::string& msg)
        : Exception("Unknown MIPS instruction is an unhandled trap", msg)
    { }
};

template<typename RegisterUInt>
class BaseMIPSInstr
{
    private:
        using RegisterSInt = sign_t<RegisterUInt>;

        enum OperationType : uint8
        {
            OUT_R_ARITHM,
            OUT_R_ACCUM,
            OUT_R_CONDM,
            OUT_R_SHAMT,
            OUT_R_JUMP,
            OUT_R_SPECIAL,
            OUT_R_SUBTR,
            OUT_R_TRAP,
            OUT_I_ARITHM,
            OUT_I_BRANCH,
            OUT_RI_BRANCH_0,
            OUT_RI_TRAP,
            OUT_I_LOAD,
            OUT_I_LOADU,
            OUT_I_PARTIAL_LOAD,
            OUT_I_CONST,
            OUT_I_STORE,
            OUT_J_JUMP,
            OUT_J_SPECIAL,
            OUT_UNKNOWN
        } operation = OUT_UNKNOWN;

        Trap trap = Trap::NO_TRAP;

        // Endian specific
        const union _instr
        {
            const struct AsR
            {
                uint32 funct  :6;
                uint32 shamt  :5;
                uint32 rd     :5;
                uint32 rt     :5;
                uint32 rs     :5;
                uint32 opcode :6;
            } asR;
            const struct AsI
            {
                uint32 imm    :16;
                uint32 rt     :5;
                uint32 rs     :5;
                uint32 opcode :6;
            } asI;
            const struct AsJ
            {
                uint32 imm    :26;
                uint32 opcode :6;
            } asJ;

            const uint32 raw;

            _instr() : raw(NO_VAL32) { };
            explicit _instr(uint32 bytes) : raw( bytes) { }

            static_assert( sizeof( AsR) == sizeof( uint32));
            static_assert( sizeof( AsI) == sizeof( uint32));
            static_assert( sizeof( AsJ) == sizeof( uint32));
            static_assert( sizeof( uint32) == 4);
        } instr;

        using Execute = void (BaseMIPSInstr::*)();
        using Predicate = bool (BaseMIPSInstr::*)() const;

        enum class RegType : uint8
        {
            RS, RT, RD,
            ZERO, RA,
            HI, LO, HI_LO
        };

        static bool is_explicit_register( RegType type)
        {
            return type == RegType::RS
                || type == RegType::RT
                || type == RegType::RD;
        }

        MIPSRegister get_register( RegType type) const;

        struct ISAEntry
        {
            std::string_view name;
            OperationType operation;
            uint8 mem_size;
            RegType src1;
            RegType src2;
            RegType dst;
            BaseMIPSInstr::Execute function;
            MIPSVersionMask versions;
            ISAEntry() = delete;
        };

        using MapType = std::unordered_map <uint8, BaseMIPSInstr<RegisterUInt>::ISAEntry>;
        static const MapType isaMapR;
        static const MapType isaMapRI;
        static const MapType isaMapIJ;
        static const MapType isaMapMIPS32;

        MIPSRegister src1 = MIPSRegister::zero;
        MIPSRegister src2 = MIPSRegister::zero;
        MIPSRegister dst  = MIPSRegister::zero;
        MIPSRegister dst2 = MIPSRegister::zero;

        uint32 v_imm = NO_VAL32;
        auto sign_extend() const { return static_cast<RegisterSInt>( static_cast<int16>(v_imm)); }
        auto zero_extend() const { return static_cast<RegisterUInt>( static_cast<uint16>(v_imm)); }

        RegisterUInt v_src1 = NO_VAL<RegisterUInt>;
        RegisterUInt v_src2 = NO_VAL<RegisterUInt>;
        RegisterUInt v_dst  = NO_VAL<RegisterUInt>;
        RegisterUInt v_dst2 = NO_VAL<RegisterUInt>;
        RegisterUInt mask   = all_ones<RegisterUInt>();

        uint16 shamt = NO_VAL16;
        Addr mem_addr = NO_VAL32;
        uint32 mem_size = NO_VAL32;

        // convert this to bitset
        bool complete   = false;
        bool _is_jump_taken = false; // actual result
        bool memory_complete = false;

        Addr new_PC = NO_VAL32;

        const Addr PC = NO_VAL32;

        uint64 sequence_id = NO_VAL64;

        KryuCowString disasm = {};

        void init( const ISAEntry& entry, MIPSVersion version);

        // Predicate helpers - unary
        bool lez() const { return static_cast<RegisterSInt>( v_src1) <= 0; }
        bool gez() const { return static_cast<RegisterSInt>( v_src1) >= 0; }
        bool ltz() const { return static_cast<RegisterSInt>( v_src1) < 0; }
        bool gtz() const { return static_cast<RegisterSInt>( v_src1) > 0; }

        // Predicate helpers - binary
        bool eq()  const { return v_src1 == v_src2; }
        bool ne()  const { return v_src1 != v_src2; }
        bool geu() const { return v_src1 >= v_src2; }
        bool ltu() const { return v_src1 <  v_src2; }
        bool ge()  const { return static_cast<RegisterSInt>( v_src1) >= static_cast<RegisterSInt>( v_src2); }
        bool lt()  const { return static_cast<RegisterSInt>( v_src1) <  static_cast<RegisterSInt>( v_src2); }

        // Predicate helpers - immediate
        bool eqi() const { return static_cast<RegisterSInt>( v_src1) == sign_extend(); }
        bool nei() const { return static_cast<RegisterSInt>( v_src1) != sign_extend(); }
        bool lti() const { return static_cast<RegisterSInt>( v_src1) <  sign_extend(); }
        bool gei() const { return static_cast<RegisterSInt>( v_src1) >= sign_extend(); }

        // Predicate helpers - immediate unsigned
        bool ltiu() const { return v_src1 <  static_cast<RegisterUInt>(sign_extend()); }
        bool geiu() const { return v_src1 >= static_cast<RegisterUInt>(sign_extend()); }

        template <typename T>
        void execute_addition()     { v_dst = static_cast<unsign_t<T>>( static_cast<T>( v_src1) + static_cast<T>( v_src2)); }

        template <typename T>
        void execute_subtraction()  { v_dst = static_cast<unsign_t<T>>( static_cast<T>( v_src1) - static_cast<T>( v_src2)); }

        template <typename T>
        void execute_addition_imm() { v_dst = static_cast<unsign_t<T>>( static_cast<T>( v_src1) + static_cast<T>( sign_extend())); }

        template <typename T>
        void execute_multiplication() { std::tie(v_dst, v_dst2) = mips_multiplication<T>(v_src1, v_src2); }

        template <typename T>
        void execute_division() { std::tie(v_dst, v_dst2) = mips_division<T>(v_src1, v_src2); }

        void execute_move()   { v_dst = v_src1; }

        template <typename T>
        void execute_sll()   { v_dst = static_cast<T>( v_src1) << shamt; }
        void execute_dsll32() { v_dst = v_src1 << (shamt + 32u); }

        template <typename T>
        void execute_srl()
        {
            // On 64-bit CPUs the result word is sign-extended
            v_dst = static_cast<RegisterUInt>(static_cast<RegisterSInt>(static_cast<unsign_t<T>>(static_cast<T>(v_src1) >> shamt)));
        }
        void execute_dsrl32() { v_dst = v_src1 >> (shamt + 32u); }

        template <typename T>
        void execute_sra()   { v_dst = arithmetic_rs( static_cast<T>( v_src1), shamt); }
        void execute_dsra32() { v_dst = arithmetic_rs( v_src1, shamt + 32); }

        template <typename T>
        void execute_sllv()   { v_dst = static_cast<T>( v_src1) << v_src2; }

        template <typename T>
        void execute_srlv()   { v_dst = static_cast<T>( v_src1) >> v_src2; }

        template <typename T>
        void execute_srav()   { v_dst = arithmetic_rs( static_cast<T>( v_src1), v_src2); }
        void execute_lui()    { v_dst = static_cast<RegisterUInt>( sign_extend()) << 0x10u; }

        void execute_and()   { v_dst = v_src1 & v_src2; }
        void execute_or()    { v_dst = v_src1 | v_src2; }
        void execute_xor()   { v_dst = v_src1 ^ v_src2; }
        void execute_nor()   { v_dst = ~(v_src1 | v_src2); }

        void execute_andi()  { v_dst = v_src1 & zero_extend(); }
        void execute_ori()   { v_dst = v_src1 | zero_extend(); }
        void execute_xori()  { v_dst = v_src1 ^ zero_extend(); }

        void execute_movn()  { execute_move(); if (v_src2 == 0) mask = 0; }
        void execute_movz()  { execute_move(); if (v_src2 != 0) mask = 0; }

        void check_halt_trap() {
            if (new_PC == 0)
                trap = Trap::HALT;
        }

        // Function-templated method is a little-known feature of C++, but useful here
        template<Predicate p>
        void execute_set() { v_dst = (this->*p)(); }

        template<Predicate p>
        void execute_trap() { if ((this->*p)()) trap = Trap::EXPLICIT_TRAP; }

        template<Predicate p>
        void execute_branch()
        {
            _is_jump_taken = (this->*p)();
            if ( _is_jump_taken) {
                new_PC += sign_extend() * 4;
                check_halt_trap();
            }
        }

        void execute_clo()  { v_dst = count_leading_ones<uint32>( v_src1); }
        void execute_dclo() { v_dst = count_leading_ones<uint64>( v_src1); }
        void execute_clz()  { v_dst = count_leading_zeroes<uint32>(  v_src1); }
        void execute_dclz() { v_dst = count_leading_zeroes<uint64>(  v_src1); }

        void execute_jump( Addr target)
        {
            _is_jump_taken = true;
            new_PC = target;
            check_halt_trap();
        }

        void execute_j()  { execute_jump((PC & 0xf0000000) | (v_imm << 2u)); }
        void execute_jr() {
            if (v_src1 % 4 != 0)
                trap = Trap::UNALIGNED_ADDRESS;
            execute_jump(align_up<2>(v_src1));
        }

        template<Execute j>
        void execute_jump_and_link()
        {
            v_dst = new_PC; // link
            (this->*j)();   // jump
        }

        template<Predicate p>
        void execute_branch_and_link()
        {
            _is_jump_taken = (this->*p)();
            if ( _is_jump_taken)
            {
                v_dst = new_PC;
                new_PC += sign_extend() * 4;
                check_halt_trap();
            }
        }

        void execute_syscall() { trap = Trap::SYSCALL; };
        void execute_break()   { trap = Trap::BREAKPOINT; };

        void execute_unknown();
        void calculate_addr() { mem_addr = v_src1 + sign_extend(); }

        void calculate_load_addr()  { calculate_addr(); }
        void calculate_store_addr() {
            calculate_addr();
            mask = bitmask<RegisterUInt>(mem_size * 8);
        }

        void calculate_load_addr_aligned() {
            calculate_load_addr();
            if ( mem_addr % 4 != 0)
                trap = Trap::UNALIGNED_ADDRESS;
        }

        void calculate_load_addr_right32() {
            // Endian specific
            calculate_load_addr();
            /* switch (mem_addr % 4) {
               case 0: return 0xFFFF'FFFF;
               case 1: return 0x00FF'FFFF;
               case 2: return 0x0000'FFFF;
               case 3: return 0x0000'00FF;
               }
             */
            mask = bitmask<uint32>( ( 4 - mem_addr % 4) * 8);
        }

        void calculate_load_addr_left32() {
            // Endian specific
            calculate_load_addr();
            /* switch (mem_addr % 4) {
               case 0: return 0xFF00'0000;
               case 1: return 0xFFFF'0000;
               case 2: return 0xFFFF'FF00;
               case 3: return 0xFFFF'FFFF;
               }
             */
            mask = bitmask<RegisterUInt>( ( 1 + mem_addr % 4) * 8) << ( ( 3 - mem_addr % 4) * 8);
            // Actually we read a word LEFT to effective address
            mem_addr -= 3;
        }

        // store functions done by analogy with loads
        void calculate_store_addr_aligned() {
            calculate_store_addr();
            if ( mem_addr % 4 != 0)
                trap = Trap::UNALIGNED_ADDRESS;
        }

        void calculate_store_addr_right32() {
            calculate_store_addr();
            mask = bitmask<uint32>( ( 4 - mem_addr % 4) * 8);
        }

        void calculate_store_addr_left32() {
            calculate_store_addr();
            mask = bitmask<RegisterUInt>( ( 1 + mem_addr % 4) * 8) << ( ( 3 - mem_addr % 4) * 8);
            mem_addr -= 3;
        }

        Execute function = &BaseMIPSInstr::execute_unknown;
    protected:
        BaseMIPSInstr( MIPSVersion version, uint32 bytes, Addr PC);
    public:
        static const constexpr Endian endian = Endian::little;
    
        BaseMIPSInstr() = delete;

        bool is_same_bytes( uint32 bytes) const {
            return instr.raw == bytes;
        }

        bool is_same( const BaseMIPSInstr& rhs) const {
            return PC == rhs.PC && is_same_bytes( rhs.instr.raw);
        }
        
        bool is_same_checker( const BaseMIPSInstr& rhs) const {
            return is_same(rhs)
                && sequence_id == rhs.sequence_id
                && (dst.is_zero()  || v_dst == rhs.v_dst)
                && (dst2.is_zero() || v_dst2 == rhs.v_dst2);
        }

        MIPSRegister get_src_num( uint8 index) const { return ( index == 0) ? src1 : src2; }
        MIPSRegister get_dst_num()  const { return dst;  }
        MIPSRegister get_dst2_num() const { return dst2; }
        std::string_view get_disasm() const { return static_cast<std::string_view>( disasm); }

        /* Checks if instruction can change PC in unusual way. */
        bool is_jump() const { return operation == OUT_J_JUMP      ||
                                      operation == OUT_RI_BRANCH_0 ||
                                      operation == OUT_R_JUMP      ||
                                      operation == OUT_I_BRANCH;}
        bool is_jump_taken() const { return  _is_jump_taken; }

        bool is_partial_load() const
        {
            return operation == OUT_I_PARTIAL_LOAD;
        }

        bool is_load() const { return operation == OUT_I_LOAD ||
                                       operation == OUT_I_LOADU ||
                                       is_partial_load(); }

        int8 get_accumulation_type() const
        {
            return (operation == OUT_R_ACCUM) ? 1 : (operation == OUT_R_SUBTR) ? -1 : 0;
        }

        bool is_store() const { return operation == OUT_I_STORE; }

        bool is_nop() const { return instr.raw == 0x0u; }
        bool is_halt() const { return trap_type() == Trap::HALT; }

        bool is_conditional_move() const { return operation == OUT_R_CONDM; }

        bool is_divmult() const { return get_dst_num().is_mips_lo() && get_dst2_num().is_mips_hi(); }

        bool is_explicit_trap() const { return operation == OUT_R_TRAP ||
                                               operation == OUT_RI_TRAP; }

        bool is_special() const { return operation == OUT_R_SPECIAL; }

        bool has_trap() const { return trap_type() != Trap::NO_TRAP; }

        Trap trap_type() const { return trap; }

        bool is_bubble() const { return is_nop() && PC == 0; }

        void set_v_src( RegisterUInt value, uint8 index)
        {
            if ( index == 0)
                v_src1 = value;
            else
                v_src2 = value;
        }

        auto get_v_dst()  const { return v_dst; }
        auto get_v_dst2() const { return v_dst2; }
        auto get_mask()  const { return mask;  }

        auto get_mem_addr() const { return mem_addr; }
        auto get_mem_size() const { return mem_size; }
        auto get_new_PC() const { return new_PC; }
        auto get_PC() const { return PC; }

        void set_v_dst(RegisterUInt value); // for loads
        auto get_v_src2() const { return v_src2; } // for stores

        void execute();
        void check_trap() { };

        void set_sequence_id( uint64 id) { sequence_id = id; }
        auto get_sequence_id() const { return sequence_id; }
        
        std::ostream& dump( std::ostream& out) const;
        std::string string_dump() const;
};

template<typename RegisterUInt>
std::ostream& operator<<( std::ostream& out, const BaseMIPSInstr<RegisterUInt>& instr)
{
    return instr.dump( out);
}

template<MIPSVersion V>
class MIPSInstr : public BaseMIPSInstr<MIPSRegisterUInt<V>>
{
    using Base = BaseMIPSInstr<MIPSRegisterUInt<V>>;
public:
    explicit MIPSInstr( uint32 bytes, Addr PC = 0)
        : Base( V, bytes, PC) { }
};


using MIPS32Instr = MIPSInstr<MIPSVersion::v32>;
using MIPS64Instr = MIPSInstr<MIPSVersion::v64>;

#endif //MIPS_INSTR_H
