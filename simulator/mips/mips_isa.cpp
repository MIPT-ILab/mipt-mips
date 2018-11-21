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

enum class RegType : uint8
{
    RS, RT, RD,
    ZERO, RA,
    HI, LO, HI_LO
};

bool is_explicit_register( RegType type)
{
    return type == RegType::RS
        || type == RegType::RT
        || type == RegType::RD;
}

template<typename R>
struct MIPSTableEntry
{
    std::string_view name;
    ALU::Execute<BaseMIPSInstr<R>> function;
    OperationType operation;
    uint8 mem_size;
    RegType src1;
    RegType src2;
    RegType dst;
    MIPSVersionMask versions;
    MIPSTableEntry() = delete;
};

MIPSRegister get_register( const MIPSInstrDecoder& instr, RegType type)
{
    switch ( type) {
    case RegType::HI:    return MIPSRegister::mips_hi;
    case RegType::LO:    return MIPSRegister::mips_lo;
    case RegType::HI_LO: return MIPSRegister::mips_lo;
    case RegType::ZERO:  return MIPSRegister::zero;
    case RegType::RA:    return MIPSRegister::return_address;
    case RegType::RS:    return MIPSRegister::from_cpu_index( instr.rs);
    case RegType::RT:    return MIPSRegister::from_cpu_index( instr.rt);
    case RegType::RD:    return MIPSRegister::from_cpu_index( instr.rd);
    default: assert(0);  return MIPSRegister::zero;
    }
}


static uint32 get_immediate( const MIPSInstrDecoder& instr, OperationType operation)
{
    switch ( operation)
    {
    case OUT_I_ARITHM:
    case OUT_I_BRANCH:
    case OUT_RI_TRAP:
    case OUT_I_LOAD:
    case OUT_I_LOADU:
    case OUT_I_PARTIAL_LOAD:
    case OUT_I_STORE:
        return instr.imm;
    case OUT_J_JUMP:
        return instr.jump;
    default:
        return 0;
    }    
}

template<typename R> auto mips_add     = ALU::addition<BaseMIPSInstr<R>, int32>;
template<typename R> auto mips_addi    = ALU::addition_imm<BaseMIPSInstr<R>, int32>;
template<typename R> auto mips_addiu   = ALU::addition_imm<BaseMIPSInstr<R>, uint32>;
template<typename R> auto mips_addu    = ALU::addition<BaseMIPSInstr<R>, uint32>;
template<typename R> auto mips_and     = ALU::andv<BaseMIPSInstr<R>>;
template<typename R> auto mips_andi    = ALU::andi<BaseMIPSInstr<R>>;
template<typename R> auto mips_beq     = ALU::branch<BaseMIPSInstr<R>, ALU::eq<BaseMIPSInstr<R>>>;
template<typename R> auto mips_beql    = ALU::branch<BaseMIPSInstr<R>, ALU::eq<BaseMIPSInstr<R>>>;
template<typename R> auto mips_bgez    = ALU::branch<BaseMIPSInstr<R>, ALU::gez<BaseMIPSInstr<R>>>;
template<typename R> auto mips_bgezal  = ALU::branch_and_link<BaseMIPSInstr<R>, ALU::gez<BaseMIPSInstr<R>>>;
template<typename R> auto mips_bgezall = ALU::branch_and_link<BaseMIPSInstr<R>, ALU::gez<BaseMIPSInstr<R>>>;
template<typename R> auto mips_bgezl   = ALU::branch<BaseMIPSInstr<R>, ALU::gez<BaseMIPSInstr<R>>>;
template<typename R> auto mips_bgtz    = ALU::branch<BaseMIPSInstr<R>, ALU::gtz<BaseMIPSInstr<R>>>;
template<typename R> auto mips_bgtzl   = ALU::branch<BaseMIPSInstr<R>, ALU::gtz<BaseMIPSInstr<R>>>;
template<typename R> auto mips_blez    = ALU::branch<BaseMIPSInstr<R>, ALU::lez<BaseMIPSInstr<R>>>;
template<typename R> auto mips_blezl   = ALU::branch<BaseMIPSInstr<R>, ALU::lez<BaseMIPSInstr<R>>>;
template<typename R> auto mips_bltz    = ALU::branch<BaseMIPSInstr<R>, ALU::ltz<BaseMIPSInstr<R>>>;
template<typename R> auto mips_bltzal  = ALU::branch_and_link<BaseMIPSInstr<R>, ALU::ltz<BaseMIPSInstr<R>>>;
template<typename R> auto mips_bltzall = ALU::branch_and_link<BaseMIPSInstr<R>, ALU::ltz<BaseMIPSInstr<R>>>;
template<typename R> auto mips_bltzl   = ALU::branch<BaseMIPSInstr<R>, ALU::ltz<BaseMIPSInstr<R>>>;
template<typename R> auto mips_bne     = ALU::branch<BaseMIPSInstr<R>, ALU::ne<BaseMIPSInstr<R>>>;
template<typename R> auto mips_bnel    = ALU::branch<BaseMIPSInstr<R>, ALU::ne<BaseMIPSInstr<R>>>;
template<typename R> auto mips_break   = ALU::breakpoint<BaseMIPSInstr<R>>;
template<typename R> auto mips_clo     = ALU::clo<BaseMIPSInstr<R>>;
template<typename R> auto mips_clz     = ALU::clz<BaseMIPSInstr<R>>;
template<typename R> auto mips_dadd    = ALU::addition<BaseMIPSInstr<R>, int64>;
template<typename R> auto mips_daddi   = ALU::addition_imm<BaseMIPSInstr<R>, int64>;
template<typename R> auto mips_daddiu  = ALU::addition_imm<BaseMIPSInstr<R>, uint64>;
template<typename R> auto mips_daddu   = ALU::addition<BaseMIPSInstr<R>, uint64>;
template<typename R> auto mips_dclo    = ALU::dclo<BaseMIPSInstr<R>>;
template<typename R> auto mips_dclz    = ALU::dclz<BaseMIPSInstr<R>>;
template<typename R> auto mips_ddiv    = ALU::division<BaseMIPSInstr<R>, int64>;
template<typename R> auto mips_ddivu   = ALU::division<BaseMIPSInstr<R>, uint64>;
template<typename R> auto mips_div     = ALU::division<BaseMIPSInstr<R>, int32>;
template<typename R> auto mips_divu    = ALU::division<BaseMIPSInstr<R>, uint32>;
template<typename R> auto mips_dmult   = ALU::multiplication<BaseMIPSInstr<R>, int64>;
template<typename R> auto mips_dmultu  = ALU::multiplication<BaseMIPSInstr<R>, uint64>;
template<typename R> auto mips_dsll    = ALU::sll<BaseMIPSInstr<R>, uint64>;
template<typename R> auto mips_dsll32  = ALU::dsll32<BaseMIPSInstr<R>>;
template<typename R> auto mips_dsllv   = ALU::sllv<BaseMIPSInstr<R>, uint64>;
template<typename R> auto mips_dsra    = ALU::sra<BaseMIPSInstr<R>, uint64>;
template<typename R> auto mips_dsra32  = ALU::dsra32<BaseMIPSInstr<R>>;
template<typename R> auto mips_dsrav   = ALU::srav<BaseMIPSInstr<R>, uint64>;
template<typename R> auto mips_dsrl    = ALU::srl<BaseMIPSInstr<R>, uint64>;
template<typename R> auto mips_dsrl32  = ALU::dsrl32<BaseMIPSInstr<R>>;
template<typename R> auto mips_dsrlv   = ALU::srlv<BaseMIPSInstr<R>, uint64>;
template<typename R> auto mips_dsub    = ALU::subtraction<BaseMIPSInstr<R>, int64>;
template<typename R> auto mips_dsubu   = ALU::subtraction<BaseMIPSInstr<R>, uint64>;
template<typename R> auto mips_j       = ALU::j<BaseMIPSInstr<R>>;
template<typename R> auto mips_jal     = ALU::jump_and_link<BaseMIPSInstr<R>, ALU::j<BaseMIPSInstr<R>>>;
template<typename R> auto mips_jalr    = ALU::jump_and_link<BaseMIPSInstr<R>, ALU::jr<BaseMIPSInstr<R>>>;
template<typename R> auto mips_jr      = ALU::jr<BaseMIPSInstr<R>>;
template<typename R> auto mips_lb      = ALU::load_addr<BaseMIPSInstr<R>>;
template<typename R> auto mips_lbu     = ALU::load_addr<BaseMIPSInstr<R>>;
template<typename R> auto mips_ld      = ALU::load_addr<BaseMIPSInstr<R>>;
template<typename R> auto mips_ldl     = ALU::load_addr<BaseMIPSInstr<R>>;
template<typename R> auto mips_ldr     = ALU::load_addr<BaseMIPSInstr<R>>;
template<typename R> auto mips_lh      = ALU::load_addr_aligned<BaseMIPSInstr<R>>;
template<typename R> auto mips_lhu     = ALU::load_addr_aligned<BaseMIPSInstr<R>>;
template<typename R> auto mips_ll      = ALU::load_addr<BaseMIPSInstr<R>>;
template<typename R> auto mips_lui     = ALU::lui<BaseMIPSInstr<R>>;
template<typename R> auto mips_lw      = ALU::load_addr_aligned<BaseMIPSInstr<R>>;
template<typename R> auto mips_lwl     = ALU::load_addr_left32<BaseMIPSInstr<R>>;
template<typename R> auto mips_lwr     = ALU::load_addr_right32<BaseMIPSInstr<R>>;
template<typename R> auto mips_lwu     = ALU::load_addr_aligned<BaseMIPSInstr<R>>;
template<typename R> auto mips_madd    = ALU::multiplication<BaseMIPSInstr<R>, int32>;
template<typename R> auto mips_maddu   = ALU::multiplication<BaseMIPSInstr<R>, uint32>;
template<typename R> auto mips_mfhi    = ALU::move<BaseMIPSInstr<R>>;
template<typename R> auto mips_mflo    = ALU::move<BaseMIPSInstr<R>>;
template<typename R> auto mips_movn    = ALU::movn<BaseMIPSInstr<R>>;
template<typename R> auto mips_movz    = ALU::movz<BaseMIPSInstr<R>>;
template<typename R> auto mips_msub    = ALU::multiplication<BaseMIPSInstr<R>, int32>;
template<typename R> auto mips_msubu   = ALU::multiplication<BaseMIPSInstr<R>, uint32>;
template<typename R> auto mips_mthi    = ALU::move<BaseMIPSInstr<R>>;
template<typename R> auto mips_mtlo    = ALU::move<BaseMIPSInstr<R>>;
template<typename R> auto mips_mul     = ALU::multiplication<BaseMIPSInstr<R>, int32>;
template<typename R> auto mips_mult    = ALU::multiplication<BaseMIPSInstr<R>, int32>;
template<typename R> auto mips_multu   = ALU::multiplication<BaseMIPSInstr<R>, uint32>;
template<typename R> auto mips_nor     = ALU::nor<BaseMIPSInstr<R>>;
template<typename R> auto mips_or      = ALU::orv<BaseMIPSInstr<R>>;
template<typename R> auto mips_ori     = ALU::ori<BaseMIPSInstr<R>>;
template<typename R> auto mips_sb      = ALU::store_addr<BaseMIPSInstr<R>>;
template<typename R> auto mips_sc      = ALU::store_addr<BaseMIPSInstr<R>>;
template<typename R> auto mips_sd      = ALU::store_addr<BaseMIPSInstr<R>>;
template<typename R> auto mips_sdl     = ALU::store_addr<BaseMIPSInstr<R>>;
template<typename R> auto mips_sdr     = ALU::store_addr<BaseMIPSInstr<R>>;
template<typename R> auto mips_sh      = ALU::store_addr<BaseMIPSInstr<R>>;
template<typename R> auto mips_sll     = ALU::sll<BaseMIPSInstr<R>, uint32>;
template<typename R> auto mips_sllv    = ALU::sllv<BaseMIPSInstr<R>, uint32>;
template<typename R> auto mips_slt     = ALU::set<BaseMIPSInstr<R>, ALU::lt<BaseMIPSInstr<R>>>;
template<typename R> auto mips_slti    = ALU::set<BaseMIPSInstr<R>, ALU::lti<BaseMIPSInstr<R>>>;
template<typename R> auto mips_sltiu   = ALU::set<BaseMIPSInstr<R>, ALU::ltiu<BaseMIPSInstr<R>>>;
template<typename R> auto mips_sltu    = ALU::set<BaseMIPSInstr<R>, ALU::ltu<BaseMIPSInstr<R>>>;
template<typename R> auto mips_sra     = ALU::sra<BaseMIPSInstr<R>, uint32>;
template<typename R> auto mips_srav    = ALU::srav<BaseMIPSInstr<R>, uint32>;
template<typename R> auto mips_srl     = ALU::srl<BaseMIPSInstr<R>, uint32>;
template<typename R> auto mips_srlv    = ALU::srlv<BaseMIPSInstr<R>, uint32>;
template<typename R> auto mips_sub     = ALU::subtraction<BaseMIPSInstr<R>, int32>;
template<typename R> auto mips_subu    = ALU::subtraction<BaseMIPSInstr<R>, uint32>;
template<typename R> auto mips_sw      = ALU::store_addr_aligned<BaseMIPSInstr<R>>;
template<typename R> auto mips_swl     = ALU::store_addr_left32<BaseMIPSInstr<R>>;
template<typename R> auto mips_swr     = ALU::store_addr_right32<BaseMIPSInstr<R>>;
template<typename R> auto mips_syscall = ALU::syscall<BaseMIPSInstr<R>>;
template<typename R> auto mips_teq     = ALU::trap<BaseMIPSInstr<R>, ALU::eq<BaseMIPSInstr<R>>>;
template<typename R> auto mips_teqi    = ALU::trap<BaseMIPSInstr<R>, ALU::eqi<BaseMIPSInstr<R>>>;
template<typename R> auto mips_tge     = ALU::trap<BaseMIPSInstr<R>, ALU::ge<BaseMIPSInstr<R>>>;
template<typename R> auto mips_tgei    = ALU::trap<BaseMIPSInstr<R>, ALU::gei<BaseMIPSInstr<R>>>;
template<typename R> auto mips_tgeiu   = ALU::trap<BaseMIPSInstr<R>, ALU::geiu<BaseMIPSInstr<R>>>;
template<typename R> auto mips_tgeu    = ALU::trap<BaseMIPSInstr<R>, ALU::geu<BaseMIPSInstr<R>>>;
template<typename R> auto mips_tlt     = ALU::trap<BaseMIPSInstr<R>, ALU::lt<BaseMIPSInstr<R>>>;
template<typename R> auto mips_tlti    = ALU::trap<BaseMIPSInstr<R>, ALU::lti<BaseMIPSInstr<R>>>;
template<typename R> auto mips_tltiu   = ALU::trap<BaseMIPSInstr<R>, ALU::ltiu<BaseMIPSInstr<R>>>;
template<typename R> auto mips_tltu    = ALU::trap<BaseMIPSInstr<R>, ALU::ltu<BaseMIPSInstr<R>>>;
template<typename R> auto mips_tne     = ALU::trap<BaseMIPSInstr<R>, ALU::ne<BaseMIPSInstr<R>>>;
template<typename R> auto mips_tnei    = ALU::trap<BaseMIPSInstr<R>, ALU::nei<BaseMIPSInstr<R>>>;
template<typename R> auto mips_xor     = ALU::xorv<BaseMIPSInstr<R>>;
template<typename R> auto mips_xori    = ALU::xori<BaseMIPSInstr<R>>;

template<typename R>
using Table = std::unordered_map<uint32, MIPSTableEntry<R>>;

//unordered map for R-instructions
template<typename R>
static const Table<R> isaMapR =
{
    // **************** R INSTRUCTIONS ****************
    // Constant shifts
    //key    name  pointer       operation  ms  source1      source2      destination     mips version
    {0x0, { "sll",  mips_sll<R>,  OUT_R_SHAMT, 0, RegType::RT, RegType::ZERO, RegType::RD, MIPS_I_Instr} },
//  {0x1, { "movci"
    {0x2, { "srl",  mips_srl<R>,  OUT_R_SHAMT, 0, RegType::RT, RegType::ZERO, RegType::RD, MIPS_I_Instr} },
    {0x3, { "sra",  mips_sra<R>,  OUT_R_SHAMT, 0, RegType::RT, RegType::ZERO, RegType::RD, MIPS_I_Instr} },
    // Variable shifts
    {0x4, { "sllv", mips_sllv<R>, OUT_R_ARITHM, 0, RegType::RT, RegType::RS, RegType::RD, MIPS_I_Instr} },
//  {0x5
    {0x6, { "srlv", mips_srlv<R>, OUT_R_ARITHM, 0, RegType::RT, RegType::RS, RegType::RD, MIPS_I_Instr} },
    {0x7, { "srav", mips_srav<R>, OUT_R_ARITHM, 0, RegType::RT, RegType::RS, RegType::RD, MIPS_I_Instr} },
    // Indirect branches
    {0x8, { "jr",   mips_jr<R>  , OUT_R_JUMP, 0, RegType::RS, RegType::ZERO, RegType::ZERO, MIPS_I_Instr} },
    {0x9, { "jalr", mips_jalr<R>, OUT_R_JUMP, 0, RegType::RS, RegType::ZERO, RegType::RD,   MIPS_I_Instr} },
    // Conditional moves (MIPS IV)
    {0xA, { "movz", mips_movz<R>, OUT_R_CONDM, 0, RegType::RS, RegType::RT, RegType::RD, MIPS_IV_Instr | MIPS_32_Instr} },
    {0xB, { "movn", mips_movn<R>, OUT_R_CONDM, 0, RegType::RS, RegType::RT, RegType::RD, MIPS_IV_Instr | MIPS_32_Instr} },
    // System calls
    {0xC, { "syscall", mips_syscall<R>, OUT_R_SPECIAL, 0, RegType::ZERO, RegType::ZERO, RegType::ZERO, MIPS_I_Instr} },
    {0xD, { "break",   mips_break<R>,   OUT_R_SPECIAL, 0, RegType::ZERO, RegType::ZERO, RegType::ZERO, MIPS_I_Instr} },
    // Hi/Lo manipulators
    {0x10, { "mfhi", mips_mfhi<R>, OUT_R_ARITHM, 0, RegType::HI, RegType::ZERO, RegType::RD, MIPS_I_Instr} },
    {0x11, { "mthi", mips_mthi<R>, OUT_R_ARITHM, 0, RegType::RS, RegType::ZERO, RegType::HI, MIPS_I_Instr} },
    {0x12, { "mflo", mips_mflo<R>, OUT_R_ARITHM, 0, RegType::LO, RegType::ZERO, RegType::RD, MIPS_I_Instr} },
    {0x13, { "mtlo", mips_mtlo<R>, OUT_R_ARITHM, 0, RegType::RS, RegType::ZERO, RegType::LO, MIPS_I_Instr} },
    // Doubleword variable shifts
    {0x14, { "dsllv", mips_dsllv<R>, OUT_R_ARITHM, 0, RegType::RT, RegType::RS, RegType::RD, MIPS_III_Instr} },
    {0x16, { "dsrlv", mips_dsrlv<R>, OUT_R_ARITHM, 0, RegType::RT, RegType::RS, RegType::RD, MIPS_III_Instr} },
    {0x17, { "dsrav", mips_dsrav<R>, OUT_R_ARITHM, 0, RegType::RT, RegType::RS, RegType::RD, MIPS_III_Instr} },
    // Multiplication/Division
    {0x18, { "mult",   mips_mult<R>,  OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::HI_LO, MIPS_I_Instr} },
    {0x19, { "multu",  mips_multu<R>, OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::HI_LO, MIPS_I_Instr} },
    {0x1A, { "div",    mips_div<R>,   OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::HI_LO, MIPS_I_Instr} },
    {0x1B, { "divu",   mips_divu<R>,  OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::HI_LO, MIPS_I_Instr} },
    {0x1C, { "dmult",  mips_dmult<R>, OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::HI_LO, MIPS_III_Instr} },
    {0x1D, { "dmultu", mips_dmultu<R>,OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::HI_LO, MIPS_III_Instr} },
    {0x1E, { "ddiv",   mips_ddiv<R>,  OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::HI_LO, MIPS_III_Instr} },
    {0x1F, { "ddivu",  mips_ddivu<R>, OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::HI_LO, MIPS_III_Instr} },
    // Addition/Subtraction
    {0x20, { "add",  mips_add<R>,  OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, MIPS_I_Instr} },
    {0x21, { "addu", mips_addu<R>, OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, MIPS_I_Instr} },
    {0x22, { "sub",  mips_sub<R>,  OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, MIPS_I_Instr} },
    {0x23, { "subu", mips_subu<R>, OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, MIPS_I_Instr} },
    // Logical operations
    {0x24, { "and", mips_and<R>, OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, MIPS_I_Instr} },
    {0x25, { "or",  mips_or<R>,  OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, MIPS_I_Instr} },
    {0x26, { "xor", mips_xor<R>, OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, MIPS_I_Instr} },
    {0x27, { "nor", mips_nor<R>, OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, MIPS_I_Instr} },
//  {0x28
//  {0x29
    {0x2A, { "slt",   mips_slt<R>, OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, MIPS_I_Instr} },
    {0x2B, { "sltu", mips_sltu<R>, OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, MIPS_I_Instr} },
    // Doubleword addition/Subtraction
    {0x2C, { "dadd",  mips_dadd<R>,  OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, MIPS_I_Instr} },
    {0x2D, { "daddu", mips_daddu<R>, OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, MIPS_I_Instr} },
    {0x2E, { "dsub",  mips_dsub<R>,  OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, MIPS_III_Instr} },
    {0x2F, { "dsubu", mips_dsubu<R>, OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD, MIPS_III_Instr} },
    // Conditional traps (MIPS II)
    {0x30, { "tge",  mips_tge<R>,  OUT_R_TRAP, 0, RegType::RS, RegType::RT, RegType::ZERO, MIPS_II_Instr} },
    {0x31, { "tgeu", mips_tgeu<R>, OUT_R_TRAP, 0, RegType::RS, RegType::RT, RegType::ZERO, MIPS_II_Instr} },
    {0x32, { "tlt",  mips_tlt<R>,  OUT_R_TRAP, 0, RegType::RS, RegType::RT, RegType::ZERO, MIPS_II_Instr} },
    {0x33, { "tltu", mips_tltu<R>, OUT_R_TRAP, 0, RegType::RS, RegType::RT, RegType::ZERO, MIPS_II_Instr} },
    {0x34, { "teq",  mips_teq<R>,  OUT_R_TRAP, 0, RegType::RS, RegType::RT, RegType::ZERO, MIPS_II_Instr} },
//  {0x35
    {0x36, { "tne",  mips_tne<R>,  OUT_R_TRAP, 0, RegType::RS, RegType::RT, RegType::ZERO, MIPS_II_Instr} },
//  {0x37
    // Doubleword shifts
    {0x38, { "dsll",   mips_dsll<R>  , OUT_R_SHAMT, 0, RegType::RT, RegType::ZERO, RegType::RD, MIPS_III_Instr} },
    {0x3A, { "dsrl",   mips_dsrl<R>  , OUT_R_SHAMT, 0, RegType::RT, RegType::ZERO, RegType::RD, MIPS_III_Instr} },
    {0x3B, { "dsra",   mips_dsra<R>  , OUT_R_SHAMT, 0, RegType::RT, RegType::ZERO, RegType::RD, MIPS_III_Instr} },
    {0x3C, { "dsll32", mips_dsll32<R>, OUT_R_SHAMT, 0, RegType::RT, RegType::ZERO, RegType::RD, MIPS_III_Instr} },
    {0x3E, { "dsrl32", mips_dsrl32<R>, OUT_R_SHAMT, 0, RegType::RT, RegType::ZERO, RegType::RD, MIPS_III_Instr} },
    {0x3F, { "dsra32", mips_dsra32<R>, OUT_R_SHAMT, 0, RegType::RT, RegType::ZERO, RegType::RD, MIPS_III_Instr} }
};

//unordered map for RI-instructions
template<typename R>
static const Table<R> isaMapRI =
{
    // key     name    pointer        operation   memsize    source1        source2      destination  mips version
    // Branches
    {0x0,  { "bltz",  mips_bltz<R>,  OUT_I_BRANCH,  0, RegType::RS, RegType::ZERO, RegType::ZERO, MIPS_I_Instr} },
    {0x1,  { "bgez",  mips_bgez<R>,  OUT_I_BRANCH,  0, RegType::RS, RegType::ZERO, RegType::ZERO, MIPS_I_Instr} },
    {0x2,  { "bltzl", mips_bltzl<R>, OUT_I_BRANCH,  0, RegType::RS, RegType::ZERO, RegType::ZERO, MIPS_II_Instr} },
    {0x3,  { "bgezl", mips_bgezl<R>, OUT_I_BRANCH,  0, RegType::RS, RegType::ZERO, RegType::ZERO, MIPS_II_Instr} },
    // Conditional traps
    {0x8,  { "tgei",  mips_tgei<R>,  OUT_RI_TRAP, 0, RegType::RS, RegType::ZERO, RegType::ZERO, MIPS_II_Instr} },
    {0x9,  { "tgeiu", mips_tgeiu<R>, OUT_RI_TRAP, 0, RegType::RS, RegType::ZERO, RegType::ZERO, MIPS_II_Instr} },
    {0xA,  { "tlti",  mips_tlti<R>,  OUT_RI_TRAP, 0, RegType::RS, RegType::ZERO, RegType::ZERO, MIPS_II_Instr} },
    {0xB,  { "tltiu", mips_tltiu<R>, OUT_RI_TRAP, 0, RegType::RS, RegType::ZERO, RegType::ZERO, MIPS_II_Instr} },
    {0xC,  { "teqi",  mips_teqi<R>,  OUT_RI_TRAP, 0, RegType::RS, RegType::ZERO, RegType::ZERO, MIPS_II_Instr} },
    {0xE,  { "tnei",  mips_tnei<R>,  OUT_RI_TRAP, 0, RegType::RS, RegType::ZERO, RegType::ZERO, MIPS_II_Instr} },
    // Linking branches
    {0x10, { "bltzal",  mips_bltzal<R>,  OUT_I_BRANCH, 0, RegType::RS, RegType::ZERO, RegType::RA, MIPS_I_Instr} },
    {0x11, { "bgezal",  mips_bgezal<R>,  OUT_I_BRANCH, 0, RegType::RS, RegType::ZERO, RegType::RA, MIPS_I_Instr} },
    {0x12, { "bltzall", mips_bltzall<R>, OUT_I_BRANCH, 0, RegType::RS, RegType::ZERO, RegType::RA, MIPS_II_Instr} },
    {0x13, { "bgezall", mips_bgezall<R>, OUT_I_BRANCH, 0, RegType::RS, RegType::ZERO, RegType::RA, MIPS_II_Instr} }
};

//unordered map for I-instructions and J-instructions
template<typename R>
static const Table<R> isaMapIJ =
{
    // key     name    pointer  operation   memsize    source1        source2      destination  mips version
    // Direct jumps
    {0x2, { "j",   mips_j<R>,   OUT_J_JUMP, 0, RegType::ZERO, RegType::ZERO, RegType::ZERO, MIPS_I_Instr } },
    {0x3, { "jal", mips_jal<R>, OUT_J_JUMP, 0, RegType::ZERO, RegType::ZERO, RegType::RA, MIPS_I_Instr } },
    // Branches
    {0x4, { "beq",  mips_beq<R>,  OUT_I_BRANCH,    0, RegType::RS, RegType::RT,   RegType::ZERO, MIPS_I_Instr} },
    {0x5, { "bne",  mips_bne<R>,  OUT_I_BRANCH,    0, RegType::RS, RegType::RT,   RegType::ZERO, MIPS_I_Instr} },
    {0x6, { "blez", mips_blez<R>, OUT_I_BRANCH, 0, RegType::RS, RegType::ZERO, RegType::ZERO, MIPS_I_Instr} },
    {0x7, { "bgtz", mips_bgtz<R>, OUT_I_BRANCH, 0, RegType::RS, RegType::ZERO, RegType::ZERO, MIPS_I_Instr} },
    // Addition/Subtraction
    {0x8, { "addi",  mips_addi<R>,  OUT_I_ARITHM, 0, RegType::RS, RegType::ZERO, RegType::RT, MIPS_I_Instr} },
    {0x9, { "addiu", mips_addiu<R>, OUT_I_ARITHM, 0, RegType::RS, RegType::ZERO, RegType::RT, MIPS_I_Instr} },
    // Logical operations
    {0xA, { "slti",  mips_slti<R>,  OUT_I_ARITHM, 0, RegType::RS,   RegType::ZERO, RegType::RT, MIPS_I_Instr} },
    {0xB, { "sltiu", mips_sltiu<R>, OUT_I_ARITHM, 0, RegType::RS,   RegType::ZERO, RegType::RT, MIPS_I_Instr} },
    {0xC, { "andi",  mips_andi<R>,  OUT_I_ARITHM, 0, RegType::RS,   RegType::ZERO, RegType::RT, MIPS_I_Instr} },
    {0xD, { "ori",   mips_ori<R>,   OUT_I_ARITHM, 0, RegType::RS,   RegType::ZERO, RegType::RT, MIPS_I_Instr} },
    {0xE, { "xori",  mips_xori<R>,  OUT_I_ARITHM, 0, RegType::RS,   RegType::ZERO, RegType::RT, MIPS_I_Instr} },
    {0xF, { "lui",   mips_lui<R>,   OUT_I_ARITHM, 0, RegType::ZERO, RegType::ZERO, RegType::RT, MIPS_I_Instr} },
    // 0x10 - 0x13 coprocessor operations
    // Likely branches (MIPS II)
    {0x14, { "beql",  mips_beql<R>,  OUT_I_BRANCH, 0, RegType::RS, RegType::RT,   RegType::ZERO, MIPS_II_Instr} },
    {0x15, { "bnel",  mips_bnel<R>,  OUT_I_BRANCH, 0, RegType::RS, RegType::RT,   RegType::ZERO, MIPS_II_Instr} },
    {0x16, { "blezl", mips_blezl<R>, OUT_I_BRANCH, 0, RegType::RS, RegType::RT,   RegType::ZERO, MIPS_II_Instr} },
    {0x17, { "bgtzl", mips_bgtzl<R>, OUT_I_BRANCH, 0, RegType::RS, RegType::RT,   RegType::ZERO, MIPS_II_Instr} },
    // Doubleword unaligned loads
    {0x1A, { "ldl", mips_ldl<R>,  OUT_I_LOAD, 8, RegType::RS, RegType::ZERO, RegType::RT, MIPS_III_Instr} },
    {0x1B, { "ldr", mips_ldr<R>,  OUT_I_LOAD, 8, RegType::RS, RegType::ZERO, RegType::RT, MIPS_III_Instr} },
    // Doubleword addition
    {0x18, { "daddi",  mips_daddi<R>,  OUT_I_ARITHM, 0, RegType::RS, RegType::ZERO, RegType::RT, MIPS_III_Instr} },
    {0x19, { "daddiu", mips_daddiu<R>, OUT_I_ARITHM, 0, RegType::RS, RegType::ZERO, RegType::RT, MIPS_III_Instr} },
    // Loads
    {0x20, { "lb",  mips_lb<R>,  OUT_I_LOAD,         1, RegType::RS, RegType::ZERO, RegType::RT, MIPS_I_Instr} },
    {0x21, { "lh",  mips_lh<R>,  OUT_I_LOAD,         2, RegType::RS, RegType::ZERO, RegType::RT, MIPS_I_Instr} },
    {0x22, { "lwl", mips_lwl<R>, OUT_I_PARTIAL_LOAD, 4, RegType::RS, RegType::ZERO, RegType::RT, MIPS_I_Instr} },
    {0x23, { "lw",  mips_lw<R>,  OUT_I_LOAD,         4, RegType::RS, RegType::ZERO, RegType::RT, MIPS_I_Instr} },
    {0x24, { "lbu", mips_lbu<R>, OUT_I_LOADU,        1, RegType::RS, RegType::ZERO, RegType::RT, MIPS_I_Instr} },
    {0x25, { "lhu", mips_lhu<R>, OUT_I_LOADU,        2, RegType::RS, RegType::ZERO, RegType::RT, MIPS_I_Instr} },
    {0x26, { "lwr", mips_lwr<R>, OUT_I_PARTIAL_LOAD, 4, RegType::RS, RegType::ZERO, RegType::RT, MIPS_I_Instr} },
    {0x27, { "lwu", mips_lwu<R>, OUT_I_LOADU,        4, RegType::RS, RegType::ZERO, RegType::RT, MIPS_I_Instr} },
    // Stores
    {0x28, { "sb",  mips_sb<R>,   OUT_I_STORE, 1, RegType::RS, RegType::RT, RegType::ZERO, MIPS_I_Instr} },
    {0x29, { "sh",  mips_sh<R>,   OUT_I_STORE, 2, RegType::RS, RegType::RT, RegType::ZERO, MIPS_I_Instr} },
    {0x2A, { "swl", mips_swl<R>, OUT_I_STORE, 4, RegType::RS, RegType::RT, RegType::ZERO, MIPS_I_Instr} },
    {0x2B, { "sw",  mips_sw<R>,   OUT_I_STORE, 4, RegType::RS, RegType::RT, RegType::ZERO, MIPS_I_Instr} },
    {0x2C, { "sdl", mips_sdl<R>, OUT_I_STORE, 8, RegType::RS, RegType::RT, RegType::ZERO, MIPS_III_Instr} },
    {0x2D, { "sdr", mips_sdr<R>, OUT_I_STORE, 8, RegType::RS, RegType::RT, RegType::ZERO, MIPS_III_Instr} },
    {0x2E, { "swr", mips_swr<R>, OUT_I_STORE, 4, RegType::RS, RegType::RT, RegType::ZERO, MIPS_I_Instr} },
//  {0x2F, { "cache"
    // Advanced loads and stores
    {0x30, { "ll", mips_ll<R>,  OUT_I_LOAD,   2, RegType::RS, RegType::ZERO, RegType::RT, MIPS_I_Instr} },
    {0x37, { "ld", mips_ld<R>,  OUT_I_LOAD,   8, RegType::RS, RegType::ZERO, RegType::RT, MIPS_III_Instr} },
    {0x38, { "sc", mips_sc<R>,  OUT_I_STORE,  2, RegType::RS, RegType::RT, RegType::ZERO, MIPS_I_Instr} },
    {0x3F, { "sd", mips_sd<R>,  OUT_I_STORE,  8, RegType::RS, RegType::RT, RegType::ZERO, MIPS_III_Instr} }
};

template<typename R>
static const Table<R> isaMapMIPS32 =
{
    // key     name    pointer  operation   memsize    source1        source2      destination  mips version
    // Advanced multiplication
    {0x00, { "madd",  mips_madd<R>,  OUT_R_ACCUM,  0, RegType::RS, RegType::RT, RegType::HI_LO, MIPS_32_Instr} },
    {0x01, { "maddu", mips_maddu<R>, OUT_R_ACCUM,  0, RegType::RS, RegType::RT, RegType::HI_LO, MIPS_32_Instr} },
    {0x02, { "mul",   mips_mul<R>,   OUT_R_ARITHM, 0, RegType::RS, RegType::RT, RegType::RD,    MIPS_32_Instr} },
    {0x04, { "msub",  mips_msub<R>,  OUT_R_SUBTR,  0, RegType::RS, RegType::RT, RegType::HI_LO, MIPS_32_Instr} },
    {0x05, { "msubu", mips_msubu<R>, OUT_R_SUBTR,  0, RegType::RS, RegType::RT, RegType::HI_LO, MIPS_32_Instr} },
    // Count leading zeroes/ones
    {0x20, { "clz",  mips_clz<R>,  OUT_R_ARITHM, 0, RegType::RS, RegType::ZERO, RegType::RD, MIPS_32_Instr} },
    {0x21, { "clo",  mips_clo<R>,  OUT_R_ARITHM, 0, RegType::RS, RegType::ZERO, RegType::RD, MIPS_32_Instr} },
    {0x24, { "dclz", mips_dclz<R>, OUT_R_ARITHM, 0, RegType::RS, RegType::ZERO, RegType::RD, MIPS_64_Instr} },
    {0x25, { "dclo", mips_dclo<R>, OUT_R_ARITHM, 0, RegType::RS, RegType::ZERO, RegType::RD, MIPS_64_Instr} }
};

template<typename R>
MIPSTableEntry<R> unknown_instruction =
{ "Unknown instruction", unknown_mips_instruction, OUT_R_SPECIAL, 0, RegType::ZERO, RegType::ZERO, RegType::ZERO, MIPS_I_Instr};

template<typename R>
MIPSTableEntry<R> nop =
{ "nop" , mips_sll<R>, OUT_R_SPECIAL, 0, RegType::ZERO, RegType::ZERO, RegType::ZERO, MIPS_I_Instr};

template<typename R>
const MIPSTableEntry<R>& get_table_entry( const Table<R>& table, uint32 key)
{
    auto it = table.find( key);
    return it == table.end() ? unknown_instruction<R> : it->second;
}

template<typename R>
const MIPSTableEntry<R>& get_table_entry( uint32 bytes)
{
    MIPSInstrDecoder instr( bytes);

    if ( instr.bytes == 0)
        return nop<R>;

    switch ( instr.opcode)
    {
        case 0x0:  return get_table_entry( isaMapR<R>,      instr.funct);  // R instruction
        case 0x1:  return get_table_entry( isaMapRI<R>,     instr.rt);     // RegIMM instruction
        case 0x1C: return get_table_entry( isaMapMIPS32<R>, instr.funct);  // MIPS32 instruction
        default:   return get_table_entry( isaMapIJ<R>,     instr.opcode); // I and J instructions
    }
}

template<typename M>
auto find_entry( const M& map, std::string_view name)
{
    return std::find_if( map.begin(), map.end(), [name]( const auto& e) {
        return e.second.name == name;
    });
}

template<typename R>
const MIPSTableEntry<R>& get_table_entry( std::string_view str_opcode)
{
    if ( str_opcode == "nop")
        return nop<R>;

    for ( const auto& map : { isaMapR<R>, isaMapRI<R>, isaMapMIPS32<R>, isaMapIJ<R> })
    {
        auto res = find_entry( map, str_opcode);
        if ( res != map.end())
            return res->second;
    }

    return unknown_instruction<R>;
}

template<typename R>
BaseMIPSInstr<R>::BaseMIPSInstr( MIPSVersion version, uint32 bytes, Addr PC) :
    raw( bytes),
    new_PC( PC + 4),
    PC( PC)
{
    init( get_table_entry<R>( raw), version);
}

template<typename R>
BaseMIPSInstr<R>::BaseMIPSInstr( MIPSVersion version, std::string_view str_opcode, Addr PC)
    : raw( 0)
    , new_PC( PC + 4)
    , PC( PC)
{
    init( get_table_entry<R>( str_opcode), version);
}

template<typename R>
void BaseMIPSInstr<R>::init( const MIPSTableEntry<R>& entry, MIPSVersion version)
{
    MIPSInstrDecoder instr( raw);

    operation = entry.operation;
    mem_size  = entry.mem_size;
    execute_function = entry.versions.is_supported(version)
        ? entry.function
        : unknown_mips_instruction<BaseMIPSInstr<R>>;

    shamt = instr.shamt;

    src1 = get_register( instr, entry.src1);
    src2 = get_register( instr, entry.src2);
    dst  = get_register( instr, entry.dst);
    if ( entry.dst == RegType::HI_LO)
        dst2 = MIPSRegister::mips_hi;

    v_imm = get_immediate( instr, operation);

    generate_disasm( entry);
}

template<typename R>
void BaseMIPSInstr<R>::generate_disasm( const MIPSTableEntry<R>& entry)
{
    const bool print_dst  = is_explicit_register( entry.dst);
    const bool print_src1 = is_explicit_register( entry.src1);
    const bool print_src2 = is_explicit_register( entry.src2);

    std::ostringstream oss;
    if ( PC != 0)
        oss << std::hex << "0x" << PC << ": ";
    oss << entry.name;

    switch ( operation)
    {
        case OUT_R_SHAMT:
            oss <<  " $" << dst
                << ", $" << src1
                <<  ", " << std::dec << shamt;
            break;
        case OUT_I_ARITHM:
            oss << " $" << dst;
            if ( print_src1)
               oss  << ", $" << src1;
            oss << ", 0x" << std::hex << v_imm << std::dec;
            break;
        case OUT_I_BRANCH:
            oss << " $" << src1;
            if ( print_src2)
                oss << ", $" << src2;
            oss << ", " << std::dec << narrow_cast<int16>(v_imm);
            break;
        case OUT_RI_TRAP:
            oss << " $" << src1 << ", 0x"
                << std::hex << narrow_cast<int16>(v_imm) << std::dec;
            break;
        case OUT_I_LOAD:
        case OUT_I_LOADU:
        case OUT_I_PARTIAL_LOAD:
            oss << " $" << dst << ", 0x"
                << std::hex << v_imm
                << "($" << src1 << ")" << std::dec;
            break;

        case OUT_I_STORE:
            oss << " $" << src2 << ", 0x"
                << std::hex << v_imm
                << "($" << src1 << ")" << std::dec;
            break;
        case OUT_J_JUMP:
            oss << " 0x" << std::hex << v_imm << std::dec;
            break;
        default:
            if (print_dst) {
                oss <<  " $" << dst;
                if (print_src1)
                    oss << ",";
            }
            if (print_src1)
                oss <<  " $" << src1;
            if (print_src2)
                oss << ", $" << src2;
            break;
    }

    disasm = oss.str();
}

template class BaseMIPSInstr<uint32>;
template class BaseMIPSInstr<uint64>;
