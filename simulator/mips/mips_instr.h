 /* func_instr.h - instruction parser for mips
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2014-2017 MIPT-MIPS
 */

/** Edited by Ladin Oleg. */

#ifndef FUNC_INSTR_H
#define FUNC_INSTR_H

// Generic C++
#include <cassert>
#include <array>
#include <unordered_map>

// MIPT-MIPS modules
#include <infra/types.h>
#include <infra/macro.h>
#include <infra/string/cow_string.h>

enum RegNum : uint8
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

inline uint32 count_zeros(uint32 value)
{
    uint32_t count = 0;
    for ( uint32_t i = 0x80000000; i > 0; i >>= 1)
    {
        if ( ( value & i) != 0)
           break;
        count++;
    }
    return count;
}

template<size_t N, typename T>
T align_up(T value) { return ((value + ((1ull << N) - 1)) >> N) << N; }

class FuncInstr
{
    private:
        enum OperationType : uint8
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
            OUT_RI_BRANCH_0,
            OUT_RI_TRAP,
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
            OUT_RI_JUMP_LINK,
            OUT_J_SPECIAL,
            OUT_SP2_COUNT,
            OUT_UNKNOWN
        } operation = OUT_UNKNOWN;

        enum class TrapType : uint8
        {
            NO_TRAP,
            EXPLICIT_TRAP,
        } trap = TrapType::NO_TRAP;

        const union _instr
        {
            const struct
            {
                uint32 funct  :6;
                uint32 shamt  :5;
                uint32 rd     :5;
                uint32 rt     :5;
                uint32 rs     :5;
                uint32 opcode :6;
            } asR;
            const struct
            {
                uint32 imm    :16;
                uint32 rt     :5;
                uint32 rs     :5;
                uint32 opcode :6;
            } asI;
            const struct
            {
                uint32 imm    :26;
                uint32 opcode :6;
            } asJ;

            const uint32 raw;

            _instr() : raw(NO_VAL32) { };
            explicit _instr(uint32 bytes) : raw( bytes) { }
        } instr;

        using Execute = void (FuncInstr::*)();
        using Predicate = bool (FuncInstr::*)() const;

        struct ISAEntry
        {
            std::string_view name;
            OperationType operation;
            uint8 mem_size;
            FuncInstr::Execute function;
            uint8 mips_version;
        };

        static const std::unordered_map <uint8, FuncInstr::ISAEntry> isaMapR;
        static const std::unordered_map <uint8, FuncInstr::ISAEntry> isaMapRI;
        static const std::unordered_map <uint8, FuncInstr::ISAEntry> isaMapIJ;
        static const std::unordered_map <uint8, FuncInstr::ISAEntry> isaMapMIPS32;

        static std::string_view regTableName(RegNum reg);
        static std::array<std::string_view, REG_NUM_MAX> regTable;

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

        bool complete   = false;
        bool writes_dst = true;

        /* info for branch misprediction unit */
        bool predicted_taken = false;     // Predicted direction
        Addr predicted_target = NO_VAL32; // PC, predicted by BPU
        bool _is_jump_taken = false;      // actual result

        const Addr PC = NO_VAL32;
        Addr new_PC = NO_VAL32;

#if 0
        std::string disasm = {};
#else
        CowString disasm = {};
#endif

        void init( const ISAEntry& entry);

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

        void execute_and()   { v_dst = v_src1 & v_src2; }
        void execute_or()    { v_dst = v_src1 | v_src2; }
        void execute_xor()   { v_dst = v_src1 ^ v_src2; }
        void execute_nor()   { v_dst = ~( v_src1 | v_src2); }

        void execute_andi()  { v_dst = v_src1 & zero_extend(v_imm); }
        void execute_ori()   { v_dst = v_src1 | zero_extend(v_imm); }
        void execute_xori()  { v_dst = v_src1 ^ zero_extend(v_imm); }

        void execute_movn()  { v_dst = v_src1; writes_dst = (v_src2 != 0);}
        void execute_movz()  { v_dst = v_src1; writes_dst = (v_src2 == 0);}

        // Function-templated method is a little-known feature of C++, but useful here
        template<Predicate p>
        void execute_set() { v_dst = static_cast<uint32>((this->*p)()); }

        template<Predicate p>
        void execute_trap() { if ((this->*p)()) trap = TrapType::EXPLICIT_TRAP; }

        template<Predicate p>
        void execute_branch()
        {
            _is_jump_taken = (this->*p)();
            if ( _is_jump_taken)
                new_PC += sign_extend( v_imm) << 2;
        }

        void execute_clo() { v_dst = count_zeros( ~v_src1); }
        void execute_clz() { v_dst = count_zeros(  v_src1); }

        void execute_j()      { _is_jump_taken = true; new_PC = (PC & 0xf0000000) | (v_imm << 2); }
        void execute_jr()     { _is_jump_taken = true; new_PC = align_up<2>(v_src1); }

        void execute_jal()    { _is_jump_taken = true; v_dst = new_PC; new_PC = (PC & 0xF0000000) | (v_imm << 2); };
        void execute_jalr()   { _is_jump_taken = true; v_dst = new_PC; new_PC = align_up<2>(v_src1); };

        template<Predicate p>
        void execute_jlr()
        {
            _is_jump_taken = (this->*p)();
            if ( _is_jump_taken)
            {
                v_dst = new_PC;
                new_PC += sign_extend( v_imm) << 2;
            }
        }

        void execute_syscall(){ };
        void execute_break()  { };

        void execute_unknown();

        void calculate_load_addr()  { mem_addr = v_src1 + sign_extend(v_imm); }
        void calculate_store_addr() { mem_addr = v_src1 + sign_extend(v_imm); }

        Execute function = &FuncInstr::execute_unknown;
    public:
        uint32 hi = NO_VAL32;
        uint32 lo = NO_VAL32;

        FuncInstr() = delete;

        explicit
        FuncInstr( uint32 bytes, Addr PC = 0,
                   bool predicted_taken = false,
                   Addr predicted_target = 0);

        const std::string_view Dump() const { return static_cast<std::string_view>(disasm); }
        bool is_same( const FuncInstr& rhs) const {
            return PC == rhs.PC && instr.raw == rhs.instr.raw;
        }

        RegNum get_src1_num() const { return src1; }
        RegNum get_src2_num() const { return src2; }
        RegNum get_dst_num()  const { return dst;  }

        /* Checks if instruction can change PC in unusual way. */
        bool is_jump() const { return operation == OUT_J_JUMP       ||
                                      operation == OUT_J_JUMP_LINK  ||
                                      operation == OUT_RI_JUMP_LINK ||
                                      operation == OUT_R_JUMP       ||
                                      operation == OUT_R_JUMP_LINK  ||
                                      operation == OUT_I_BRANCH_0   ||
                                      operation == OUT_RI_BRANCH_0  ||
                                      operation == OUT_I_BRANCH;     }
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

        bool get_writes_dst() const { return writes_dst; }

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
