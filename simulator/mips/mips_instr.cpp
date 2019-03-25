/**
 * mips_instr.cpp - instruction decoder for mips
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */
 
#include "mips_instr.h"
#include "mips_instr_decode.h"

#include <func_sim/alu.h>
#include <infra/instrcache/LRUCache.h>
#include <infra/macro.h>
#include <infra/types.h>

#include <iomanip>
#include <iostream>
#include <unordered_map>
#include <vector>

template<typename I> void do_nothing(I* /* instr */) { }
template<typename I> auto mips_add     = ALU::addition_overflow<I, uint32>;
template<typename I> auto mips_addi    = ALU::addition_overflow_imm<I, uint32>;
template<typename I> auto mips_addiu   = ALU::addition_imm<I, uint32>;
template<typename I> auto mips_addu    = ALU::addition<I, uint32>;
template<typename I> auto mips_and     = ALU::andv<I>;
template<typename I> auto mips_andi    = ALU::andi<I>;
template<typename I> auto mips_beq     = ALU::branch<I, ALU::eq<I>>;
template<typename I> auto mips_beql    = ALU::branch<I, ALU::eq<I>>;
template<typename I> auto mips_bgez    = ALU::branch<I, ALU::gez<I>>;
template<typename I> auto mips_bgezal  = ALU::branch_and_link<I, ALU::gez<I>>;
template<typename I> auto mips_bgezall = ALU::branch_and_link<I, ALU::gez<I>>;
template<typename I> auto mips_bgezl   = ALU::branch<I, ALU::gez<I>>;
template<typename I> auto mips_bgtz    = ALU::branch<I, ALU::gtz<I>>;
template<typename I> auto mips_bgtzl   = ALU::branch<I, ALU::gtz<I>>;
template<typename I> auto mips_blez    = ALU::branch<I, ALU::lez<I>>;
template<typename I> auto mips_blezl   = ALU::branch<I, ALU::lez<I>>;
template<typename I> auto mips_bltz    = ALU::branch<I, ALU::ltz<I>>;
template<typename I> auto mips_bltzal  = ALU::branch_and_link<I, ALU::ltz<I>>;
template<typename I> auto mips_bltzall = ALU::branch_and_link<I, ALU::ltz<I>>;
template<typename I> auto mips_bltzl   = ALU::branch<I, ALU::ltz<I>>;
template<typename I> auto mips_bne     = ALU::branch<I, ALU::ne<I>>;
template<typename I> auto mips_bnel    = ALU::branch<I, ALU::ne<I>>;
template<typename I> auto mips_break   = ALU::breakpoint<I>;
template<typename I> auto mips_clo     = ALU::clo<I, uint32>;
template<typename I> auto mips_clz     = ALU::clz<I, uint32>;
template<typename I> auto mips_dadd    = ALU::addition_overflow<I, uint64>;
template<typename I> auto mips_daddi   = ALU::addition_overflow_imm<I, uint64>;
template<typename I> auto mips_daddiu  = ALU::addition_imm<I, uint64>;
template<typename I> auto mips_daddu   = ALU::addition<I, uint64>;
template<typename I> auto mips_dclo    = ALU::clo<I, uint64>;
template<typename I> auto mips_dclz    = ALU::clz<I, uint64>;
template<typename I> auto mips_ddiv    = ALU::division<I, int64>;
template<typename I> auto mips_ddivu   = ALU::division<I, uint64>;
template<typename I> auto mips_div     = ALU::division<I, int32>;
template<typename I> auto mips_divu    = ALU::division<I, uint32>;
template<typename I> auto mips_dmult   = ALU::multiplication<I, int64>;
template<typename I> auto mips_dmultu  = ALU::multiplication<I, uint64>;
template<typename I> auto mips_dsll    = ALU::sll<I, uint64>;
template<typename I> auto mips_dsll32  = ALU::dsll32<I>;
template<typename I> auto mips_dsllv   = ALU::sllv<I, uint64>;
template<typename I> auto mips_dsra    = ALU::sra<I, uint64>;
template<typename I> auto mips_dsra32  = ALU::dsra32<I>;
template<typename I> auto mips_dsrav   = ALU::srav<I, uint64>;
template<typename I> auto mips_dsrl    = ALU::srl<I, uint64>;
template<typename I> auto mips_dsrl32  = ALU::dsrl32<I>;
template<typename I> auto mips_dsrlv   = ALU::srlv<I, uint64>;
template<typename I> auto mips_dsub    = ALU::subtraction_overflow<I, uint64>;
template<typename I> auto mips_dsubu   = ALU::subtraction<I, uint64>;
template<typename I> auto mips_j       = ALU::j<I>;
template<typename I> auto mips_jal     = ALU::jump_and_link<I, ALU::j<I>>;
template<typename I> auto mips_jalr    = ALU::jump_and_link<I, ALU::jr<I>>;
template<typename I> auto mips_jr      = ALU::jr<I>;
template<typename I> auto mips_lb      = ALU::load_addr<I>;
template<typename I> auto mips_lbu     = ALU::load_addr<I>;
template<typename I> auto mips_ld      = ALU::load_addr<I>;
template<typename I> auto mips_ldl     = ALU::load_addr<I>;
template<typename I> auto mips_ldr     = ALU::load_addr<I>;
template<typename I> auto mips_lh      = ALU::load_addr_aligned<I>;
template<typename I> auto mips_lhu     = ALU::load_addr_aligned<I>;
template<typename I> auto mips_ll      = ALU::load_addr<I>;
template<typename I> auto mips_lui     = ALU::upper_immediate<I, 16>;
template<typename I> auto mips_lw      = ALU::load_addr_aligned<I>;
template<typename I> auto mips_lwl     = ALU::load_addr_left32<I>;
template<typename I> auto mips_lwr     = ALU::load_addr_right32<I>;
template<typename I> auto mips_lwu     = ALU::load_addr_aligned<I>;
template<typename I> auto mips_madd    = ALU::multiplication<I, int32>;
template<typename I> auto mips_maddu   = ALU::multiplication<I, uint32>;
template<typename I> auto mips_mfc0    = ALU::move<I>;
template<typename I> auto mips_mfhi    = ALU::move<I>;
template<typename I> auto mips_mflo    = ALU::move<I>;
template<typename I> auto mips_movn    = ALU::movn<I>;
template<typename I> auto mips_movz    = ALU::movz<I>;
template<typename I> auto mips_msub    = ALU::multiplication<I, int32>;
template<typename I> auto mips_msubu   = ALU::multiplication<I, uint32>;
template<typename I> auto mips_mtc0    = ALU::move<I>;
template<typename I> auto mips_mthi    = ALU::move<I>;
template<typename I> auto mips_mtlo    = ALU::move<I>;
template<typename I> auto mips_mul     = ALU::multiplication<I, int32>;
template<typename I> auto mips_mult    = ALU::multiplication<I, int32>;
template<typename I> auto mips_multu   = ALU::multiplication<I, uint32>;
template<typename I> auto mips_nor     = ALU::nor<I>;
template<typename I> auto mips_or      = ALU::orv<I>;
template<typename I> auto mips_ori     = ALU::ori<I>;
template<typename I> auto mips_sb      = ALU::store_addr<I>;
template<typename I> auto mips_sc      = ALU::store_addr<I>;
template<typename I> auto mips_sd      = ALU::store_addr<I>;
template<typename I> auto mips_sdl     = ALU::store_addr<I>;
template<typename I> auto mips_sdr     = ALU::store_addr<I>;
template<typename I> auto mips_sh      = ALU::store_addr<I>;
template<typename I> auto mips_sll     = ALU::sll<I, uint32>;
template<typename I> auto mips_sllv    = ALU::sllv<I, uint32>;
template<typename I> auto mips_slt     = ALU::set<I, ALU::lt<I>>;
template<typename I> auto mips_slti    = ALU::set<I, ALU::lti<I>>;
template<typename I> auto mips_sltiu   = ALU::set<I, ALU::ltiu<I>>;
template<typename I> auto mips_sltu    = ALU::set<I, ALU::ltu<I>>;
template<typename I> auto mips_sra     = ALU::sra<I, uint32>;
template<typename I> auto mips_srav    = ALU::srav<I, uint32>;
template<typename I> auto mips_srl     = ALU::srl<I, uint32>;
template<typename I> auto mips_srlv    = ALU::srlv<I, uint32>;
template<typename I> auto mips_sub     = ALU::subtraction_overflow<I, uint32>;
template<typename I> auto mips_subu    = ALU::subtraction<I, uint32>;
template<typename I> auto mips_sw      = ALU::store_addr_aligned<I>;
template<typename I> auto mips_swl     = ALU::store_addr_left32<I>;
template<typename I> auto mips_swr     = ALU::store_addr_right32<I>;
template<typename I> auto mips_syscall = do_nothing<I>;
template<typename I> auto mips_teq     = ALU::trap<I, ALU::eq<I>>;
template<typename I> auto mips_teqi    = ALU::trap<I, ALU::eqi<I>>;
template<typename I> auto mips_tge     = ALU::trap<I, ALU::ge<I>>;
template<typename I> auto mips_tgei    = ALU::trap<I, ALU::gei<I>>;
template<typename I> auto mips_tgeiu   = ALU::trap<I, ALU::geiu<I>>;
template<typename I> auto mips_tgeu    = ALU::trap<I, ALU::geu<I>>;
template<typename I> auto mips_tlt     = ALU::trap<I, ALU::lt<I>>;
template<typename I> auto mips_tlti    = ALU::trap<I, ALU::lti<I>>;
template<typename I> auto mips_tltiu   = ALU::trap<I, ALU::ltiu<I>>;
template<typename I> auto mips_tltu    = ALU::trap<I, ALU::ltu<I>>;
template<typename I> auto mips_tne     = ALU::trap<I, ALU::ne<I>>;
template<typename I> auto mips_tnei    = ALU::trap<I, ALU::nei<I>>;
template<typename I> auto mips_xor     = ALU::xorv<I>;
template<typename I> auto mips_xori    = ALU::xori<I>;
template<typename I> auto mips_unknown = ALU::unknown_instruction<I>;

//CP1 instructions
template<typename I> auto mips_abs_s     = ALU::unknown_instruction<I>;
template<typename I> auto mips_add_s     = ALU::unknown_instruction<I>;
template<typename I> auto mips_ceil_l_s  = ALU::unknown_instruction<I>;
template<typename I> auto mips_div_s     = ALU::unknown_instruction<I>;
template<typename I> auto mips_floor_l_s = ALU::unknown_instruction<I>;
template<typename I> auto mips_mov_s     = ALU::unknown_instruction<I>;
template<typename I> auto mips_mul_s     = ALU::unknown_instruction<I>;
template<typename I> auto mips_neg_s     = ALU::unknown_instruction<I>;
template<typename I> auto mips_round_l_s = ALU::unknown_instruction<I>;
template<typename I> auto mips_sub_s     = ALU::unknown_instruction<I>;
template<typename I> auto mips_sqrt_s    = ALU::unknown_instruction<I>;
template<typename I> auto mips_trunc_l_s = ALU::unknown_instruction<I>;

template<typename I>
struct MIPSTableEntry
{
    std::string_view name;
    ALU::Execute<I> function;
    OperationType operation;
    uint8 mem_size;
    char imm_type;
    Imm imm_print_type;
    Src1 src1;
    Src2 src2;
    Dst  dst;
    MIPSVersionMask versions;
    MIPSTableEntry() = delete;
};

template<typename R>
using Table = std::unordered_map<uint32, MIPSTableEntry<R>>;

//unordered map for R-instructions
template<typename I>
static const Table<I> isaMapR =
{
    // **************** R INSTRUCTIONS ****************
    // Constant shifts
    {0x0, { "sll",  mips_sll<I>,  OUT_ARITHM, 0, 'S', Imm::SHIFT, Src1::RT, Src2::ZERO, Dst::RD, MIPS_I_Instr} },
//  {0x1, { "movci"
    {0x2, { "srl",  mips_srl<I>,  OUT_ARITHM, 0, 'S', Imm::SHIFT, Src1::RT, Src2::ZERO, Dst::RD, MIPS_I_Instr} },
    {0x3, { "sra",  mips_sra<I>,  OUT_ARITHM, 0, 'S', Imm::SHIFT, Src1::RT, Src2::ZERO, Dst::RD, MIPS_I_Instr} },
    // Variable shifts
    {0x4, { "sllv", mips_sllv<I>, OUT_ARITHM, 0, 'N', Imm::NO, Src1::RT, Src2::RS, Dst::RD, MIPS_I_Instr} },
//  {0x5
    {0x6, { "srlv", mips_srlv<I>, OUT_ARITHM, 0, 'N', Imm::NO, Src1::RT, Src2::RS, Dst::RD, MIPS_I_Instr} },
    {0x7, { "srav", mips_srav<I>, OUT_ARITHM, 0, 'N', Imm::NO, Src1::RT, Src2::RS, Dst::RD, MIPS_I_Instr} },
    // Indirect branches
    {0x8, { "jr",   mips_jr<I>  , OUT_R_JUMP, 0, 'N', Imm::NO, Src1::RS, Src2::ZERO, Dst::ZERO, MIPS_I_Instr} },
    {0x9, { "jalr", mips_jalr<I>, OUT_R_JUMP, 0, 'N', Imm::NO, Src1::RS, Src2::ZERO, Dst::RD,   MIPS_I_Instr} },
    // Conditional moves (MIPS IV)
    {0xA, { "movz", mips_movz<I>, OUT_R_CONDM, 0, 'N', Imm::NO, Src1::RS, Src2::RT, Dst::RD, MIPS_IV_Instr | MIPS_32_Instr} },
    {0xB, { "movn", mips_movn<I>, OUT_R_CONDM, 0, 'N', Imm::NO, Src1::RS, Src2::RT, Dst::RD, MIPS_IV_Instr | MIPS_32_Instr} },
    // System calls
    {0xC, { "syscall", mips_syscall<I>, OUT_SYSCALL, 0, 'N', Imm::NO, Src1::ZERO, Src2::ZERO, Dst::ZERO, MIPS_I_Instr} },
    {0xD, { "break",   mips_break<I>,   OUT_BREAK,   0, 'N', Imm::NO, Src1::ZERO, Src2::ZERO, Dst::ZERO, MIPS_I_Instr} },
    // Hi/Lo manipulators
    {0x10, { "mfhi", mips_mfhi<I>, OUT_ARITHM, 0, 'N', Imm::NO, Src1::HI, Src2::ZERO, Dst::RD, MIPS_I_Instr} },
    {0x11, { "mthi", mips_mthi<I>, OUT_ARITHM, 0, 'N', Imm::NO, Src1::RS, Src2::ZERO, Dst::HI, MIPS_I_Instr} },
    {0x12, { "mflo", mips_mflo<I>, OUT_ARITHM, 0, 'N', Imm::NO, Src1::LO, Src2::ZERO, Dst::RD, MIPS_I_Instr} },
    {0x13, { "mtlo", mips_mtlo<I>, OUT_ARITHM, 0, 'N', Imm::NO, Src1::RS, Src2::ZERO, Dst::LO, MIPS_I_Instr} },
    // Doubleword variable shifts
    {0x14, { "dsllv", mips_dsllv<I>, OUT_ARITHM, 0, 'N', Imm::NO, Src1::RT, Src2::RS, Dst::RD, MIPS_III_Instr} },
    {0x16, { "dsrlv", mips_dsrlv<I>, OUT_ARITHM, 0, 'N', Imm::NO, Src1::RT, Src2::RS, Dst::RD, MIPS_III_Instr} },
    {0x17, { "dsrav", mips_dsrav<I>, OUT_ARITHM, 0, 'N', Imm::NO, Src1::RT, Src2::RS, Dst::RD, MIPS_III_Instr} },
    // Multiplication/Division
    {0x18, { "mult",   mips_mult<I>,  OUT_DIVMULT, 0, 'N', Imm::NO, Src1::RS, Src2::RT, Dst::HI_LO, MIPS_I_Instr} },
    {0x19, { "multu",  mips_multu<I>, OUT_DIVMULT, 0, 'N', Imm::NO, Src1::RS, Src2::RT, Dst::HI_LO, MIPS_I_Instr} },
    {0x1A, { "div",    mips_div<I>,   OUT_DIVMULT, 0, 'N', Imm::NO, Src1::RS, Src2::RT, Dst::HI_LO, MIPS_I_Instr} },
    {0x1B, { "divu",   mips_divu<I>,  OUT_DIVMULT, 0, 'N', Imm::NO, Src1::RS, Src2::RT, Dst::HI_LO, MIPS_I_Instr} },
    {0x1C, { "dmult",  mips_dmult<I>, OUT_DIVMULT, 0, 'N', Imm::NO, Src1::RS, Src2::RT, Dst::HI_LO, MIPS_III_Instr} },
    {0x1D, { "dmultu", mips_dmultu<I>,OUT_DIVMULT, 0, 'N', Imm::NO, Src1::RS, Src2::RT, Dst::HI_LO, MIPS_III_Instr} },
    {0x1E, { "ddiv",   mips_ddiv<I>,  OUT_DIVMULT, 0, 'N', Imm::NO, Src1::RS, Src2::RT, Dst::HI_LO, MIPS_III_Instr} },
    {0x1F, { "ddivu",  mips_ddivu<I>, OUT_DIVMULT, 0, 'N', Imm::NO, Src1::RS, Src2::RT, Dst::HI_LO, MIPS_III_Instr} },
    // Addition/Subtraction
    {0x20, { "add",  mips_add<I>,  OUT_ARITHM, 0, 'N', Imm::NO, Src1::RS, Src2::RT, Dst::RD, MIPS_I_Instr} },
    {0x21, { "addu", mips_addu<I>, OUT_ARITHM, 0, 'N', Imm::NO, Src1::RS, Src2::RT, Dst::RD, MIPS_I_Instr} },
    {0x22, { "sub",  mips_sub<I>,  OUT_ARITHM, 0, 'N', Imm::NO, Src1::RS, Src2::RT, Dst::RD, MIPS_I_Instr} },
    {0x23, { "subu", mips_subu<I>, OUT_ARITHM, 0, 'N', Imm::NO, Src1::RS, Src2::RT, Dst::RD, MIPS_I_Instr} },
    // Logical operations
    {0x24, { "and", mips_and<I>, OUT_ARITHM, 0, 'N', Imm::NO, Src1::RS, Src2::RT, Dst::RD, MIPS_I_Instr} },
    {0x25, { "or",  mips_or<I>,  OUT_ARITHM, 0, 'N', Imm::NO, Src1::RS, Src2::RT, Dst::RD, MIPS_I_Instr} },
    {0x26, { "xor", mips_xor<I>, OUT_ARITHM, 0, 'N', Imm::NO, Src1::RS, Src2::RT, Dst::RD, MIPS_I_Instr} },
    {0x27, { "nor", mips_nor<I>, OUT_ARITHM, 0, 'N', Imm::NO, Src1::RS, Src2::RT, Dst::RD, MIPS_I_Instr} },
//  {0x28
//  {0x29
    {0x2A, { "slt",  mips_slt<I>, OUT_ARITHM, 0, 'N', Imm::NO, Src1::RS, Src2::RT, Dst::RD, MIPS_I_Instr} },
    {0x2B, { "sltu", mips_sltu<I>, OUT_ARITHM, 0, 'N', Imm::NO, Src1::RS, Src2::RT, Dst::RD, MIPS_I_Instr} },
    // Doubleword addition/Subtraction
    {0x2C, { "dadd",  mips_dadd<I>,  OUT_ARITHM, 0, 'N', Imm::NO, Src1::RS, Src2::RT, Dst::RD, MIPS_III_Instr} },
    {0x2D, { "daddu", mips_daddu<I>, OUT_ARITHM, 0, 'N', Imm::NO, Src1::RS, Src2::RT, Dst::RD, MIPS_III_Instr} },
    {0x2E, { "dsub",  mips_dsub<I>,  OUT_ARITHM, 0, 'N', Imm::NO, Src1::RS, Src2::RT, Dst::RD, MIPS_III_Instr} },
    {0x2F, { "dsubu", mips_dsubu<I>, OUT_ARITHM, 0, 'N', Imm::NO, Src1::RS, Src2::RT, Dst::RD, MIPS_III_Instr} },
    // Conditional traps (MIPS II)
    {0x30, { "tge",  mips_tge<I>,  OUT_TRAP, 0, 'N', Imm::NO, Src1::RS, Src2::RT, Dst::ZERO, MIPS_II_Instr} },
    {0x31, { "tgeu", mips_tgeu<I>, OUT_TRAP, 0, 'N', Imm::NO, Src1::RS, Src2::RT, Dst::ZERO, MIPS_II_Instr} },
    {0x32, { "tlt",  mips_tlt<I>,  OUT_TRAP, 0, 'N', Imm::NO, Src1::RS, Src2::RT, Dst::ZERO, MIPS_II_Instr} },
    {0x33, { "tltu", mips_tltu<I>, OUT_TRAP, 0, 'N', Imm::NO, Src1::RS, Src2::RT, Dst::ZERO, MIPS_II_Instr} },
    {0x34, { "teq",  mips_teq<I>,  OUT_TRAP, 0, 'N', Imm::NO, Src1::RS, Src2::RT, Dst::ZERO, MIPS_II_Instr} },
//  {0x35
    {0x36, { "tne",  mips_tne<I>,  OUT_TRAP, 0, 'N', Imm::NO, Src1::RS, Src2::RT, Dst::ZERO, MIPS_II_Instr} },
//  {0x37
    // Doubleword shifts
    {0x38, { "dsll",   mips_dsll<I>  , OUT_ARITHM, 0, 'S', Imm::SHIFT, Src1::RT, Src2::ZERO, Dst::RD, MIPS_III_Instr} },
    {0x3A, { "dsrl",   mips_dsrl<I>  , OUT_ARITHM, 0, 'S', Imm::SHIFT, Src1::RT, Src2::ZERO, Dst::RD, MIPS_III_Instr} },
    {0x3B, { "dsra",   mips_dsra<I>  , OUT_ARITHM, 0, 'S', Imm::SHIFT, Src1::RT, Src2::ZERO, Dst::RD, MIPS_III_Instr} },
    {0x3C, { "dsll32", mips_dsll32<I>, OUT_ARITHM, 0, 'S', Imm::SHIFT, Src1::RT, Src2::ZERO, Dst::RD, MIPS_III_Instr} },
    {0x3E, { "dsrl32", mips_dsrl32<I>, OUT_ARITHM, 0, 'S', Imm::SHIFT, Src1::RT, Src2::ZERO, Dst::RD, MIPS_III_Instr} },
    {0x3F, { "dsra32", mips_dsra32<I>, OUT_ARITHM, 0, 'S', Imm::SHIFT, Src1::RT, Src2::ZERO, Dst::RD, MIPS_III_Instr} }
};

//unordered map for RI-instructions
template<typename I>
static const Table<I> isaMapRI =
{
    // Branches
    {0x0,  { "bltz",  mips_bltz<I>,  OUT_BRANCH, 0, 'I', Imm::ARITH, Src1::RS, Src2::ZERO, Dst::ZERO, MIPS_I_Instr} },
    {0x1,  { "bgez",  mips_bgez<I>,  OUT_BRANCH, 0, 'I', Imm::ARITH, Src1::RS, Src2::ZERO, Dst::ZERO, MIPS_I_Instr} },
    {0x2,  { "bltzl", mips_bltzl<I>, OUT_BRANCH, 0, 'I', Imm::ARITH, Src1::RS, Src2::ZERO, Dst::ZERO, MIPS_II_Instr} },
    {0x3,  { "bgezl", mips_bgezl<I>, OUT_BRANCH, 0, 'I', Imm::ARITH, Src1::RS, Src2::ZERO, Dst::ZERO, MIPS_II_Instr} },
    // Conditional traps
    {0x8,  { "tgei",  mips_tgei<I>,  OUT_TRAP, 0, 'I', Imm::TRAP, Src1::RS, Src2::ZERO, Dst::ZERO, MIPS_II_Instr} },
    {0x9,  { "tgeiu", mips_tgeiu<I>, OUT_TRAP, 0, 'I', Imm::TRAP, Src1::RS, Src2::ZERO, Dst::ZERO, MIPS_II_Instr} },
    {0xA,  { "tlti",  mips_tlti<I>,  OUT_TRAP, 0, 'I', Imm::TRAP, Src1::RS, Src2::ZERO, Dst::ZERO, MIPS_II_Instr} },
    {0xB,  { "tltiu", mips_tltiu<I>, OUT_TRAP, 0, 'I', Imm::TRAP, Src1::RS, Src2::ZERO, Dst::ZERO, MIPS_II_Instr} },
    {0xC,  { "teqi",  mips_teqi<I>,  OUT_TRAP, 0, 'I', Imm::TRAP, Src1::RS, Src2::ZERO, Dst::ZERO, MIPS_II_Instr} },
    {0xE,  { "tnei",  mips_tnei<I>,  OUT_TRAP, 0, 'I', Imm::TRAP, Src1::RS, Src2::ZERO, Dst::ZERO, MIPS_II_Instr} },
    // Linking branches
    {0x10, { "bltzal",  mips_bltzal<I>,  OUT_BRANCH, 0, 'I', Imm::ARITH, Src1::RS, Src2::ZERO, Dst::RA, MIPS_I_Instr} },
    {0x11, { "bgezal",  mips_bgezal<I>,  OUT_BRANCH, 0, 'I', Imm::ARITH, Src1::RS, Src2::ZERO, Dst::RA, MIPS_I_Instr} },
    {0x12, { "bltzall", mips_bltzall<I>, OUT_BRANCH, 0, 'I', Imm::ARITH, Src1::RS, Src2::ZERO, Dst::RA, MIPS_II_Instr} },
    {0x13, { "bgezall", mips_bgezall<I>, OUT_BRANCH, 0, 'I', Imm::ARITH, Src1::RS, Src2::ZERO, Dst::RA, MIPS_II_Instr} }
};

//unordered map for I-instructions and J-instructions
template<typename I>
static const Table<I> isaMapIJ =
{
    // Direct jumps
    {0x2, { "j",   mips_j<I>,   OUT_J_JUMP, 0, 'J', Imm::JUMP, Src1::ZERO, Src2::ZERO, Dst::ZERO, MIPS_I_Instr } },
    {0x3, { "jal", mips_jal<I>, OUT_J_JUMP, 0, 'J', Imm::JUMP, Src1::ZERO, Src2::ZERO, Dst::RA, MIPS_I_Instr } },
    // Branches
    {0x4, { "beq",  mips_beq<I>,  OUT_BRANCH, 0, 'I', Imm::ARITH, Src1::RS, Src2::RT, Dst::ZERO, MIPS_I_Instr} },
    {0x5, { "bne",  mips_bne<I>,  OUT_BRANCH, 0, 'I', Imm::ARITH, Src1::RS, Src2::RT, Dst::ZERO, MIPS_I_Instr} },
    {0x6, { "blez", mips_blez<I>, OUT_BRANCH, 0, 'I', Imm::ARITH, Src1::RS, Src2::ZERO, Dst::ZERO, MIPS_I_Instr} },
    {0x7, { "bgtz", mips_bgtz<I>, OUT_BRANCH, 0, 'I', Imm::ARITH, Src1::RS, Src2::ZERO, Dst::ZERO, MIPS_I_Instr} },
    // Addition/Subtraction
    {0x8, { "addi",  mips_addi<I>,  OUT_ARITHM, 0, 'I', Imm::ARITH, Src1::RS, Src2::ZERO, Dst::RT, MIPS_I_Instr} },
    {0x9, { "addiu", mips_addiu<I>, OUT_ARITHM, 0, 'I', Imm::ARITH, Src1::RS, Src2::ZERO, Dst::RT, MIPS_I_Instr} },
    // Logical operations
    {0xA, { "slti",  mips_slti<I>,  OUT_ARITHM, 0, 'I', Imm::ARITH, Src1::RS, Src2::ZERO, Dst::RT, MIPS_I_Instr} },
    {0xB, { "sltiu", mips_sltiu<I>, OUT_ARITHM, 0, 'I', Imm::ARITH, Src1::RS, Src2::ZERO, Dst::RT, MIPS_I_Instr} },
    {0xC, { "andi",  mips_andi<I>,  OUT_ARITHM, 0, 'L', Imm::LOGIC, Src1::RS, Src2::ZERO, Dst::RT, MIPS_I_Instr} },
    {0xD, { "ori",   mips_ori<I>,   OUT_ARITHM, 0, 'L', Imm::LOGIC, Src1::RS, Src2::ZERO, Dst::RT, MIPS_I_Instr} },
    {0xE, { "xori",  mips_xori<I>,  OUT_ARITHM, 0, 'L', Imm::LOGIC, Src1::RS, Src2::ZERO, Dst::RT, MIPS_I_Instr} },
    {0xF, { "lui",   mips_lui<I>,   OUT_ARITHM, 0, 'I', Imm::LOGIC, Src1::ZERO, Src2::ZERO, Dst::RT, MIPS_I_Instr} },
    // 0x10 - 0x13 coprocessor operations
    // Likely branches (MIPS II)
    {0x14, { "beql",  mips_beql<I>,  OUT_BRANCH, 0, 'I', Imm::ARITH, Src1::RS, Src2::RT, Dst::ZERO, MIPS_II_Instr} },
    {0x15, { "bnel",  mips_bnel<I>,  OUT_BRANCH, 0, 'I', Imm::ARITH, Src1::RS, Src2::RT, Dst::ZERO, MIPS_II_Instr} },
    {0x16, { "blezl", mips_blezl<I>, OUT_BRANCH, 0, 'I', Imm::ARITH, Src1::RS, Src2::RT, Dst::ZERO, MIPS_II_Instr} },
    {0x17, { "bgtzl", mips_bgtzl<I>, OUT_BRANCH, 0, 'I', Imm::ARITH, Src1::RS, Src2::RT, Dst::ZERO, MIPS_II_Instr} },
    // Doubleword unaligned loads
    {0x1A, { "ldl", mips_ldl<I>,  OUT_LOAD, 8, 'I', Imm::ADDR, Src1::RS, Src2::ZERO, Dst::RT, MIPS_III_Instr} },
    {0x1B, { "ldr", mips_ldr<I>,  OUT_LOAD, 8, 'I', Imm::ADDR, Src1::RS, Src2::ZERO, Dst::RT, MIPS_III_Instr} },
    // Doubleword addition
    {0x18, { "daddi",  mips_daddi<I>,  OUT_ARITHM, 0, 'I', Imm::ARITH, Src1::RS, Src2::ZERO, Dst::RT, MIPS_III_Instr} },
    {0x19, { "daddiu", mips_daddiu<I>, OUT_ARITHM, 0, 'I', Imm::ARITH, Src1::RS, Src2::ZERO, Dst::RT, MIPS_III_Instr} },
    // Loads
    {0x20, { "lb",  mips_lb<I>,  OUT_LOAD, 1, 'I',         Imm::ADDR, Src1::RS, Src2::ZERO, Dst::RT, MIPS_I_Instr} },
    {0x21, { "lh",  mips_lh<I>,  OUT_LOAD, 2, 'I',         Imm::ADDR, Src1::RS, Src2::ZERO, Dst::RT, MIPS_I_Instr} },
    {0x22, { "lwl", mips_lwl<I>, OUT_PARTIAL_LOAD, 4, 'I', Imm::ADDR, Src1::RS, Src2::ZERO, Dst::RT, MIPS_I_Instr} },
    {0x23, { "lw",  mips_lw<I>,  OUT_LOAD, 4, 'I',         Imm::ADDR, Src1::RS, Src2::ZERO, Dst::RT, MIPS_I_Instr} },
    {0x24, { "lbu", mips_lbu<I>, OUT_LOADU, 1, 'I',        Imm::ADDR, Src1::RS, Src2::ZERO, Dst::RT, MIPS_I_Instr} },
    {0x25, { "lhu", mips_lhu<I>, OUT_LOADU, 2, 'I',        Imm::ADDR, Src1::RS, Src2::ZERO, Dst::RT, MIPS_I_Instr} },
    {0x26, { "lwr", mips_lwr<I>, OUT_PARTIAL_LOAD, 4, 'I', Imm::ADDR, Src1::RS, Src2::ZERO, Dst::RT, MIPS_I_Instr} },
    {0x27, { "lwu", mips_lwu<I>, OUT_LOADU, 4, 'I',        Imm::ADDR, Src1::RS, Src2::ZERO, Dst::RT, MIPS_I_Instr} },
    // Stores
    {0x28, { "sb",  mips_sb<I>,  OUT_STORE, 1, 'I', Imm::ADDR, Src1::RS, Src2::RT, Dst::ZERO, MIPS_I_Instr} },
    {0x29, { "sh",  mips_sh<I>,  OUT_STORE, 2, 'I', Imm::ADDR, Src1::RS, Src2::RT, Dst::ZERO, MIPS_I_Instr} },
    {0x2A, { "swl", mips_swl<I>, OUT_STORE, 4, 'I', Imm::ADDR, Src1::RS, Src2::RT, Dst::ZERO, MIPS_I_Instr} },
    {0x2B, { "sw",  mips_sw<I>,  OUT_STORE, 4, 'I', Imm::ADDR, Src1::RS, Src2::RT, Dst::ZERO, MIPS_I_Instr} },
    {0x2C, { "sdl", mips_sdl<I>, OUT_STORE, 8, 'I', Imm::ADDR, Src1::RS, Src2::RT, Dst::ZERO, MIPS_III_Instr} },
    {0x2D, { "sdr", mips_sdr<I>, OUT_STORE, 8, 'I', Imm::ADDR, Src1::RS, Src2::RT, Dst::ZERO, MIPS_III_Instr} },
    {0x2E, { "swr", mips_swr<I>, OUT_STORE, 4, 'I', Imm::ADDR, Src1::RS, Src2::RT, Dst::ZERO, MIPS_I_Instr} },
//  {0x2F, { "cache"
    // Advanced loads and stores
    {0x30, { "ll", mips_ll<I>,  OUT_LOAD,  2, 'I', Imm::ADDR, Src1::RS, Src2::ZERO, Dst::RT, MIPS_I_Instr} },
    {0x37, { "ld", mips_ld<I>,  OUT_LOAD,  8, 'I', Imm::ADDR, Src1::RS, Src2::ZERO, Dst::RT, MIPS_III_Instr} },
    {0x38, { "sc", mips_sc<I>,  OUT_STORE, 2, 'I', Imm::ADDR, Src1::RS, Src2::RT, Dst::ZERO, MIPS_I_Instr} },
    {0x3F, { "sd", mips_sd<I>,  OUT_STORE, 8, 'I', Imm::ADDR, Src1::RS, Src2::RT, Dst::ZERO, MIPS_III_Instr} }
};

template<typename I>
static const Table<I> isaMapMIPS32 =
{
    // Advanced multiplication
    {0x00, { "madd",  mips_madd<I>,  OUT_R_ACCUM, 0, 'N', Imm::NO, Src1::RS, Src2::RT, Dst::HI_LO, MIPS_32_Instr} },
    {0x01, { "maddu", mips_maddu<I>, OUT_R_ACCUM, 0, 'N', Imm::NO, Src1::RS, Src2::RT, Dst::HI_LO, MIPS_32_Instr} },
    {0x02, { "mul",   mips_mul<I>,   OUT_ARITHM,  0, 'N', Imm::NO, Src1::RS, Src2::RT, Dst::RD,    MIPS_32_Instr} },
    {0x04, { "msub",  mips_msub<I>,  OUT_R_SUBTR, 0, 'N', Imm::NO, Src1::RS, Src2::RT, Dst::HI_LO, MIPS_32_Instr} },
    {0x05, { "msubu", mips_msubu<I>, OUT_R_SUBTR, 0, 'N', Imm::NO, Src1::RS, Src2::RT, Dst::HI_LO, MIPS_32_Instr} },
    // Count leading zeroes/ones
    {0x20, { "clz",  mips_clz<I>,  OUT_ARITHM, 0, 'N', Imm::NO, Src1::RS, Src2::ZERO, Dst::RD, MIPS_32_Instr} },
    {0x21, { "clo",  mips_clo<I>,  OUT_ARITHM, 0, 'N', Imm::NO, Src1::RS, Src2::ZERO, Dst::RD, MIPS_32_Instr} },
    {0x24, { "dclz", mips_dclz<I>, OUT_ARITHM, 0, 'N', Imm::NO, Src1::RS, Src2::ZERO, Dst::RD, MIPS_64_Instr} },
    {0x25, { "dclo", mips_dclo<I>, OUT_ARITHM, 0, 'N', Imm::NO, Src1::RS, Src2::ZERO, Dst::RD, MIPS_64_Instr} }
};

template<typename I>
static const Table<I> isaMapCOP0 =
{
    {0x00, { "mtc0",  mips_mtc0<I>, OUT_ARITHM, 0, 'N', Imm::NO, Src1::RT, Src2::ZERO, Dst::CP0_RD, MIPS_I_Instr} },
    {0x04, { "mfc0",  mips_mfc0<I>, OUT_ARITHM, 0, 'N', Imm::NO, Src1::CP0_RT, Src2::ZERO, Dst::RD, MIPS_I_Instr} },
};

template<typename I>
static const Table<I> isaMapCOP1 =
{
    {0x00, { "add.s",      mips_add_s<I>,     OUT_FPU, 0, 'N', Imm::NO, Src1::CP1_FS, Src2::CP1_FT, Dst::CP1_FD, MIPS_I_Instr} },
    {0x01, { "sub.s",      mips_sub_s<I>,     OUT_FPU, 0, 'N', Imm::NO, Src1::CP1_FS, Src2::CP1_FT, Dst::CP1_FD, MIPS_I_Instr} },
    {0x02, { "mul.s",      mips_mul_s<I>,     OUT_FPU, 0, 'N', Imm::NO, Src1::CP1_FS, Src2::CP1_FT, Dst::CP1_FD, MIPS_I_Instr} },
    {0x03, { "div.s",      mips_div_s<I>,     OUT_FPU, 0, 'N', Imm::NO, Src1::CP1_FS, Src2::CP1_FT, Dst::CP1_FD, MIPS_I_Instr} },
    {0x04, { "sqrt.s",     mips_sqrt_s<I>,    OUT_FPU, 0, 'N', Imm::NO, Src1::CP1_FS, Src2::ZERO,   Dst::CP1_FD, MIPS_II_Instr} },
    {0x05, { "abs.s",      mips_abs_s<I>,     OUT_FPU, 0, 'N', Imm::NO, Src1::CP1_FS, Src2::ZERO,   Dst::CP1_FD, MIPS_I_Instr} },
    {0x06, { "mov.s",      mips_mov_s<I>,     OUT_FPU, 0, 'N', Imm::NO, Src1::CP1_FS, Src2::ZERO,   Dst::CP1_FD, MIPS_I_Instr} },
    {0x07, { "neg.s",      mips_neg_s<I>,     OUT_FPU, 0, 'N', Imm::NO, Src1::CP1_FS, Src2::ZERO,   Dst::CP1_FD, MIPS_I_Instr} },
    {0x08, { "round.l.s",  mips_round_l_s<I>, OUT_FPU, 0, 'N', Imm::NO, Src1::CP1_FS, Src2::ZERO,   Dst::CP1_FD, MIPS_III_Instr} },
    {0x09, { "trunc.l.s",  mips_trunc_l_s<I>, OUT_FPU, 0, 'N', Imm::NO, Src1::CP1_FS, Src2::ZERO,   Dst::CP1_FD, MIPS_III_Instr} },
    {0x0A, { "ceil.l.s",   mips_ceil_l_s<I>,  OUT_FPU, 0, 'N', Imm::NO, Src1::CP1_FS, Src2::ZERO,   Dst::CP1_FD, MIPS_III_Instr} },
    {0x0B, { "floor.l.s",  mips_floor_l_s<I>, OUT_FPU, 0, 'N', Imm::NO, Src1::CP1_FS, Src2::ZERO,   Dst::CP1_FD, MIPS_III_Instr} },
};

template<typename I>
static const std::vector<const Table<I>*> all_isa_maps =
{
    &isaMapR<I>,
    &isaMapRI<I>,
    &isaMapMIPS32<I>,
    &isaMapIJ<I>,
    &isaMapCOP0<I>,
    &isaMapCOP1<I>
};

template<typename I>
MIPSTableEntry<I> unknown_instruction =
{ "Unknown instruction", mips_unknown<I> , OUT_ARITHM, 0, 'N', Imm::NO, Src1::ZERO, Src2::ZERO, Dst::ZERO, MIPS_I_Instr};

template<typename I>
MIPSTableEntry<I> nop =
{ "nop" , do_nothing<I>, OUT_ARITHM, 0, 'N', Imm::NO, Src1::ZERO, Src2::ZERO, Dst::ZERO, MIPS_I_Instr};

template<typename I>
MIPSTableEntry<I> get_table_entry( const Table<I>& table, uint32 key)
{
    auto it = table.find( key);
    return it == table.end() ? unknown_instruction<I> : it->second;
}

template<typename I>
MIPSTableEntry<I> get_table_entry( uint32 bytes)
{
    MIPSInstrDecoder instr( bytes);

    if ( instr.bytes == 0)
        return nop<I>;

    switch ( instr.opcode)
    {
        case 0x0:  return get_table_entry( isaMapR<I>,      instr.funct);
        case 0x1:  return get_table_entry( isaMapRI<I>,     instr.rt);
        case 0x10: return get_table_entry( isaMapCOP0<I>,   instr.rs);
        case 0x11: return get_table_entry( isaMapCOP1<I>,   instr.funct);
        case 0x1C: return get_table_entry( isaMapMIPS32<I>, instr.funct);
        default:   return get_table_entry( isaMapIJ<I>,     instr.opcode);
    }
}

template<typename M>
auto find_entry( const M& map, std::string_view name)
{
    return std::find_if( map.begin(), map.end(), [name]( const auto& e) {
        return e.second.name == name;
    });
}

template<typename I>
MIPSTableEntry<I> get_table_entry( std::string_view str_opcode)
{
    if ( str_opcode == "nop")
        return nop<I>;

    for ( const auto& map : all_isa_maps<I>)
    {
        auto res = find_entry( *map, str_opcode);
        if ( res != map->end())
            return res->second;
    }

    return unknown_instruction<I>;
}

static_assert(std::is_trivially_destructible<BaseMIPSInstr<uint32>>::value,
              "For performance reasons, BaseMIPSInstr should not contain non-trivial members");
static_assert(std::is_trivially_destructible<BaseMIPSInstr<uint64>>::value,
              "For performance reasons, BaseMIPSInstr should not contain non-trivial members");
// Visual Studio implements is_trivially copyable in a buggy way
// https://developercommunity.visualstudio.com/content/problem/170883/msvc-type-traits-stdis-trivial-is-bugged.html
#ifdef __GNUC__
static_assert(std::is_trivially_copyable<BaseMIPSInstr<uint32>>::value,
              "For performance reasons, BaseMIPSInstr should not contain non-trivial members");
static_assert(std::is_trivially_copyable<BaseMIPSInstr<uint64>>::value,
              "For performance reasons, BaseMIPSInstr should not contain non-trivial members");
#endif

template<typename R>
BaseMIPSInstr<R>::BaseMIPSInstr( MIPSVersion version, Endian endian, uint32 bytes, Addr PC)
    : BaseInstruction<R, MIPSRegister>( PC, PC + 4)
    , raw( bytes)
    , raw_valid( true)
    , endian( endian)
{
    auto entry = get_table_entry<MyDatapath>( raw);
    MIPSInstrDecoder instr( raw);
    init( entry, version);
 
    this->src1  = instr.get_register( entry.src1);
    this->src2  = instr.get_register( entry.src2);
    this->dst   = instr.get_register( entry.dst);
    this->v_imm = MIPSInstrDecoder::get_immediate<R>( entry.imm_type, instr.get_immediate_value( entry.imm_type));

    init_target();
}

template<typename R>
BaseMIPSInstr<R>::BaseMIPSInstr( MIPSVersion version, std::string_view str_opcode, Endian endian, uint32 immediate, Addr PC)
    : BaseInstruction<R, MIPSRegister>( PC, PC + 4)
    , raw( 0)
    , endian( endian)
{
    auto entry = get_table_entry<MyDatapath>( str_opcode);
    init( entry, version);
    this->v_imm = MIPSInstrDecoder::get_immediate<R>( entry.imm_type, immediate);
    init_target();
}

template<typename R>
void BaseMIPSInstr<R>::init_target()
{
    if ( this->is_branch())
        this->target = this->PC + 4 + ( sign_extension<bitwidth<R>, Addr>( this->v_imm) << 2U);
    else if ( this->is_direct_jump())
        this->target = ( this->PC & 0xf0000000) | ( this->v_imm << 2U);
}

template<typename R>
void BaseMIPSInstr<R>::init( const MIPSTableEntry<MyDatapath>& entry, MIPSVersion version)
{
    this->imm_print_type = entry.imm_print_type;
    this->operation = entry.operation;
    this->mem_size  = entry.mem_size;
    this->executor  = entry.versions.is_supported(version) ? entry.function : mips_unknown<MyDatapath>;
    this->dst2      = ( entry.dst == Reg::HI_LO) ? MIPSRegister::mips_hi() : MIPSRegister::zero();
    this->opname    = entry.name;
    this->print_dst = is_explicit_register( entry.dst);
    this->print_src1 = is_explicit_register( entry.src1);
    this->print_src2 = is_explicit_register( entry.src2);

    bool has_delayed_slot = this->is_jump() && version != MIPSVersion::mars && version != MIPSVersion::mars64;
    this->delayed_slots = has_delayed_slot ? 1 : 0;
}

template<typename R>
typename BaseMIPSInstr<R>::DisasmCache& BaseMIPSInstr<R>::get_disasm_cache()
{
    static DisasmCache instance;
    return instance;
}

template<typename R>
std::string BaseMIPSInstr<R>::string_dump() const
{
    std::ostringstream oss;
    this->dump_content( oss, get_disasm());
    return oss.str();
}

template<typename R>
std::string BaseMIPSInstr<R>::bytes_dump() const
{
     std::ostringstream oss;
     oss << "Bytes:" << std::hex;
     const auto& bytes = endian == Endian::little ? unpack_array<uint32, Endian::little>( raw) : unpack_array<uint32, Endian::big>( raw);
     for ( const auto& b : bytes)
         oss << " 0x" << std::setfill( '0') << std::setw( 2) << static_cast<uint16>( b);
     return oss.str();
}

template<typename R>
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
