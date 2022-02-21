/**
 * mips_instr.cpp - instruction decoder for mips
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#include "mips_instr.h"
#include "mips_instr_decode.h"

#include <func_sim/alu/alu.h>
#include <infra/instrcache/instr_cache.h>
#include <infra/macro.h>
#include <infra/types.h>

#include <iomanip>
#include <iostream>
#include <unordered_map>
#include <vector>

template<Executable I> void do_nothing(I* /* instr */) { }

template<Executable I>
struct MIPSTableEntry
{
	using Execute = void (*)(I*);

    std::string_view name = "Unknown instruction";
    Execute function = ALU::unknown_instruction<I>;
    OperationType operation = OUT_ARITHM;
    uint8 mem_size = 0;
    char imm_type = 'N';
    Imm imm_print_type = Imm::NO;
    std::vector<Src> src = { };
    std::vector<Dst> dst = { Dst::ZERO };
    MIPSVersionMask versions = MIPS_I_Instr;
};

template<Executable I>
using Table = std::unordered_map<uint32, MIPSTableEntry<I>>;

//unordered map for R-instructions
template<Executable I>
static const Table<I> isaMapR =
{
    // **************** R INSTRUCTIONS ****************
    // Constant shifts
    {0x0, { "sll",  Shifter::sll<I, uint32>,  OUT_ARITHM, 0, 'S', Imm::SHIFT, { Src::RT }, { Dst::RD }, MIPS_I_Instr} },
//  {0x1, { "movci"
    {0x2, { "srl",  Shifter::srl<I, uint32>,  OUT_ARITHM, 0, 'S', Imm::SHIFT, { Src::RT }, { Dst::RD }, MIPS_I_Instr} },
    {0x3, { "sra",  Shifter::sra<I, uint32>,  OUT_ARITHM, 0, 'S', Imm::SHIFT, { Src::RT }, { Dst::RD }, MIPS_I_Instr} },
    // Variable shifts
    {0x4, { "sllv", Shifter::sllv<I, uint32>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::RT, Src::RS }, { Dst::RD }, MIPS_I_Instr} },
//  {0x5
    {0x6, { "srlv", Shifter::srlv<I, uint32>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::RT, Src::RS }, { Dst::RD }, MIPS_I_Instr} },
    {0x7, { "srav", Shifter::srav<I, uint32>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::RT, Src::RS }, { Dst::RD }, MIPS_I_Instr} },
    // Indirect branches
    {0x8, { "jr",   ALU::jr<I>,                     OUT_R_JUMP, 0, 'N', Imm::NO, { Src::RS }, { Dst::ZERO }, MIPS_I_Instr} },
    {0x9, { "jalr", ALU::jump_and_link<I, ALU::jr>, OUT_R_JUMP, 0, 'N', Imm::NO, { Src::RS }, { Dst::RD },   MIPS_I_Instr} },
    // Conditional moves (MIPS IV)
    {0xA, { "movz", ALU::movz<I>, OUT_R_CONDM, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::RD }, MIPS_IV_Instr | MIPS_32_Instr} },
    {0xB, { "movn", ALU::movn<I>, OUT_R_CONDM, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::RD }, MIPS_IV_Instr | MIPS_32_Instr} },
    // System calls
    {0xC, { "syscall", ALU::syscall<I>,    OUT_SYSCALL, 0, 'N', Imm::NO, { }, { Dst::ZERO }, MIPS_I_Instr} },
    {0xD, { "break",   ALU::breakpoint<I>, OUT_BREAK,   0, 'N', Imm::NO, { }, { Dst::ZERO }, MIPS_I_Instr} },
    // Hi/Lo manipulators
    {0x10, { "mfhi", ALU::move<I>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::HI }, { Dst::RD }, MIPS_I_Instr} },
    {0x11, { "mthi", ALU::move<I>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS }, { Dst::HI }, MIPS_I_Instr} },
    {0x12, { "mflo", ALU::move<I>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::LO }, { Dst::RD }, MIPS_I_Instr} },
    {0x13, { "mtlo", ALU::move<I>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS }, { Dst::LO }, MIPS_I_Instr} },
    // Doubleword variable shifts
    {0x14, { "dsllv", Shifter::sllv<I, uint64>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::RT, Src::RS }, { Dst::RD }, MIPS_III_Instr} },
    {0x16, { "dsrlv", Shifter::srlv<I, uint64>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::RT, Src::RS }, { Dst::RD }, MIPS_III_Instr} },
    {0x17, { "dsrav", Shifter::srav<I, uint64>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::RT, Src::RS }, { Dst::RD }, MIPS_III_Instr} },
    // Multiplication/Division
    {0x18, { "mult",   MIPSMultALU::multiplication<int32, I>,  OUT_DIVMULT, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::LO, Dst::HI }, MIPS_I_Instr} },
    {0x19, { "multu",  MIPSMultALU::multiplication<uint32, I>, OUT_DIVMULT, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::LO, Dst::HI }, MIPS_I_Instr} },
    {0x1A, { "div",    MIPSMultALU::division<int32, I>,        OUT_DIVMULT, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::LO, Dst::HI }, MIPS_I_Instr} },
    {0x1B, { "divu",   MIPSMultALU::division<uint32, I>,       OUT_DIVMULT, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::LO, Dst::HI }, MIPS_I_Instr} },
    {0x1C, { "dmult",  MIPSMultALU::multiplication<int64, I>,  OUT_DIVMULT, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::LO, Dst::HI }, MIPS_III_Instr} },
    {0x1D, { "dmultu", MIPSMultALU::multiplication<uint64, I>, OUT_DIVMULT, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::LO, Dst::HI }, MIPS_III_Instr} },
    {0x1E, { "ddiv",   MIPSMultALU::division<int64, I>,        OUT_DIVMULT, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::LO, Dst::HI }, MIPS_III_Instr} },
    {0x1F, { "ddivu",  MIPSMultALU::division<uint64, I>,       OUT_DIVMULT, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::LO, Dst::HI }, MIPS_III_Instr} },
    // Addition/Subtraction
    {0x20, { "add",  ALU::addition_overflow<I, uint32>,    OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::RD }, MIPS_I_Instr} },
    {0x21, { "addu", ALU::addition<I, uint32>,             OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::RD }, MIPS_I_Instr} },
    {0x22, { "sub",  ALU::subtraction_overflow<I, uint32>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::RD }, MIPS_I_Instr} },
    {0x23, { "subu", ALU::subtraction<I, uint32>,          OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::RD }, MIPS_I_Instr} },
    // Logical operations
    {0x24, { "and", ALU::andv<I>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::RD }, MIPS_I_Instr} },
    {0x25, { "or",  ALU::orv<I>,  OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::RD }, MIPS_I_Instr} },
    {0x26, { "xor", ALU::xorv<I>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::RD }, MIPS_I_Instr} },
    {0x27, { "nor", ALU::nor<I>,  OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::RD }, MIPS_I_Instr} },
//  {0x28
//  {0x29
    {0x2A, { "slt",  ALU::set<I, &I::lt>,  OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::RD }, MIPS_I_Instr} },
    {0x2B, { "sltu", ALU::set<I, &I::ltu>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::RD }, MIPS_I_Instr} },
    // Doubleword addition/Subtraction
    {0x2C, { "dadd",  ALU::addition_overflow<I, uint64>,    OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::RD }, MIPS_III_Instr} },
    {0x2D, { "daddu", ALU::addition<I, uint64>,             OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::RD }, MIPS_III_Instr} },
    {0x2E, { "dsub",  ALU::subtraction_overflow<I, uint64>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::RD }, MIPS_III_Instr} },
    {0x2F, { "dsubu", ALU::subtraction<I, uint64>,          OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::RD }, MIPS_III_Instr} },
    // Conditional traps (MIPS II)
    {0x30, { "tge",  ALU::trap<I, &I::ge>,  OUT_TRAP, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_II_Instr} },
    {0x31, { "tgeu", ALU::trap<I, &I::geu>, OUT_TRAP, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_II_Instr} },
    {0x32, { "tlt",  ALU::trap<I, &I::lt>,  OUT_TRAP, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_II_Instr} },
    {0x33, { "tltu", ALU::trap<I, &I::ltu>, OUT_TRAP, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_II_Instr} },
    {0x34, { "teq",  ALU::trap<I, &I::eq>,  OUT_TRAP, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_II_Instr} },
//  {0x35
    {0x36, { "tne",  ALU::trap<I, &I::ne>,  OUT_TRAP, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_II_Instr} },
//  {0x37
    // Doubleword shifts
    {0x38, { "dsll",   Shifter::sll<I, uint64>, OUT_ARITHM, 0, 'S', Imm::SHIFT, { Src::RT }, { Dst::RD }, MIPS_III_Instr} },
    {0x3A, { "dsrl",   Shifter::srl<I, uint64>, OUT_ARITHM, 0, 'S', Imm::SHIFT, { Src::RT }, { Dst::RD }, MIPS_III_Instr} },
    {0x3B, { "dsra",   Shifter::sra<I, uint64>, OUT_ARITHM, 0, 'S', Imm::SHIFT, { Src::RT }, { Dst::RD }, MIPS_III_Instr} },
    {0x3C, { "dsll32", Shifter::dsll32<I>, OUT_ARITHM, 0, 'S', Imm::SHIFT, { Src::RT }, { Dst::RD }, MIPS_III_Instr} },
    {0x3E, { "dsrl32", Shifter::dsrl32<I>, OUT_ARITHM, 0, 'S', Imm::SHIFT, { Src::RT }, { Dst::RD }, MIPS_III_Instr} },
    {0x3F, { "dsra32", Shifter::dsra32<I>, OUT_ARITHM, 0, 'S', Imm::SHIFT, { Src::RT }, { Dst::RD }, MIPS_III_Instr} }
};

//unordered map for RI-instructions
template<Executable I>
static const Table<I> isaMapRI =
{
    // Branches
    {0x0,  { "bltz",  ALU::branch<I, &I::ltz>, OUT_BRANCH,        0, 'I', Imm::ARITH, { Src::RS }, { Dst::ZERO }, MIPS_I_Instr} },
    {0x1,  { "bgez",  ALU::branch<I, &I::gez>, OUT_BRANCH,        0, 'I', Imm::ARITH, { Src::RS }, { Dst::ZERO }, MIPS_I_Instr} },
    {0x2,  { "bltzl", ALU::branch<I, &I::ltz>, OUT_BRANCH_LIKELY, 0, 'I', Imm::ARITH, { Src::RS }, { Dst::ZERO }, MIPS_II_Instr} },
    {0x3,  { "bgezl", ALU::branch<I, &I::gez>, OUT_BRANCH_LIKELY, 0, 'I', Imm::ARITH, { Src::RS }, { Dst::ZERO }, MIPS_II_Instr} },
    // Conditional traps
    {0x8,  { "tgei",  ALU::trap<I, &I::gei>,  OUT_TRAP, 0, 'I', Imm::TRAP, { Src::RS }, { Dst::ZERO }, MIPS_II_Instr} },
    {0x9,  { "tgeiu", ALU::trap<I, &I::geiu>, OUT_TRAP, 0, 'I', Imm::TRAP, { Src::RS }, { Dst::ZERO }, MIPS_II_Instr} },
    {0xA,  { "tlti",  ALU::trap<I, &I::lti>,  OUT_TRAP, 0, 'I', Imm::TRAP, { Src::RS }, { Dst::ZERO }, MIPS_II_Instr} },
    {0xB,  { "tltiu", ALU::trap<I, &I::ltiu>, OUT_TRAP, 0, 'I', Imm::TRAP, { Src::RS }, { Dst::ZERO }, MIPS_II_Instr} },
    {0xC,  { "teqi",  ALU::trap<I, &I::eqi>,  OUT_TRAP, 0, 'I', Imm::TRAP, { Src::RS }, { Dst::ZERO }, MIPS_II_Instr} },
    {0xE,  { "tnei",  ALU::trap<I, &I::nei>,  OUT_TRAP, 0, 'I', Imm::TRAP, { Src::RS }, { Dst::ZERO }, MIPS_II_Instr} },
    // Linking branches
    {0x10, { "bltzal",  ALU::branch_and_link<I, &I::ltz>, OUT_BRANCH,        0, 'I', Imm::ARITH, { Src::RS }, { Dst::RA }, MIPS_I_Instr} },
    {0x11, { "bgezal",  ALU::branch_and_link<I, &I::gez>, OUT_BRANCH,        0, 'I', Imm::ARITH, { Src::RS }, { Dst::RA }, MIPS_I_Instr} },
    {0x12, { "bltzall", ALU::branch_and_link<I, &I::ltz>, OUT_BRANCH_LIKELY, 0, 'I', Imm::ARITH, { Src::RS }, { Dst::RA }, MIPS_II_Instr} },
    {0x13, { "bgezall", ALU::branch_and_link<I, &I::gez>, OUT_BRANCH_LIKELY, 0, 'I', Imm::ARITH, { Src::RS }, { Dst::RA }, MIPS_II_Instr} }
};

//unordered map for I-instructions and J-instructions
template<Executable I>
static const Table<I> isaMapIJ =
{
    // Direct jumps
    {0x2, { "j",   ALU::j<I>,                     OUT_J_JUMP, 0, 'J', Imm::JUMP, { }, { Dst::ZERO }, MIPS_I_Instr } },
    {0x3, { "jal", ALU::jump_and_link<I, ALU::j>, OUT_J_JUMP, 0, 'J', Imm::JUMP, { }, { Dst::RA },   MIPS_I_Instr } },
    // Branches
    {0x4, { "beq",  ALU::branch<I, &I::eq>,  OUT_BRANCH, 0, 'I', Imm::ARITH, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_I_Instr} },
    {0x5, { "bne",  ALU::branch<I, &I::ne>,  OUT_BRANCH, 0, 'I', Imm::ARITH, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_I_Instr} },
    {0x6, { "blez", ALU::branch<I, &I::lez>, OUT_BRANCH, 0, 'I', Imm::ARITH, { Src::RS },          { Dst::ZERO }, MIPS_I_Instr} },
    {0x7, { "bgtz", ALU::branch<I, &I::gtz>, OUT_BRANCH, 0, 'I', Imm::ARITH, { Src::RS },          { Dst::ZERO }, MIPS_I_Instr} },
    // Addition/Subtraction
    {0x8, { "addi",  ALU::addition_overflow_imm<I, uint32>,  OUT_ARITHM, 0, 'I', Imm::ARITH, { Src::RS }, { Dst::RT }, MIPS_I_Instr} },
    {0x9, { "addiu", ALU::addition_imm<I, uint32>,           OUT_ARITHM, 0, 'I', Imm::ARITH, { Src::RS }, { Dst::RT }, MIPS_I_Instr} },
    // Logical operations
    {0xA, { "slti",  ALU::set<I, &I::lti>,  OUT_ARITHM, 0, 'I', Imm::ARITH, { Src::RS }, { Dst::RT }, MIPS_I_Instr} },
    {0xB, { "sltiu", ALU::set<I, &I::ltiu>, OUT_ARITHM, 0, 'I', Imm::ARITH, { Src::RS }, { Dst::RT }, MIPS_I_Instr} },
    {0xC, { "andi",  ALU::andi<I>,          OUT_ARITHM, 0, 'L', Imm::LOGIC, { Src::RS }, { Dst::RT }, MIPS_I_Instr} },
    {0xD, { "ori",   ALU::ori<I>,           OUT_ARITHM, 0, 'L', Imm::LOGIC, { Src::RS }, { Dst::RT }, MIPS_I_Instr} },
    {0xE, { "xori",  ALU::xori<I>,          OUT_ARITHM, 0, 'L', Imm::LOGIC, { Src::RS }, { Dst::RT }, MIPS_I_Instr} },
    {0xF, { "lui",   ALU::upper_immediate<16, I>,   OUT_ARITHM, 0, 'I', Imm::LOGIC, { }, { Dst::RT }, MIPS_I_Instr} },
    // 0x10 - 0x13 coprocessor operations
    // Likely branches (MIPS II)
    {0x14, { "beql",  ALU::branch<I, &I::eq>,  OUT_BRANCH_LIKELY, 0, 'I', Imm::ARITH, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_II_Instr} },
    {0x15, { "bnel",  ALU::branch<I, &I::ne>,  OUT_BRANCH_LIKELY, 0, 'I', Imm::ARITH, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_II_Instr} },
    {0x16, { "blezl", ALU::branch<I, &I::lez>, OUT_BRANCH_LIKELY, 0, 'I', Imm::ARITH, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_II_Instr} },
    {0x17, { "bgtzl", ALU::branch<I, &I::gtz>, OUT_BRANCH_LIKELY, 0, 'I', Imm::ARITH, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_II_Instr} },
    // Doubleword unaligned loads
    {0x1A, { "ldl", ALU::load_addr<I>,  OUT_LOAD, 8, 'I', Imm::ADDR, { Src::RS }, { Dst::RT }, MIPS_III_Instr} },
    {0x1B, { "ldr", ALU::load_addr<I>,  OUT_LOAD, 8, 'I', Imm::ADDR, { Src::RS }, { Dst::RT }, MIPS_III_Instr} },
    // Doubleword addition
    {0x18, { "daddi",  ALU::addition_overflow_imm<I, uint64>, OUT_ARITHM, 0, 'I', Imm::ARITH, { Src::RS }, { Dst::RT }, MIPS_III_Instr} },
    {0x19, { "daddiu", ALU::addition_imm<I, uint64>,          OUT_ARITHM, 0, 'I', Imm::ARITH, { Src::RS }, { Dst::RT }, MIPS_III_Instr} },
    // Loads
    {0x20, { "lb",  ALU::load_addr<I>,         OUT_LOAD, 1, 'I',         Imm::ADDR, { Src::RS }, { Dst::RT }, MIPS_I_Instr} },
    {0x21, { "lh",  ALU::load_addr_aligned<I>, OUT_LOAD, 2, 'I',         Imm::ADDR, { Src::RS }, { Dst::RT }, MIPS_I_Instr} },
    {0x22, { "lwl", ALU::load_addr_left32<I>,  OUT_PARTIAL_LOAD, 4, 'I', Imm::ADDR, { Src::RS }, { Dst::RT }, MIPS_I_Instr} },
    {0x23, { "lw",  ALU::load_addr_aligned<I>, OUT_LOAD, 4, 'I',         Imm::ADDR, { Src::RS }, { Dst::RT }, MIPS_I_Instr} },
    {0x24, { "lbu", ALU::load_addr<I>,         OUT_LOADU, 1, 'I',        Imm::ADDR, { Src::RS }, { Dst::RT }, MIPS_I_Instr} },
    {0x25, { "lhu", ALU::load_addr_aligned<I>, OUT_LOADU, 2, 'I',        Imm::ADDR, { Src::RS }, { Dst::RT }, MIPS_I_Instr} },
    {0x26, { "lwr", ALU::load_addr_right32<I>, OUT_PARTIAL_LOAD, 4, 'I', Imm::ADDR, { Src::RS }, { Dst::RT }, MIPS_I_Instr} },
    {0x27, { "lwu", ALU::load_addr_aligned<I>, OUT_LOADU, 4, 'I',        Imm::ADDR, { Src::RS }, { Dst::RT }, MIPS_I_Instr} },
    // Stores
    {0x28, { "sb",  ALU::store_addr<I>,         OUT_STORE, 1, 'I', Imm::ADDR, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_I_Instr} },
    {0x29, { "sh",  ALU::store_addr_aligned<I>, OUT_STORE, 2, 'I', Imm::ADDR, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_I_Instr} },
    {0x2A, { "swl", ALU::store_addr_left32<I>,  OUT_STORE, 4, 'I', Imm::ADDR, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_I_Instr} },
    {0x2B, { "sw",  ALU::store_addr_aligned<I>, OUT_STORE, 4, 'I', Imm::ADDR, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_I_Instr} },
    {0x2C, { "sdl", ALU::store_addr<I>,         OUT_STORE, 8, 'I', Imm::ADDR, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_III_Instr} },
    {0x2D, { "sdr", ALU::store_addr<I>,         OUT_STORE, 8, 'I', Imm::ADDR, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_III_Instr} },
    {0x2E, { "swr", ALU::store_addr_right32<I>, OUT_STORE, 4, 'I', Imm::ADDR, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_I_Instr} },
//  {0x2F, { "cache"
    // Advanced loads
    {0x30, { "ll",   ALU::load_addr_aligned<I>,   OUT_LOAD,  2, 'I', Imm::ADDR, { Src::RS },          { Dst::RT },   MIPS_I_Instr} },
    {0x31, { "lwc1", ALU::unknown_instruction<I>, OUT_LOAD,  4, 'I', Imm::ADDR, { Src::RS },          { Dst::FT },   MIPS_I_Instr} },
    {0x35, { "ldc1", ALU::unknown_instruction<I>, OUT_LOAD,  8, 'I', Imm::ADDR, { Src::RS },          { Dst::FT },   MIPS_II_Instr} },
    {0x37, { "ld",   ALU::load_addr_aligned<I>,   OUT_LOAD,  8, 'I', Imm::ADDR, { Src::RS },          { Dst::RT },   MIPS_III_Instr} },
    // Advanced stores
    {0x38, { "sc",   ALU::store_addr_aligned<I>,  OUT_STORE, 2, 'I', Imm::ADDR, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_I_Instr} },
    {0x39, { "swc1", ALU::unknown_instruction<I>, OUT_STORE, 4, 'I', Imm::ADDR, { Src::RS },          { Dst::FT },   MIPS_I_Instr} },
    {0x3D, { "sdc1", ALU::unknown_instruction<I>, OUT_STORE, 8, 'I', Imm::ADDR, { Src::RS },          { Dst::FT },   MIPS_II_Instr} },
    {0x3F, { "sd",   ALU::store_addr_aligned<I>,  OUT_STORE, 8, 'I', Imm::ADDR, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_III_Instr} },
};

template<Executable I>
static const Table<I> isaMapMIPS32 =
{
    // Advanced multiplication
    {0x00, { "madd",  MIPSMultALU::multiplication<int32, I>,  OUT_R_ACCUM, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::LO, Dst::HI }, MIPS_32_Instr} },
    {0x01, { "maddu", MIPSMultALU::multiplication<uint32, I>, OUT_R_ACCUM, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::LO, Dst::HI }, MIPS_32_Instr} },
    {0x02, { "mul",   MIPSMultALU::multiplication<int32, I>,  OUT_ARITHM,  0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::RD },          MIPS_32_Instr} },
    {0x04, { "msub",  MIPSMultALU::multiplication<int32, I>,  OUT_R_SUBTR, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::LO, Dst::HI }, MIPS_32_Instr} },
    {0x05, { "msubu", MIPSMultALU::multiplication<uint32, I>, OUT_R_SUBTR, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::LO, Dst::HI }, MIPS_32_Instr} },
    // Count leading zeroes/ones
    {0x20, { "clz",  ALU::clz<I, uint32>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS }, { Dst::RD }, MIPS_32_Instr} },
    {0x21, { "clo",  ALU::clo<I, uint32>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS }, { Dst::RD }, MIPS_32_Instr} },
    {0x24, { "dclz", ALU::clz<I, uint64>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS }, { Dst::RD }, MIPS_64_Instr} },
    {0x25, { "dclo", ALU::clo<I, uint64>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS }, { Dst::RD }, MIPS_64_Instr} }
};

template<Executable I>
static const Table<I> isaMapCOP0_rs =
{
    {0x00, { "mfc0",  ALU::move<I>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::CP0_RD }, { Dst::RT },     MIPS_I_Instr} },
    {0x04, { "mtc0",  ALU::move<I>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::RT },     { Dst::CP0_RD }, MIPS_I_Instr} },
};

template<Executable I>
static const Table<I> isaMapCOP0_funct =
{
    {0x18, { "eret",  ALU::eret<I>, OUT_R_JUMP, 0, 'N', Imm::NO, { Src::EPC, Src::SR }, { Dst::SR }, MIPS_I_Instr} },
};

template<Executable I>
static const Table<I> isaMapCOP1 =
{
    // Moves from Floating Point
    {0x00, { "mfc1",  ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::RT }, MIPS_I_Instr} },
    {0x01, { "dmfc1", ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::RT }, MIPS_III_Instr} },
    {0x02, { "cfc1",  ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::RT }, MIPS_I_Instr} },
    // 0x03
    // Moves to Floating Point
    {0x04, { "mtc1",  ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::RT }, { Dst::FS }, MIPS_I_Instr} },
    {0x05, { "dmtc1", ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::RT }, { Dst::FS }, MIPS_III_Instr} },
    {0x06, { "ctc1",  ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::RT }, { Dst::FS }, MIPS_III_Instr} },
};

template<Executable I>
static const Table<I> isaMapCOP1_s =
{
    // Formatted basic instructions
    {0x00, { "add.s",     ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FD }, MIPS_I_Instr} },
    {0x01, { "sub.s",     ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FD }, MIPS_I_Instr} },
    {0x02, { "mul.s",     ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FD }, MIPS_I_Instr} },
    {0x03, { "div.s",     ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FD }, MIPS_I_Instr} },
    // FPU specific instructions
    {0x04, { "sqrt.s",    ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_II_Instr} },
    {0x05, { "abs.s",     ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_I_Instr} },
    {0x06, { "mov.s",     ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_I_Instr} },
    {0x07, { "neg.s",     ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_I_Instr} },
    {0x08, { "round.l.s", ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_III_Instr} },
    {0x09, { "trunc.l.s", ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_III_Instr} },
    {0x0A, { "ceil.l.s",  ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_III_Instr} },
    {0x0B, { "floor.l.s", ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_III_Instr} },
    {0x0C, { "round.w.s", ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_II_Instr} },
    {0x0D, { "trunc.w.s", ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_II_Instr} },
    {0x0E, { "ceil.w.s",  ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_II_Instr} },
    {0x0F, { "floor.w.s", ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_II_Instr} },
    // 0x10 - 0x11
    {0x12, { "movz.s",    ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::RT }, { Dst::FD }, MIPS_IV_Instr} },
    {0x13, { "movn.s",    ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::RT }, { Dst::FD }, MIPS_IV_Instr} },
    // 0x14
    {0x15, { "recip.s",   ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_IV_Instr} },
    {0x16, { "rsqrt.s",   ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_IV_Instr} },
    // Converts
    {0x21, { "cvt.d.s",   ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_I_Instr} },
    // 0x22 - 0x23
    {0x24, { "cvt.w.s",   ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_I_Instr} },
    {0x25, { "cvt.l.s",   ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_III_Instr} },
    // 0x26 - 0x2F
    // Conditions
    {0x30, { "c.f.s",     ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x31, { "c.un.s",    ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x32, { "c.eq.s",    ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x33, { "c.ueq.s",   ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x34, { "c.olt.s",   ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x35, { "c.ult.s",   ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x36, { "c.ole.s",   ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x37, { "c.ule.s",   ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x38, { "c.sf.s",    ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x39, { "c.ngle.s",  ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x3A, { "c.seq.s",   ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x3B, { "c.ngl.s",   ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x3C, { "c.lt.s",    ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x3D, { "c.nge.s",   ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x3E, { "c.le.s",    ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x3F, { "c.ngt.s",   ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
};

template<Executable I>
static const Table<I> isaMapCOP1_d =
{
    // Formatted basic instructions
    {0x00, { "add.d",     ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FD }, MIPS_I_Instr} },
    {0x01, { "sub.d",     ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FD }, MIPS_I_Instr} },
    {0x02, { "mul.d",     ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FD }, MIPS_I_Instr} },
    {0x03, { "div.d",     ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FD }, MIPS_I_Instr} },
    // FPU specific instructions
    {0x04, { "sqrt.d",    ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_II_Instr} },
    {0x05, { "abs.d",     ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_I_Instr} },
    {0x06, { "mov.d",     ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_I_Instr} },
    {0x07, { "neg.d",     ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_I_Instr} },
    {0x08, { "round.l.d", ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_III_Instr} },
    {0x09, { "trunc.l.d", ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_III_Instr} },
    {0x0A, { "ceil.l.d",  ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_III_Instr} },
    {0x0B, { "floor.l.d", ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_III_Instr} },
    {0x0C, { "round.w.d", ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_II_Instr} },
    {0x0D, { "trunc.w.d", ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_II_Instr} },
    {0x0E, { "ceil.w.d",  ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_II_Instr} },
    {0x0F, { "floor.w.d", ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_II_Instr} },
    // 0x10 - 0x11
    {0x12, { "movz.d",    ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::RT }, { Dst::FD }, MIPS_IV_Instr} },
    {0x13, { "movn.d",    ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::RT }, { Dst::FD }, MIPS_IV_Instr} },
    // 0x14
    {0x15, { "recip.d",   ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_IV_Instr} },
    {0x16, { "rsqrt.d",   ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_IV_Instr} },
    // 0x17 - 0x20
    // Converts
    {0x20, { "cvt.s.d",   ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_I_Instr} },
    // 0x21 - 0x23
    {0x24, { "cvt.w.d",   ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_I_Instr} },
    {0x25, { "cvt.l.d",   ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_III_Instr} },
    // 0x26 - 0x2F
    // Conditions
    {0x30, { "c.f.d",     ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x31, { "c.un.d",    ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x32, { "c.eq.d",    ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x33, { "c.ueq.d",   ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x34, { "c.olt.d",   ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x35, { "c.ult.d",   ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x36, { "c.ole.d",   ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x37, { "c.ule.d",   ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x38, { "c.sf.d",    ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x39, { "c.ngle.d",  ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x3A, { "c.seq.d",   ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x3B, { "c.ngl.d",   ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x3C, { "c.lt.d",    ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x3D, { "c.nge.d",   ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x3E, { "c.le.d",    ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x3F, { "c.ngt.d",   ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
};

template<Executable I>
static const Table<I> isaMapCOP1_l =
{
    // Converts
    {0x20, { "cvt.s.l", ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_III_Instr} },
    {0x21, { "cvt.d.l", ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_III_Instr} },
};

template<Executable I>
static const Table<I> isaMapCOP1_w =
{
    // Converts
    {0x20, { "cvt.s.w", ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_I_Instr} },
    {0x21, { "cvt.d.w", ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_I_Instr} },
};

template<Executable I>
static const Table<I> isaMapCOP1I =
{
    // Branches
    {0x0, { "bc1f",  ALU::unknown_instruction<I>, OUT_BRANCH, 0, 'I', Imm::ARITH, { Src::FCSR }, { Dst::ZERO }, MIPS_I_Instr} },
    {0x1, { "bc1t",  ALU::unknown_instruction<I>, OUT_BRANCH, 0, 'I', Imm::ARITH, { Src::FCSR }, { Dst::ZERO }, MIPS_I_Instr} },
    {0x2, { "bc1fl", ALU::unknown_instruction<I>, OUT_BRANCH, 0, 'I', Imm::ARITH, { Src::FCSR }, { Dst::ZERO }, MIPS_I_Instr} },
    {0x3, { "bc1tl", ALU::unknown_instruction<I>, OUT_BRANCH, 0, 'I', Imm::ARITH, { Src::FCSR }, { Dst::ZERO }, MIPS_I_Instr} },
};

template<Executable I>
static const Table<I> isaMapCOP1X =
{
    // Loads
    {0x0, { "lwxc1", ALU::unknown_instruction<I>,  OUT_LOAD,  4, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::FD }, MIPS_IV_Instr} },
    {0x1, { "ldxc1", ALU::unknown_instruction<I>,  OUT_LOAD,  8, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::FD }, MIPS_IV_Instr} },
    // 0x2 - 0x7
    // Stores
    {0x8, { "swxc1", ALU::unknown_instruction<I>,  OUT_STORE, 4, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::FD }, MIPS_IV_Instr} },
    {0x9, { "sdxc1", ALU::unknown_instruction<I>,  OUT_STORE, 8, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::FD }, MIPS_IV_Instr} },
    // 0xa - 0xe
    // 0xf PREFX
    // Advanced multiplication
    {0x20, { "madd.s", ALU::unknown_instruction<I>,  OUT_FPU, 0, 'N', Imm::NO, { Src::FR, Src::FS, Src::FT }, { Dst::FD }, MIPS_IV_Instr} },
    {0x21, { "madd.d", ALU::unknown_instruction<I>,  OUT_FPU, 0, 'N', Imm::NO, { Src::FR, Src::FS, Src::FT }, { Dst::FD }, MIPS_IV_Instr} },
    // 0x22 - 0x27
    {0x28, { "msub.s", ALU::unknown_instruction<I>,  OUT_FPU, 0, 'N', Imm::NO, { Src::FR, Src::FS, Src::FT }, { Dst::FD }, MIPS_IV_Instr} },
    {0x29, { "msub.d", ALU::unknown_instruction<I>,  OUT_FPU, 0, 'N', Imm::NO, { Src::FR, Src::FS, Src::FT }, { Dst::FD }, MIPS_IV_Instr} },
    // 0x2A - 0x2F
    {0x30, { "nmadd.s", ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FR, Src::FS, Src::FT }, { Dst::FD }, MIPS_IV_Instr} },
    {0x31, { "nmadd.d", ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FR, Src::FS, Src::FT }, { Dst::FD }, MIPS_IV_Instr} },
    // 0x32 - 0x37
    {0x38, { "nmsub.s", ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FR, Src::FS, Src::FT }, { Dst::FD }, MIPS_IV_Instr} },
    {0x39, { "nmsub.d", ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FR, Src::FS, Src::FT }, { Dst::FD }, MIPS_IV_Instr} },
};

template<Executable I>
static const Table<I> isaMapMOVCI =
{
    // Moves on FP condition
    {0x0, { "movf",  ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::RS, Src::FCSR }, { Dst::RD }, MIPS_IV_Instr} },
    {0x1, { "movt",  ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::RS, Src::FCSR }, { Dst::RD }, MIPS_IV_Instr} },
};

template<Executable I>
static const Table<I> isaMapMOVCF_d =
{
    // Moves on FP condition
    {0x0, { "movf.d",  ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FCSR }, { Dst::FD }, MIPS_IV_Instr} },
    {0x1, { "movt.d",  ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FCSR }, { Dst::FD }, MIPS_IV_Instr} },
};

template<Executable I>
static const Table<I> isaMapMOVCF_s =
{
    // Moves on FP condition
    {0x0, { "movf.s",  ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FCSR }, { Dst::FD }, MIPS_IV_Instr} },
    {0x1, { "movt.s",  ALU::unknown_instruction<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FCSR }, { Dst::FD }, MIPS_IV_Instr} },
};

template<Executable I>
static const std::vector<const Table<I>*> all_isa_maps =
{
    &isaMapR<I>,
    &isaMapRI<I>,
    &isaMapMIPS32<I>,
    &isaMapIJ<I>,
    &isaMapCOP0_rs<I>,
    &isaMapCOP0_funct<I>,
    &isaMapCOP1<I>,
    &isaMapCOP1X<I>,
    &isaMapCOP1_s<I>,
    &isaMapCOP1_d<I>,
    &isaMapCOP1_l<I>,
    &isaMapCOP1_w<I>,
    &isaMapCOP1I<I>,
    &isaMapMOVCI<I>,
    &isaMapMOVCF_s<I>,
    &isaMapMOVCF_d<I>
};

template<Executable I>
static const MIPSTableEntry<I> unknown_instruction = { };

template<Executable I>
static const MIPSTableEntry<I> instr_nop =
{ "nop" , do_nothing<I>, OUT_ARITHM, 0, 'N', Imm::NO, { }, { Dst::ZERO }, MIPS_I_Instr};

template<Executable I>
static MIPSTableEntry<I> get_table_entry( const Table<I>& table, uint32 key)
{
    auto it = table.find( key);
    return it == table.end() ? unknown_instruction<I> : it->second;
}

template<Executable I>
static MIPSTableEntry<I> get_opcode_special_entry( const MIPSInstrDecoder& instr)
{
    if ( instr.funct == 0x1)
        return get_table_entry( isaMapMOVCI<I>, instr.ft);
    return get_table_entry( isaMapR<I>, instr.funct);
}

template<Executable I>
static MIPSTableEntry<I> get_COP1_s_entry( const MIPSInstrDecoder& instr)
{
    if ( instr.funct == 0x11)
        return get_table_entry( isaMapMOVCF_s<I>, instr.ft);
    return get_table_entry( isaMapCOP1_s<I>, instr.funct);
}

template<Executable I>
static MIPSTableEntry<I> get_COP1_d_entry( const MIPSInstrDecoder& instr)
{
    if ( instr.funct == 0x11)
        return get_table_entry( isaMapMOVCF_d<I>, instr.ft);
    return get_table_entry( isaMapCOP1_d<I>,  instr.funct);
}

template<Executable I>
static MIPSTableEntry<I> get_cp0_entry( const MIPSInstrDecoder& instr)
{
    switch ( instr.funct)
    {
        case 0x0:  return get_table_entry( isaMapCOP0_rs<I>,    instr.rs);
        default:   return get_table_entry( isaMapCOP0_funct<I>, instr.funct);
    }
}

template<Executable I>
static MIPSTableEntry<I> get_cp1_entry( const MIPSInstrDecoder& instr)
{
    switch ( instr.fmt)
    {
        case 0x8:  return get_table_entry( isaMapCOP1I<I>,  instr.ft);
        case 0x10: return get_COP1_s_entry<I>( instr);
        case 0x11: return get_COP1_d_entry<I>( instr);
        case 0x14: return get_table_entry( isaMapCOP1_w<I>, instr.funct);
        case 0x15: return get_table_entry( isaMapCOP1_l<I>, instr.funct);
        default:   return get_table_entry( isaMapCOP1<I>,   instr.fmt);
    }
}

template<Executable I>
static MIPSTableEntry<I> get_table_entry( uint32 bytes)
{
    MIPSInstrDecoder instr( bytes);

    if ( instr.bytes == 0)
        return instr_nop<I>;

    switch ( instr.opcode)
    {
        case 0x0:  return get_opcode_special_entry<I>( instr);
        case 0x1:  return get_table_entry( isaMapRI<I>,     instr.rt);
        case 0x10: return get_cp0_entry<I>( instr);
        case 0x11: return get_cp1_entry<I>( instr);
        case 0x13: return get_table_entry( isaMapCOP1X<I>,  instr.funct);
        case 0x1C: return get_table_entry( isaMapMIPS32<I>, instr.funct);
        default:   return get_table_entry( isaMapIJ<I>,     instr.opcode);
    }
}

template<typename M>
static auto find_entry( const M& map, std::string_view name)
{
    return std::find_if( map.begin(), map.end(), [name]( const auto& e) {
        return e.second.name == name;
    });
}

template<Executable I>
static MIPSTableEntry<I> get_table_entry( std::string_view str_opcode)
{
    if ( str_opcode == "nop")
        return instr_nop<I>;

    for ( const auto& map : all_isa_maps<I>)
    {
        auto res = find_entry( *map, str_opcode);
        if ( res != map->end())
            return res->second;
    }

    return unknown_instruction<I>;
}

template<Unsigned R>
BaseMIPSInstr<R>::BaseMIPSInstr( MIPSVersion version, std::endian endian, uint32 bytes, Addr PC)
    : BaseInstruction<R, MIPSRegister>( PC, PC + 4)
    , raw( bytes)
    , raw_valid( true)
    , endian( endian)
{
    auto entry = get_table_entry<MyDatapath>( raw);
    MIPSInstrDecoder instr( raw);
    init( entry, version);

    for ( size_t i = 0; i < entry.src.size(); ++i)
        this->src.at( i) = instr.get_register( entry.src.at( i));
    for ( size_t i = 0; i < entry.dst.size(); ++i)
        this->dst.at( i) = instr.get_register( entry.dst.at( i));
    this->v_imm = MIPSInstrDecoder::get_immediate<R>( entry.imm_type, instr.get_immediate_value( entry.imm_type));

    init_target();
}

template<Unsigned R>
BaseMIPSInstr<R>::BaseMIPSInstr( MIPSVersion version, std::string_view str_opcode, std::endian endian, uint32 immediate, Addr PC)
    : BaseInstruction<R, MIPSRegister>( PC, PC + 4)
    , raw( 0)
    , endian( endian)
{
    auto entry = get_table_entry<MyDatapath>( str_opcode);
    init( entry, version);
    this->v_imm = MIPSInstrDecoder::get_immediate<R>( entry.imm_type, immediate);
    init_target();
}

template<Unsigned R>
void BaseMIPSInstr<R>::init_target()
{
    if ( this->is_branch())
        this->target = this->PC + 4 + ( sign_extension<bitwidth<R>, Addr>( this->v_imm) << 2U);
    else if ( this->is_direct_jump())
        this->target = ( this->PC & 0xf0000000) | ( this->v_imm << 2U);
}

template<Unsigned R>
void BaseMIPSInstr<R>::init( const MIPSTableEntry<MyDatapath>& entry, MIPSVersion version)
{
    this->imm_print_type = entry.imm_print_type;
    this->set_type( entry.operation);
    this->mem_size   = entry.mem_size;
    this->executor   = entry.versions.is_supported(version) ? entry.function : ALU::unknown_instruction<MyDatapath>;
    this->opname     = entry.name;

    for ( size_t i = 0; i < entry.dst.size(); i++)
        this->print_dst[i] = is_explicit_register( entry.dst[i]);
    for ( size_t i = 0; i < entry.src.size(); i++)
        this->print_src[i] = is_explicit_register( entry.src[i]);

    bool has_delayed_slot = this->is_jump()
        && version != MIPSVersion::mars
        && version != MIPSVersion::mars64
        && (entry.src.empty() || entry.src[0] != Src::EPC);
    this->delayed_slots = has_delayed_slot ? 1 : 0;
    this->new_PC += this->delayed_slots * 4;
}

template<Unsigned R>
typename BaseMIPSInstr<R>::DisasmCache& BaseMIPSInstr<R>::get_disasm_cache()
{
    static DisasmCache instance;
    return instance;
}

template<Unsigned R>
std::string BaseMIPSInstr<R>::string_dump() const
{
    std::ostringstream oss;
    this->dump_content( oss, get_disasm());
    return std::move( oss).str();
}

template<Unsigned R>
std::string BaseMIPSInstr<R>::bytes_dump() const
{
     std::ostringstream oss;
     oss << "Bytes:" << std::hex;
     const auto& bytes = endian == std::endian::little ? unpack_array<std::endian::little>( raw) : unpack_array<std::endian::big>( raw);
     for ( const auto& b : bytes)
         oss << " 0x" << std::setfill( '0') << std::setw( 2) << static_cast<uint16>( b);
     return std::move( oss).str();
}

template<Unsigned R>
std::string BaseMIPSInstr<R>::get_disasm() const
{
    if ( !raw_valid)
        return this->generate_disasm();

    const auto [found, value] = get_disasm_cache().find( raw);
    if ( found) {
        get_disasm_cache().touch( raw);
        return value;
    }
    auto result = this->generate_disasm();
    get_disasm_cache().update( raw, result);
    return result;
}

template class BaseMIPSInstr<uint32>;
template class BaseMIPSInstr<uint64>;

static_assert(std::is_trivially_destructible_v<BaseMIPSInstr<uint32>>, "For performance reasons, BaseMIPSInstr should not contain non-trivial members");
static_assert(std::is_trivially_destructible_v<BaseMIPSInstr<uint64>>, "For performance reasons, BaseMIPSInstr should not contain non-trivial members");
static_assert(std::is_trivially_copyable_v<BaseMIPSInstr<uint32>>, "For performance reasons, BaseMIPSInstr should not contain non-trivial members");
static_assert(std::is_trivially_copyable_v<BaseMIPSInstr<uint64>>, "For performance reasons, BaseMIPSInstr should not contain non-trivial members");
