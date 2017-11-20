/*
 * func_instr.h - instruction parser for mips
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2014 MIPT-MIPS
 */

/** Edited by Ladin Oleg. */

#ifndef FUNC_INSTR_H
#define FUNC_INSTR_H

// Generic C++
#include <cassert>
#include <string>
#include <array>
#include <unordered_map>
#if __has_include("string_view")
#include <string_view>
using std::string_view;
#else
#include <experimental/string_view>
using std::experimental::string_view;
#endif

// MIPT-MIPS modules
#include <infra/types.h>
#include <infra/macro.h>

enum RegNum
{
    REG_NUM_ZERO = 0,
    REG_NUM_AT,
    REG_NUM_V0,
    REG_NUM_V1,
    REG_NUM_A0,
    REG_NUM_A1,
    REG_NUM_A2,
    REG_NUM_A3,
    REG_NUM_T0,
    REG_NUM_T1,
    REG_NUM_T2,
    REG_NUM_T3,
    REG_NUM_T4,
    REG_NUM_T5,
    REG_NUM_T6,
    REG_NUM_T7,
    REG_NUM_S0,
    REG_NUM_S1,
    REG_NUM_S2,
    REG_NUM_S3,
    REG_NUM_S4,
    REG_NUM_S5,
    REG_NUM_S6,
    REG_NUM_S7,
    REG_NUM_T8,
    REG_NUM_T9,
    REG_NUM_K0,
    REG_NUM_K1,
    REG_NUM_GP,
    REG_NUM_SP,
    REG_NUM_FP,
    REG_NUM_RA,
    REG_NUM_MAX
};

inline int32 sign_extend(int16 v)  { return static_cast<int32>(v); }
inline int32 zero_extend(uint16 v) { return static_cast<int32>(v); }

template<size_t N, typename T>
T align_up(T value) { return ((value + ((1ull << N) - 1)) >> N) << N; }

class FuncInstr
{
    private:
        enum Format
        {
            FORMAT_R,
            FORMAT_I,
            FORMAT_J,
            FORMAT_UNKNOWN
        } format = FORMAT_UNKNOWN;

        enum OperationType
        {
            OUT_R_ARITHM,
            OUT_R_SHIFT,
            OUT_R_SHAMT,
            OUT_R_JUMP,
            OUT_R_JUMP_LINK,
            OUT_R_SPECIAL,
            OUT_R_TRAP,
            OUT_I_ARITHM,
            OUT_I_BRANCH,
            OUT_I_BRANCH_0,
            OUT_I_LOAD,
            OUT_I_LOADU,
            OUT_I_LOADR,
            OUT_I_LOADL,
            OUT_I_CONST,
            OUT_I_STORE,
            OUT_I_STOREL,
            OUT_I_STORER,
            OUT_J_JUMP,
            OUT_J_JUMP_LINK,
            OUT_J_SPECIAL,
            OUT_UNKNOWN
        } operation = OUT_UNKNOWN;

        enum class TrapType
        {
            NO_TRAP,
            EXPLICIT_TRAP,
        } trap = TrapType::NO_TRAP;

        union _instr
        {
            struct
            {
                unsigned funct  :6;
                unsigned shamt  :5;
                unsigned rd     :5;
                unsigned rt     :5;
                unsigned rs     :5;
                unsigned opcode :6;
            } asR;
            struct
            {
                unsigned imm    :16;
                unsigned rt     :5;
                unsigned rs     :5;
                unsigned opcode :6;
            } asI;
            struct
            {
                unsigned imm    :26;
                unsigned opcode :6;
            } asJ;
            uint32 raw;

            _instr() : raw( NO_VAL32) { } // constructor w/o arguments for ports
            explicit _instr(uint32 bytes) : raw( bytes) { }
        } instr = {};

        using Execute = void (FuncInstr::*)();

        struct ISAEntry // NOLINT
        {
            std::string name;

            uint8 opcode;

            Format format;
            OperationType operation;

            uint8 mem_size;

            FuncInstr::Execute function;

            uint8 mips_version;
        };
        
        static const std::unordered_map <uint8, FuncInstr::ISAEntry> isaMapR;
	    static const std::unordered_map <uint8, FuncInstr::ISAEntry> isaMapRI;
	    static const std::unordered_map <uint8, FuncInstr::ISAEntry> isaMapIJ;
                        
        static string_view regTableName(RegNum reg);
        static std::array<std::string, REG_NUM_MAX> regTable;
        string_view name = {};

        RegNum src1 = REG_NUM_ZERO;
        RegNum src2 = REG_NUM_ZERO;
        RegNum dst = REG_NUM_ZERO;

        uint32 v_imm = NO_VAL32;
        uint32 v_src1 = NO_VAL32;
        uint32 v_src2 = NO_VAL32;
        uint32 v_dst = NO_VAL32;
        uint16 shamt = NO_VAL16;
        Addr mem_addr = NO_VAL32;
        uint32 mem_size = NO_VAL32;

        bool complete = false;

        /* info for branch misprediction unit */
        bool predicted_taken = false;     // Predicted direction
        Addr predicted_target = NO_VAL32; // PC, predicted by BPU
        bool _is_jump_taken = false;      // actual result

        Addr PC = NO_VAL32; // removing "const" keyword to supporting ports
        Addr new_PC = NO_VAL32;

        std::string disasm = "";

        void initFormat();
        void initR();
        void initI();
        void initJ();
        void initUnknown();

        // Predicate helpers - unary
        bool lez() const { return static_cast<int32>( v_src1) <= 0; }
        bool gez() const { return static_cast<int32>( v_src1) >= 0; }
        bool ltz() const { return static_cast<int32>( v_src1) < 0; }
        bool gtz() const { return static_cast<int32>( v_src1) > 0; }

        // Predicate helpers - binary
        bool eq()  const { return v_src1 == v_src2; }
        bool ne()  const { return v_src1 != v_src2; }
        bool geu() const { return v_src1 >= v_src2; }
        bool ltu() const { return v_src1 <  v_src2; }
        bool ge()  const { return static_cast<int32>( v_src1) >= static_cast<int32>( v_src2); }
        bool lt()  const { return static_cast<int32>( v_src1) <  static_cast<int32>( v_src2); }

        // Predicate helpers - immediate
        bool eqi() const { return static_cast<int32>( v_src1) == sign_extend( v_imm); }
        bool nei() const { return static_cast<int32>( v_src1) != sign_extend( v_imm); }
        bool lti() const { return static_cast<int32>( v_src1) <  sign_extend( v_imm); }
        bool gei() const { return static_cast<int32>( v_src1) >= sign_extend( v_imm); }

        // Predicate helpers - immediate unsigned
        bool ltiu() const { return v_src1 <  static_cast<uint32>(sign_extend( v_imm)); }
        bool geiu() const { return v_src1 >= static_cast<uint32>(sign_extend( v_imm)); }

        void execute_add()   { v_dst = static_cast<int32>( v_src1) + static_cast<int32>( v_src2); }
        void execute_sub()   { v_dst = static_cast<int32>( v_src1) - static_cast<int32>( v_src2); }
        void execute_addi()  { v_dst = static_cast<int32>( v_src1) + sign_extend( v_imm); }

        void execute_addu()  { v_dst = v_src1 + v_src2; }
        void execute_subu()  { v_dst = v_src1 - v_src2; }
        void execute_addiu() { v_dst = v_src1 + sign_extend(v_imm); }

        void execute_multu()
        {
             uint64 mult_res = static_cast<uint64>(v_src1) * static_cast<uint64>(v_src2);
             lo = mult_res & 0xFFFFFFFF;
             hi = mult_res >> 0x20;
        }

        void execute_mult()
        {
             uint64 mult_res = static_cast<int64>(v_src1) * static_cast<int64>(v_src2);
             lo = mult_res & 0xFFFFFFFF;
             hi = mult_res >> 0x20;
        }

        void execute_div()   { lo = v_src2 / v_src1; hi = v_src2 % v_src1; };
        void execute_divu()  { lo = v_src2 / v_src1; hi = v_src2 % v_src1; };
        void execute_mfhi()  { v_dst = hi; };
        void execute_mthi()  { hi = v_src2; };
        void execute_mflo()  { v_dst = lo; };
        void execute_mtlo()  { lo = v_src2;};

        void execute_sll()   { v_dst = v_src1 << shamt; }
        void execute_srl()   { v_dst = v_src1 >> shamt; }
        void execute_sra()   { v_dst = static_cast<int32>( v_src1) >> shamt; }
        void execute_sllv()  { v_dst = v_src1 << v_src2; }
        void execute_srlv()  { v_dst = v_src1 >> v_src2; }
        void execute_srav()  { v_dst = static_cast<int32>( v_src1) >> v_src2; }
        void execute_lui()   { v_dst = sign_extend( v_imm) << 0x10; }

        void execute_slt()   { v_dst = static_cast<uint32>( lt()); }
        void execute_sltu()  { v_dst = static_cast<uint32>( ltu()); }
        void execute_slti()  { v_dst = static_cast<uint32>( lti()); }
        void execute_sltiu() { v_dst = static_cast<uint32>( ltiu()); }

        void execute_and()   { v_dst = v_src1 & v_src2; }
        void execute_or()    { v_dst = v_src1 | v_src2; }
        void execute_xor()   { v_dst = v_src1 ^ v_src2; }
        void execute_nor()   { v_dst = ~( v_src1 | v_src2); }

        void execute_andi()  { v_dst = v_src1 & zero_extend(v_imm); }
        void execute_ori()   { v_dst = v_src1 | zero_extend(v_imm); }
        void execute_xori()  { v_dst = v_src1 ^ zero_extend(v_imm); }
    
        void execute_movn()  { execute_unknown(); }
        void execute_movz()  { execute_unknown(); }
    
        void execute_tge()  { if ( ge() ) trap = TrapType::EXPLICIT_TRAP; }
        void execute_tgeu() { if ( geu()) trap = TrapType::EXPLICIT_TRAP; }
        void execute_tlt()  { if ( lt() ) trap = TrapType::EXPLICIT_TRAP; }
        void execute_tltu() { if ( ltu()) trap = TrapType::EXPLICIT_TRAP; }
        void execute_teq()  { if ( eq() ) trap = TrapType::EXPLICIT_TRAP; }
        void execute_tne()  { if ( ne() ) trap = TrapType::EXPLICIT_TRAP; }
    
        void execute_beq()
        {
            _is_jump_taken = eq();
            if ( _is_jump_taken)
                new_PC += sign_extend( v_imm) << 2;
        }

        void execute_bne()
        {
            _is_jump_taken = ne();
            if ( _is_jump_taken)
                new_PC += sign_extend( v_imm) << 2;
        }

        void execute_blez()
        {
            _is_jump_taken = lez();
            if ( _is_jump_taken)
                new_PC += sign_extend( v_imm) << 2;
        }

        void execute_bgtz()
        {
            _is_jump_taken = gtz();
            if ( _is_jump_taken)
                new_PC += sign_extend( v_imm) << 2;
        }

        void execute_j()      { _is_jump_taken = true; new_PC = (PC & 0xf0000000) | (v_imm << 2); }
        void execute_jr()     { _is_jump_taken = true; new_PC = align_up<2>(v_src1); }

        void execute_jal()    { _is_jump_taken = true; v_dst = new_PC; new_PC = (PC & 0xF0000000) | (v_imm << 2); };
        void execute_jalr()   { _is_jump_taken = true; v_dst = new_PC; new_PC = align_up<2>(v_src1); };

        void execute_syscall(){ };
        void execute_break()  { };

        void execute_unknown();

        void calculate_load_addr()  { mem_addr = v_src1 + sign_extend(v_imm); }
        void calculate_store_addr() { mem_addr = v_src1 + sign_extend(v_imm); }

        Execute function = &FuncInstr::execute_unknown;
    public:
        uint32 hi = NO_VAL32;
        uint32 lo = NO_VAL32;

        FuncInstr() = default; // constructor w/o arguments for ports

        explicit
        FuncInstr( uint32 bytes, Addr PC = 0,
                   bool predicted_taken = false,
                   Addr predicted_target = 0);

        const std::string& Dump() const { return disasm; }

        RegNum get_src1_num() const { return src1; }
        RegNum get_src2_num() const { return src2; }
        RegNum get_dst_num()  const { return dst;  }

        /* Checks if instruction can change PC in unusual way. */
        bool isJump() const { return operation == OUT_J_JUMP      ||
                                     operation == OUT_J_JUMP_LINK ||
                                     operation == OUT_R_JUMP      ||
                                     operation == OUT_R_JUMP_LINK ||
                                     operation == OUT_I_BRANCH_0  ||
                                     operation == OUT_I_BRANCH; }
        bool is_jump_taken() const { return  _is_jump_taken; }
        bool is_misprediction() const { return predicted_taken != is_jump_taken() || predicted_target != new_PC; }
        bool is_load()  const { return operation == OUT_I_LOAD  ||
                                       operation == OUT_I_LOADU ||
                                       operation == OUT_I_LOADR ||
                                       operation == OUT_I_LOADL; }
        bool is_store() const { return operation == OUT_I_STORE  ||
                                       operation == OUT_I_STORER ||
                                       operation == OUT_I_STOREL; }
        bool is_nop() const { return instr.raw == 0x0u; }

        bool has_trap() const { return trap != TrapType::NO_TRAP; }

        void set_v_src1(uint32 value) { v_src1 = value; }
        void set_v_src2(uint32 value) { v_src2 = value; }

        uint32 get_v_dst() const { return v_dst; }

        Addr get_mem_addr() const { return mem_addr; }
        uint32 get_mem_size() const { return mem_size; }
        Addr get_new_PC() const { return new_PC; }
        Addr get_PC() const { return PC; }

        void set_v_dst(uint32 value); // for loads
        uint32 get_v_src2() const { return v_src2; } // for stores

        void execute();
        void check_trap();
};

static inline std::ostream& operator<<( std::ostream& out, const FuncInstr& instr)
{
    return out << instr.Dump();
}

#endif //FUNC_INSTR_H
