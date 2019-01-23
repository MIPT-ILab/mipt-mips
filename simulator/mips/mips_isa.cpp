/**
 * mips_isa.cpp - instruction decoder for mips
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#include <func_sim/alu.h>
#include <infra/types.h>
#include <infra/macro.h>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <unordered_map>

#include "mips_instr.h"
#include "mips_instr_decode.h"

template<typename I> void do_nothing(I* /* instr */) { }
template<typename I> auto mips_add     = ALU::addition<I, int32>;
template<typename I> auto mips_addi    = ALU::addition_imm<I, int32>;
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
template<typename I> auto mips_clo     = ALU::clo<I>;
template<typename I> auto mips_clz     = ALU::clz<I>;
template<typename I> auto mips_dadd    = ALU::addition<I, int64>;
template<typename I> auto mips_daddi   = ALU::addition_imm<I, int64>;
template<typename I> auto mips_daddiu  = ALU::addition_imm<I, uint64>;
template<typename I> auto mips_daddu   = ALU::addition<I, uint64>;
template<typename I> auto mips_dclo    = ALU::dclo<I>;
template<typename I> auto mips_dclz    = ALU::dclz<I>;
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
template<typename I> auto mips_dsub    = ALU::subtraction<I, int64>;
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
template<typename I> auto mips_lui     = ALU::lui<I>;
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
template<typename I> auto mips_sub     = ALU::subtraction<I, int32>;
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

template<typename I>
struct MIPSTableEntry
{
    std::string_view name;
    ALU::Execute<I> function;
    OperationType operation;
    uint8 mem_size;
    Imm immediate_type;
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
    {0x0, { "sll",  mips_sll<I>,  OUT_ARITHM, 0, Imm::SHIFT, Src1::RT, Src2::ZERO, Dst::RD, MIPS_I_Instr} },
//  {0x1, { "movci"
    {0x2, { "srl",  mips_srl<I>,  OUT_ARITHM, 0, Imm::SHIFT, Src1::RT, Src2::ZERO, Dst::RD, MIPS_I_Instr} },
    {0x3, { "sra",  mips_sra<I>,  OUT_ARITHM, 0, Imm::SHIFT, Src1::RT, Src2::ZERO, Dst::RD, MIPS_I_Instr} },
    // Variable shifts
    {0x4, { "sllv", mips_sllv<I>, OUT_ARITHM, 0, Imm::NO, Src1::RT, Src2::RS, Dst::RD, MIPS_I_Instr} },
//  {0x5
    {0x6, { "srlv", mips_srlv<I>, OUT_ARITHM, 0, Imm::NO, Src1::RT, Src2::RS, Dst::RD, MIPS_I_Instr} },
    {0x7, { "srav", mips_srav<I>, OUT_ARITHM, 0, Imm::NO, Src1::RT, Src2::RS, Dst::RD, MIPS_I_Instr} },
    // Indirect branches
    {0x8, { "jr",   mips_jr<I>  , OUT_R_JUMP, 0, Imm::NO, Src1::RS, Src2::ZERO, Dst::ZERO, MIPS_I_Instr} },
    {0x9, { "jalr", mips_jalr<I>, OUT_R_JUMP, 0, Imm::NO, Src1::RS, Src2::ZERO, Dst::RD,   MIPS_I_Instr} },
    // Conditional moves (MIPS IV)
    {0xA, { "movz", mips_movz<I>, OUT_R_CONDM, 0, Imm::NO, Src1::RS, Src2::RT, Dst::RD, MIPS_IV_Instr | MIPS_32_Instr} },
    {0xB, { "movn", mips_movn<I>, OUT_R_CONDM, 0, Imm::NO, Src1::RS, Src2::RT, Dst::RD, MIPS_IV_Instr | MIPS_32_Instr} },
    // System calls
    {0xC, { "syscall", mips_syscall<I>, OUT_SYSCALL, 0, Imm::NO, Src1::ZERO, Src2::ZERO, Dst::ZERO, MIPS_I_Instr} },
    {0xD, { "break",   mips_break<I>,   OUT_BREAK,   0, Imm::NO, Src1::ZERO, Src2::ZERO, Dst::ZERO, MIPS_I_Instr} },
    // Hi/Lo manipulators
    {0x10, { "mfhi", mips_mfhi<I>, OUT_ARITHM, 0, Imm::NO, Src1::HI, Src2::ZERO, Dst::RD, MIPS_I_Instr} },
    {0x11, { "mthi", mips_mthi<I>, OUT_ARITHM, 0, Imm::NO, Src1::RS, Src2::ZERO, Dst::HI, MIPS_I_Instr} },
    {0x12, { "mflo", mips_mflo<I>, OUT_ARITHM, 0, Imm::NO, Src1::LO, Src2::ZERO, Dst::RD, MIPS_I_Instr} },
    {0x13, { "mtlo", mips_mtlo<I>, OUT_ARITHM, 0, Imm::NO, Src1::RS, Src2::ZERO, Dst::LO, MIPS_I_Instr} },
    // Doubleword variable shifts
    {0x14, { "dsllv", mips_dsllv<I>, OUT_ARITHM, 0, Imm::NO, Src1::RT, Src2::RS, Dst::RD, MIPS_III_Instr} },
    {0x16, { "dsrlv", mips_dsrlv<I>, OUT_ARITHM, 0, Imm::NO, Src1::RT, Src2::RS, Dst::RD, MIPS_III_Instr} },
    {0x17, { "dsrav", mips_dsrav<I>, OUT_ARITHM, 0, Imm::NO, Src1::RT, Src2::RS, Dst::RD, MIPS_III_Instr} },
    // Multiplication/Division
    {0x18, { "mult",   mips_mult<I>,  OUT_ARITHM, 0, Imm::NO, Src1::RS, Src2::RT, Dst::HI_LO, MIPS_I_Instr} },
    {0x19, { "multu",  mips_multu<I>, OUT_ARITHM, 0, Imm::NO, Src1::RS, Src2::RT, Dst::HI_LO, MIPS_I_Instr} },
    {0x1A, { "div",    mips_div<I>,   OUT_ARITHM, 0, Imm::NO, Src1::RS, Src2::RT, Dst::HI_LO, MIPS_I_Instr} },
    {0x1B, { "divu",   mips_divu<I>,  OUT_ARITHM, 0, Imm::NO, Src1::RS, Src2::RT, Dst::HI_LO, MIPS_I_Instr} },
    {0x1C, { "dmult",  mips_dmult<I>, OUT_ARITHM, 0, Imm::NO, Src1::RS, Src2::RT, Dst::HI_LO, MIPS_III_Instr} },
    {0x1D, { "dmultu", mips_dmultu<I>,OUT_ARITHM, 0, Imm::NO, Src1::RS, Src2::RT, Dst::HI_LO, MIPS_III_Instr} },
    {0x1E, { "ddiv",   mips_ddiv<I>,  OUT_ARITHM, 0, Imm::NO, Src1::RS, Src2::RT, Dst::HI_LO, MIPS_III_Instr} },
    {0x1F, { "ddivu",  mips_ddivu<I>, OUT_ARITHM, 0, Imm::NO, Src1::RS, Src2::RT, Dst::HI_LO, MIPS_III_Instr} },
    // Addition/Subtraction
    {0x20, { "add",  mips_add<I>,  OUT_ARITHM, 0, Imm::NO, Src1::RS, Src2::RT, Dst::RD, MIPS_I_Instr} },
    {0x21, { "addu", mips_addu<I>, OUT_ARITHM, 0, Imm::NO, Src1::RS, Src2::RT, Dst::RD, MIPS_I_Instr} },
    {0x22, { "sub",  mips_sub<I>,  OUT_ARITHM, 0, Imm::NO, Src1::RS, Src2::RT, Dst::RD, MIPS_I_Instr} },
    {0x23, { "subu", mips_subu<I>, OUT_ARITHM, 0, Imm::NO, Src1::RS, Src2::RT, Dst::RD, MIPS_I_Instr} },
    // Logical operations
    {0x24, { "and", mips_and<I>, OUT_ARITHM, 0, Imm::NO, Src1::RS, Src2::RT, Dst::RD, MIPS_I_Instr} },
    {0x25, { "or",  mips_or<I>,  OUT_ARITHM, 0, Imm::NO, Src1::RS, Src2::RT, Dst::RD, MIPS_I_Instr} },
    {0x26, { "xor", mips_xor<I>, OUT_ARITHM, 0, Imm::NO, Src1::RS, Src2::RT, Dst::RD, MIPS_I_Instr} },
    {0x27, { "nor", mips_nor<I>, OUT_ARITHM, 0, Imm::NO, Src1::RS, Src2::RT, Dst::RD, MIPS_I_Instr} },
//  {0x28
//  {0x29
    {0x2A, { "slt",  mips_slt<I>, OUT_ARITHM, 0, Imm::NO, Src1::RS, Src2::RT, Dst::RD, MIPS_I_Instr} },
    {0x2B, { "sltu", mips_sltu<I>, OUT_ARITHM, 0, Imm::NO, Src1::RS, Src2::RT, Dst::RD, MIPS_I_Instr} },
    // Doubleword addition/Subtraction
    {0x2C, { "dadd",  mips_dadd<I>,  OUT_ARITHM, 0, Imm::NO, Src1::RS, Src2::RT, Dst::RD, MIPS_I_Instr} },
    {0x2D, { "daddu", mips_daddu<I>, OUT_ARITHM, 0, Imm::NO, Src1::RS, Src2::RT, Dst::RD, MIPS_I_Instr} },
    {0x2E, { "dsub",  mips_dsub<I>,  OUT_ARITHM, 0, Imm::NO, Src1::RS, Src2::RT, Dst::RD, MIPS_III_Instr} },
    {0x2F, { "dsubu", mips_dsubu<I>, OUT_ARITHM, 0, Imm::NO, Src1::RS, Src2::RT, Dst::RD, MIPS_III_Instr} },
    // Conditional traps (MIPS II)
    {0x30, { "tge",  mips_tge<I>,  OUT_TRAP, 0, Imm::NO, Src1::RS, Src2::RT, Dst::ZERO, MIPS_II_Instr} },
    {0x31, { "tgeu", mips_tgeu<I>, OUT_TRAP, 0, Imm::NO, Src1::RS, Src2::RT, Dst::ZERO, MIPS_II_Instr} },
    {0x32, { "tlt",  mips_tlt<I>,  OUT_TRAP, 0, Imm::NO, Src1::RS, Src2::RT, Dst::ZERO, MIPS_II_Instr} },
    {0x33, { "tltu", mips_tltu<I>, OUT_TRAP, 0, Imm::NO, Src1::RS, Src2::RT, Dst::ZERO, MIPS_II_Instr} },
    {0x34, { "teq",  mips_teq<I>,  OUT_TRAP, 0, Imm::NO, Src1::RS, Src2::RT, Dst::ZERO, MIPS_II_Instr} },
//  {0x35
    {0x36, { "tne",  mips_tne<I>,  OUT_TRAP, 0, Imm::NO, Src1::RS, Src2::RT, Dst::ZERO, MIPS_II_Instr} },
//  {0x37
    // Doubleword shifts
    {0x38, { "dsll",   mips_dsll<I>  , OUT_ARITHM, 0, Imm::SHIFT, Src1::RT, Src2::ZERO, Dst::RD, MIPS_III_Instr} },
    {0x3A, { "dsrl",   mips_dsrl<I>  , OUT_ARITHM, 0, Imm::SHIFT, Src1::RT, Src2::ZERO, Dst::RD, MIPS_III_Instr} },
    {0x3B, { "dsra",   mips_dsra<I>  , OUT_ARITHM, 0, Imm::SHIFT, Src1::RT, Src2::ZERO, Dst::RD, MIPS_III_Instr} },
    {0x3C, { "dsll32", mips_dsll32<I>, OUT_ARITHM, 0, Imm::SHIFT, Src1::RT, Src2::ZERO, Dst::RD, MIPS_III_Instr} },
    {0x3E, { "dsrl32", mips_dsrl32<I>, OUT_ARITHM, 0, Imm::SHIFT, Src1::RT, Src2::ZERO, Dst::RD, MIPS_III_Instr} },
    {0x3F, { "dsra32", mips_dsra32<I>, OUT_ARITHM, 0, Imm::SHIFT, Src1::RT, Src2::ZERO, Dst::RD, MIPS_III_Instr} }
};

//unordered map for RI-instructions
template<typename I>
static const Table<I> isaMapRI =
{
    // Branches
    {0x0,  { "bltz",  mips_bltz<I>,  OUT_BRANCH, 0, Imm::ARITH, Src1::RS, Src2::ZERO, Dst::ZERO, MIPS_I_Instr} },
    {0x1,  { "bgez",  mips_bgez<I>,  OUT_BRANCH, 0, Imm::ARITH, Src1::RS, Src2::ZERO, Dst::ZERO, MIPS_I_Instr} },
    {0x2,  { "bltzl", mips_bltzl<I>, OUT_BRANCH, 0, Imm::ARITH, Src1::RS, Src2::ZERO, Dst::ZERO, MIPS_II_Instr} },
    {0x3,  { "bgezl", mips_bgezl<I>, OUT_BRANCH, 0, Imm::ARITH, Src1::RS, Src2::ZERO, Dst::ZERO, MIPS_II_Instr} },
    // Conditional traps
    {0x8,  { "tgei",  mips_tgei<I>,  OUT_TRAP, 0, Imm::TRAP, Src1::RS, Src2::ZERO, Dst::ZERO, MIPS_II_Instr} },
    {0x9,  { "tgeiu", mips_tgeiu<I>, OUT_TRAP, 0, Imm::TRAP, Src1::RS, Src2::ZERO, Dst::ZERO, MIPS_II_Instr} },
    {0xA,  { "tlti",  mips_tlti<I>,  OUT_TRAP, 0, Imm::TRAP, Src1::RS, Src2::ZERO, Dst::ZERO, MIPS_II_Instr} },
    {0xB,  { "tltiu", mips_tltiu<I>, OUT_TRAP, 0, Imm::TRAP, Src1::RS, Src2::ZERO, Dst::ZERO, MIPS_II_Instr} },
    {0xC,  { "teqi",  mips_teqi<I>,  OUT_TRAP, 0, Imm::TRAP, Src1::RS, Src2::ZERO, Dst::ZERO, MIPS_II_Instr} },
    {0xE,  { "tnei",  mips_tnei<I>,  OUT_TRAP, 0, Imm::TRAP, Src1::RS, Src2::ZERO, Dst::ZERO, MIPS_II_Instr} },
    // Linking branches
    {0x10, { "bltzal",  mips_bltzal<I>,  OUT_BRANCH, 0, Imm::ARITH, Src1::RS, Src2::ZERO, Dst::RA, MIPS_I_Instr} },
    {0x11, { "bgezal",  mips_bgezal<I>,  OUT_BRANCH, 0, Imm::ARITH, Src1::RS, Src2::ZERO, Dst::RA, MIPS_I_Instr} },
    {0x12, { "bltzall", mips_bltzall<I>, OUT_BRANCH, 0, Imm::ARITH, Src1::RS, Src2::ZERO, Dst::RA, MIPS_II_Instr} },
    {0x13, { "bgezall", mips_bgezall<I>, OUT_BRANCH, 0, Imm::ARITH, Src1::RS, Src2::ZERO, Dst::RA, MIPS_II_Instr} }
};

//unordered map for I-instructions and J-instructions
template<typename I>
static const Table<I> isaMapIJ =
{
    // Direct jumps
    {0x2, { "j",   mips_j<I>,   OUT_J_JUMP, 0, Imm::JUMP, Src1::ZERO, Src2::ZERO, Dst::ZERO, MIPS_I_Instr } },
    {0x3, { "jal", mips_jal<I>, OUT_J_JUMP, 0, Imm::JUMP, Src1::ZERO, Src2::ZERO, Dst::RA, MIPS_I_Instr } },
    // Branches
    {0x4, { "beq",  mips_beq<I>,  OUT_BRANCH, 0, Imm::ARITH, Src1::RS, Src2::RT, Dst::ZERO, MIPS_I_Instr} },
    {0x5, { "bne",  mips_bne<I>,  OUT_BRANCH, 0, Imm::ARITH, Src1::RS, Src2::RT, Dst::ZERO, MIPS_I_Instr} },
    {0x6, { "blez", mips_blez<I>, OUT_BRANCH, 0, Imm::ARITH, Src1::RS, Src2::ZERO, Dst::ZERO, MIPS_I_Instr} },
    {0x7, { "bgtz", mips_bgtz<I>, OUT_BRANCH, 0, Imm::ARITH, Src1::RS, Src2::ZERO, Dst::ZERO, MIPS_I_Instr} },
    // Addition/Subtraction
    {0x8, { "addi",  mips_addi<I>,  OUT_ARITHM, 0, Imm::LOGIC, Src1::RS, Src2::ZERO, Dst::RT, MIPS_I_Instr} },
    {0x9, { "addiu", mips_addiu<I>, OUT_ARITHM, 0, Imm::LOGIC, Src1::RS, Src2::ZERO, Dst::RT, MIPS_I_Instr} },
    // Logical operations
    {0xA, { "slti",  mips_slti<I>,  OUT_ARITHM, 0, Imm::LOGIC, Src1::RS, Src2::ZERO, Dst::RT, MIPS_I_Instr} },
    {0xB, { "sltiu", mips_sltiu<I>, OUT_ARITHM, 0, Imm::LOGIC, Src1::RS, Src2::ZERO, Dst::RT, MIPS_I_Instr} },
    {0xC, { "andi",  mips_andi<I>,  OUT_ARITHM, 0, Imm::LOGIC, Src1::RS, Src2::ZERO, Dst::RT, MIPS_I_Instr} },
    {0xD, { "ori",   mips_ori<I>,   OUT_ARITHM, 0, Imm::LOGIC, Src1::RS, Src2::ZERO, Dst::RT, MIPS_I_Instr} },
    {0xE, { "xori",  mips_xori<I>,  OUT_ARITHM, 0, Imm::LOGIC, Src1::RS, Src2::ZERO, Dst::RT, MIPS_I_Instr} },
    {0xF, { "lui",   mips_lui<I>,   OUT_ARITHM, 0, Imm::LOGIC, Src1::ZERO, Src2::ZERO, Dst::RT, MIPS_I_Instr} },
    // 0x10 - 0x13 coprocessor operations
    // Likely branches (MIPS II)
    {0x14, { "beql",  mips_beql<I>,  OUT_BRANCH, 0, Imm::ARITH, Src1::RS, Src2::RT, Dst::ZERO, MIPS_II_Instr} },
    {0x15, { "bnel",  mips_bnel<I>,  OUT_BRANCH, 0, Imm::ARITH, Src1::RS, Src2::RT, Dst::ZERO, MIPS_II_Instr} },
    {0x16, { "blezl", mips_blezl<I>, OUT_BRANCH, 0, Imm::ARITH, Src1::RS, Src2::RT, Dst::ZERO, MIPS_II_Instr} },
    {0x17, { "bgtzl", mips_bgtzl<I>, OUT_BRANCH, 0, Imm::ARITH, Src1::RS, Src2::RT, Dst::ZERO, MIPS_II_Instr} },
    // Doubleword unaligned loads
    {0x1A, { "ldl", mips_ldl<I>,  OUT_LOAD, 8, Imm::ADDR, Src1::RS, Src2::ZERO, Dst::RT, MIPS_III_Instr} },
    {0x1B, { "ldr", mips_ldr<I>,  OUT_LOAD, 8, Imm::ADDR, Src1::RS, Src2::ZERO, Dst::RT, MIPS_III_Instr} },
    // Doubleword addition
    {0x18, { "daddi",  mips_daddi<I>,  OUT_ARITHM, 0, Imm::LOGIC, Src1::RS, Src2::ZERO, Dst::RT, MIPS_III_Instr} },
    {0x19, { "daddiu", mips_daddiu<I>, OUT_ARITHM, 0, Imm::LOGIC, Src1::RS, Src2::ZERO, Dst::RT, MIPS_III_Instr} },
    // Loads
    {0x20, { "lb",  mips_lb<I>,  OUT_LOAD, 1,         Imm::ADDR, Src1::RS, Src2::ZERO, Dst::RT, MIPS_I_Instr} },
    {0x21, { "lh",  mips_lh<I>,  OUT_LOAD, 2,         Imm::ADDR, Src1::RS, Src2::ZERO, Dst::RT, MIPS_I_Instr} },
    {0x22, { "lwl", mips_lwl<I>, OUT_PARTIAL_LOAD, 4, Imm::ADDR, Src1::RS, Src2::ZERO, Dst::RT, MIPS_I_Instr} },
    {0x23, { "lw",  mips_lw<I>,  OUT_LOAD, 4,         Imm::ADDR, Src1::RS, Src2::ZERO, Dst::RT, MIPS_I_Instr} },
    {0x24, { "lbu", mips_lbu<I>, OUT_LOADU, 1,        Imm::ADDR, Src1::RS, Src2::ZERO, Dst::RT, MIPS_I_Instr} },
    {0x25, { "lhu", mips_lhu<I>, OUT_LOADU, 2,        Imm::ADDR, Src1::RS, Src2::ZERO, Dst::RT, MIPS_I_Instr} },
    {0x26, { "lwr", mips_lwr<I>, OUT_PARTIAL_LOAD, 4, Imm::ADDR, Src1::RS, Src2::ZERO, Dst::RT, MIPS_I_Instr} },
    {0x27, { "lwu", mips_lwu<I>, OUT_LOADU, 4,        Imm::ADDR, Src1::RS, Src2::ZERO, Dst::RT, MIPS_I_Instr} },
    // Stores
    {0x28, { "sb",  mips_sb<I>,  OUT_STORE, 1, Imm::ADDR, Src1::RS, Src2::RT, Dst::ZERO, MIPS_I_Instr} },
    {0x29, { "sh",  mips_sh<I>,  OUT_STORE, 2, Imm::ADDR, Src1::RS, Src2::RT, Dst::ZERO, MIPS_I_Instr} },
    {0x2A, { "swl", mips_swl<I>, OUT_STORE, 4, Imm::ADDR, Src1::RS, Src2::RT, Dst::ZERO, MIPS_I_Instr} },
    {0x2B, { "sw",  mips_sw<I>,  OUT_STORE, 4, Imm::ADDR, Src1::RS, Src2::RT, Dst::ZERO, MIPS_I_Instr} },
    {0x2C, { "sdl", mips_sdl<I>, OUT_STORE, 8, Imm::ADDR, Src1::RS, Src2::RT, Dst::ZERO, MIPS_III_Instr} },
    {0x2D, { "sdr", mips_sdr<I>, OUT_STORE, 8, Imm::ADDR, Src1::RS, Src2::RT, Dst::ZERO, MIPS_III_Instr} },
    {0x2E, { "swr", mips_swr<I>, OUT_STORE, 4, Imm::ADDR, Src1::RS, Src2::RT, Dst::ZERO, MIPS_I_Instr} },
//  {0x2F, { "cache"
    // Advanced loads and stores
    {0x30, { "ll", mips_ll<I>,  OUT_LOAD,  2, Imm::ADDR, Src1::RS, Src2::ZERO, Dst::RT, MIPS_I_Instr} },
    {0x37, { "ld", mips_ld<I>,  OUT_LOAD,  8, Imm::ADDR, Src1::RS, Src2::ZERO, Dst::RT, MIPS_III_Instr} },
    {0x38, { "sc", mips_sc<I>,  OUT_STORE, 2, Imm::ADDR, Src1::RS, Src2::RT, Dst::ZERO, MIPS_I_Instr} },
    {0x3F, { "sd", mips_sd<I>,  OUT_STORE, 8, Imm::ADDR, Src1::RS, Src2::RT, Dst::ZERO, MIPS_III_Instr} }
};

template<typename I>
static const Table<I> isaMapMIPS32 =
{
    // Advanced multiplication
    {0x00, { "madd",  mips_madd<I>,  OUT_R_ACCUM, 0, Imm::NO, Src1::RS, Src2::RT, Dst::HI_LO, MIPS_32_Instr} },
    {0x01, { "maddu", mips_maddu<I>, OUT_R_ACCUM, 0, Imm::NO, Src1::RS, Src2::RT, Dst::HI_LO, MIPS_32_Instr} },
    {0x02, { "mul",   mips_mul<I>,   OUT_ARITHM,  0, Imm::NO, Src1::RS, Src2::RT, Dst::RD,    MIPS_32_Instr} },
    {0x04, { "msub",  mips_msub<I>,  OUT_R_SUBTR, 0, Imm::NO, Src1::RS, Src2::RT, Dst::HI_LO, MIPS_32_Instr} },
    {0x05, { "msubu", mips_msubu<I>, OUT_R_SUBTR, 0, Imm::NO, Src1::RS, Src2::RT, Dst::HI_LO, MIPS_32_Instr} },
    // Count leading zeroes/ones
    {0x20, { "clz",  mips_clz<I>,  OUT_ARITHM, 0, Imm::NO, Src1::RS, Src2::ZERO, Dst::RD, MIPS_32_Instr} },
    {0x21, { "clo",  mips_clo<I>,  OUT_ARITHM, 0, Imm::NO, Src1::RS, Src2::ZERO, Dst::RD, MIPS_32_Instr} },
    {0x24, { "dclz", mips_dclz<I>, OUT_ARITHM, 0, Imm::NO, Src1::RS, Src2::ZERO, Dst::RD, MIPS_64_Instr} },
    {0x25, { "dclo", mips_dclo<I>, OUT_ARITHM, 0, Imm::NO, Src1::RS, Src2::ZERO, Dst::RD, MIPS_64_Instr} }
};

template<typename I>
static const Table<I> isaMapCOP0 =
{
    {0x00, { "mtc0",  mips_mtc0<I>, OUT_ARITHM, 0, Imm::NO, Src1::RT, Src2::ZERO, Dst::CP0_RD, MIPS_I_Instr} },
    {0x04, { "mfc0",  mips_mfc0<I>, OUT_ARITHM, 0, Imm::NO, Src1::CP0_RT, Src2::ZERO, Dst::RD, MIPS_I_Instr} },
};

template<typename I>
MIPSTableEntry<I> unknown_instruction =
{ "Unknown instruction", unknown_mips_instruction, OUT_ARITHM, 0, Imm::NO, Src1::ZERO, Src2::ZERO, Dst::ZERO, MIPS_I_Instr};

template<typename I>
MIPSTableEntry<I> nop =
{ "nop" , mips_sll<I>, OUT_ARITHM, 0, Imm::NO, Src1::ZERO, Src2::ZERO, Dst::ZERO, MIPS_I_Instr};

template<typename I>
const MIPSTableEntry<I>& get_table_entry( const Table<I>& table, uint32 key)
{
    auto it = table.find( key);
    return it == table.end() ? unknown_instruction<I> : it->second;
}

template<typename I>
const MIPSTableEntry<I>& get_table_entry( uint32 bytes)
{
    MIPSInstrDecoder instr( bytes);

    if ( instr.bytes == 0)
        return nop<I>;

    switch ( instr.opcode)
    {
        case 0x0:  return get_table_entry( isaMapR<I>,      instr.funct);
        case 0x1:  return get_table_entry( isaMapRI<I>,     instr.rt);
        case 0x10: return get_table_entry( isaMapCOP0<I>,   instr.rs);
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
const MIPSTableEntry<I>& get_table_entry( std::string_view str_opcode)
{
    if ( str_opcode == "nop")
        return nop<I>;

    for ( const auto& map : { isaMapR<I>, isaMapRI<I>, isaMapMIPS32<I>, isaMapIJ<I>, isaMapCOP0<I> })
    {
        auto res = find_entry( map, str_opcode);
        if ( res != map.end())
            return res->second;
    }

    return unknown_instruction<I>;
}

template<typename R>
BaseMIPSInstr<R>::BaseMIPSInstr( MIPSVersion version, uint32 bytes, Addr PC) :
    raw( bytes),
    new_PC( PC + 4),
    PC( PC)
{
    init( get_table_entry<BaseMIPSInstr<R>>( raw), version);
}

template<typename R>
BaseMIPSInstr<R>::BaseMIPSInstr( MIPSVersion version, std::string_view str_opcode, Addr PC)
    : raw( 0)
    , new_PC( PC + 4)
    , PC( PC)
{
    init( get_table_entry<BaseMIPSInstr<R>>( str_opcode), version);
}

template<typename R>
void BaseMIPSInstr<R>::init( const MIPSTableEntry<BaseMIPSInstr<R>>& entry, MIPSVersion version)
{
    MIPSInstrDecoder instr( raw);
    operation = entry.operation;
    mem_size  = entry.mem_size;
    executor  = entry.versions.is_supported(version) ? entry.function : unknown_mips_instruction<BaseMIPSInstr<R>>;
    v_imm     = instr.get_immediate( entry.immediate_type);
    src1      = instr.get_register( entry.src1);
    src2      = instr.get_register( entry.src2);
    dst       = instr.get_register( entry.dst);
    dst2      = ( entry.dst == Reg::HI_LO) ? MIPSRegister::mips_hi() : MIPSRegister::zero();
    disasm    = generate_disasm( entry);
}

static std::string print_immediate( Imm type, uint32 value)
{
    std::ostringstream oss;
    switch ( type)
    {
    case Imm::ADDR:
    case Imm::LOGIC: oss << ", 0x" << std::hex << value << std::dec; break;
    case Imm::JUMP:  oss <<  " 0x" << std::hex << value << std::dec; break;
    case Imm::TRAP:  oss << ", 0x" << std::hex << narrow_cast<int16>(value) << std::dec; break;
    case Imm::ARITH: oss << ", "   << std::dec << narrow_cast<int16>(value); break;
    case Imm::SHIFT: oss << ", "   << std::dec << value; break;
    case Imm::NO:    break;
    }
    return oss.str();
}

template<typename R>
std::string BaseMIPSInstr<R>::generate_disasm( const MIPSTableEntry<BaseMIPSInstr<R>>& entry) const
{
    const bool print_dst = is_explicit_register( entry.dst);

    std::ostringstream oss;
    if ( PC != 0)
        oss << std::hex << "0x" << PC << ": ";
    oss << entry.name;

    if ( entry.immediate_type == Imm::ADDR)
    {
        oss << " $" << (print_dst ? dst : src2)
            << print_immediate( Imm::ADDR, v_imm)
            << "($" << src1 << ")" << std::dec;
        return oss.str();
    }

    const bool print_src1 = is_explicit_register( entry.src1);
    const bool print_src2 = is_explicit_register( entry.src2);
    
    if ( print_dst)
        oss <<  " $" << dst;
    if ( print_src1)
        oss << ( print_dst ? ", $" : " $") << src1;
    if ( print_src2)
        oss << ", $" << src2;

    oss << print_immediate( entry.immediate_type, v_imm);
    return oss.str();
}

template class BaseMIPSInstr<uint32>;
template class BaseMIPSInstr<uint64>;
