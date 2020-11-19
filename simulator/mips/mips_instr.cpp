/**
 * mips_instr.cpp - instruction decoder for mips
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#include "mips_instr.h"
#include "mips_instr_decode.h"

#include <func_sim/alu.h>
#include <infra/instrcache/instr_cache.h>
#include <infra/macro.h>
#include <infra/types.h>

#include <iomanip>
#include <iostream>
#include <unordered_map>
#include <vector>

/*  Reducing number of ALU instantiations. ALU modifies
 * only Datapath, so we do not need different instantiations
 * for RISCV and MIPS, if register sizes are the same */
template <typename I>
using MIPSALU = ALU<Datapath<typename I::RegisterUInt>>;

/*  Note. Error "expected primary-expression before '...'"
 * on GCC means that you forgot temlate keyword after RISCVALU<I>
 * while refering to template function " */

template<typename I> void do_nothing(I* /* instr */) { }
template<typename I> const auto mips_add     = MIPSALU<I>::template addition_overflow<uint32>;
template<typename I> const auto mips_addi    = MIPSALU<I>::template addition_overflow_imm<uint32>;
template<typename I> const auto mips_addiu   = MIPSALU<I>::template addition_imm<uint32>;
template<typename I> const auto mips_addu    = MIPSALU<I>::template addition<uint32>;
template<typename I> const auto mips_and     = MIPSALU<I>::andv;
template<typename I> const auto mips_andi    = MIPSALU<I>::andi;
template<typename I> const auto mips_beq     = MIPSALU<I>::template branch<MIPSALU<I>::eq>;
template<typename I> const auto mips_beql    = MIPSALU<I>::template branch<MIPSALU<I>::eq>;
template<typename I> const auto mips_bgez    = MIPSALU<I>::template branch<MIPSALU<I>::gez>;
template<typename I> const auto mips_bgezal  = MIPSALU<I>::template branch_and_link<MIPSALU<I>::gez>;
template<typename I> const auto mips_bgezall = MIPSALU<I>::template branch_and_link<MIPSALU<I>::gez>;
template<typename I> const auto mips_bgezl   = MIPSALU<I>::template branch<MIPSALU<I>::gez>;
template<typename I> const auto mips_bgtz    = MIPSALU<I>::template branch<MIPSALU<I>::gtz>;
template<typename I> const auto mips_bgtzl   = MIPSALU<I>::template branch<MIPSALU<I>::gtz>;
template<typename I> const auto mips_blez    = MIPSALU<I>::template branch<MIPSALU<I>::lez>;
template<typename I> const auto mips_blezl   = MIPSALU<I>::template branch<MIPSALU<I>::lez>;
template<typename I> const auto mips_bltz    = MIPSALU<I>::template branch<MIPSALU<I>::ltz>;
template<typename I> const auto mips_bltzal  = MIPSALU<I>::template branch_and_link<MIPSALU<I>::ltz>;
template<typename I> const auto mips_bltzall = MIPSALU<I>::template branch_and_link<MIPSALU<I>::ltz>;
template<typename I> const auto mips_bltzl   = MIPSALU<I>::template branch<MIPSALU<I>::ltz>;
template<typename I> const auto mips_bne     = MIPSALU<I>::template branch<MIPSALU<I>::ne>;
template<typename I> const auto mips_bnel    = MIPSALU<I>::template branch<MIPSALU<I>::ne>;
template<typename I> const auto mips_break   = MIPSALU<I>::breakpoint;
template<typename I> const auto mips_clo     = MIPSALU<I>::template clo<uint32>;
template<typename I> const auto mips_clz     = MIPSALU<I>::template clz<uint32>;
template<typename I> const auto mips_dadd    = MIPSALU<I>::template addition_overflow<uint64>;
template<typename I> const auto mips_daddi   = MIPSALU<I>::template addition_overflow_imm<uint64>;
template<typename I> const auto mips_daddiu  = MIPSALU<I>::template addition_imm<uint64>;
template<typename I> const auto mips_daddu   = MIPSALU<I>::template addition<uint64>;
template<typename I> const auto mips_dclo    = MIPSALU<I>::template clo<uint64>;
template<typename I> const auto mips_dclz    = MIPSALU<I>::template clz<uint64>;
template<typename I> const auto mips_dsll    = MIPSALU<I>::template sll<uint64>;
template<typename I> const auto mips_dsll32  = MIPSALU<I>::dsll32;
template<typename I> const auto mips_dsllv   = MIPSALU<I>::template sllv<uint64>;
template<typename I> const auto mips_dsra    = MIPSALU<I>::template sra<uint64>;
template<typename I> const auto mips_dsra32  = MIPSALU<I>::dsra32;
template<typename I> const auto mips_dsrav   = MIPSALU<I>::template srav<uint64>;
template<typename I> const auto mips_dsrl    = MIPSALU<I>::template srl<uint64>;
template<typename I> const auto mips_dsrl32  = MIPSALU<I>::dsrl32;
template<typename I> const auto mips_dsrlv   = MIPSALU<I>::template srlv<uint64>;
template<typename I> const auto mips_dsub    = MIPSALU<I>::template subtraction_overflow<uint64>;
template<typename I> const auto mips_dsubu   = MIPSALU<I>::template subtraction<uint64>;
template<typename I> const auto mips_eret    = MIPSALU<I>::eret;
template<typename I> const auto mips_j       = MIPSALU<I>::j;
template<typename I> const auto mips_jal     = MIPSALU<I>::template jump_and_link<MIPSALU<I>::j>;
template<typename I> const auto mips_jalr    = MIPSALU<I>::template jump_and_link<MIPSALU<I>::jr>;
template<typename I> const auto mips_jr      = MIPSALU<I>::jr;
template<typename I> const auto mips_lb      = MIPSALU<I>::load_addr;
template<typename I> const auto mips_lbu     = MIPSALU<I>::load_addr;
template<typename I> const auto mips_ld      = MIPSALU<I>::load_addr_aligned;
template<typename I> const auto mips_ldl     = MIPSALU<I>::load_addr;
template<typename I> const auto mips_ldr     = MIPSALU<I>::load_addr;
template<typename I> const auto mips_lh      = MIPSALU<I>::load_addr_aligned;
template<typename I> const auto mips_lhu     = MIPSALU<I>::load_addr_aligned;
template<typename I> const auto mips_ll      = MIPSALU<I>::load_addr_aligned;
template<typename I> const auto mips_lui     = MIPSALU<I>::template upper_immediate<16>;
template<typename I> const auto mips_lw      = MIPSALU<I>::load_addr_aligned;
template<typename I> const auto mips_lwl     = MIPSALU<I>::load_addr_left32;
template<typename I> const auto mips_lwr     = MIPSALU<I>::load_addr_right32;
template<typename I> const auto mips_lwu     = MIPSALU<I>::load_addr_aligned;
template<typename I> const auto mips_mfc0    = MIPSALU<I>::move;
template<typename I> const auto mips_mfhi    = MIPSALU<I>::move;
template<typename I> const auto mips_mflo    = MIPSALU<I>::move;
template<typename I> const auto mips_movn    = MIPSALU<I>::movn;
template<typename I> const auto mips_movz    = MIPSALU<I>::movz;
template<typename I> const auto mips_mtc0    = MIPSALU<I>::move;
template<typename I> const auto mips_mthi    = MIPSALU<I>::move;
template<typename I> const auto mips_mtlo    = MIPSALU<I>::move;
template<typename I> const auto mips_nor     = MIPSALU<I>::nor;
template<typename I> const auto mips_or      = MIPSALU<I>::orv;
template<typename I> const auto mips_ori     = MIPSALU<I>::ori;
template<typename I> const auto mips_sb      = MIPSALU<I>::store_addr;
template<typename I> const auto mips_sc      = MIPSALU<I>::store_addr_aligned;
template<typename I> const auto mips_sd      = MIPSALU<I>::store_addr_aligned;
template<typename I> const auto mips_sdl     = MIPSALU<I>::store_addr;
template<typename I> const auto mips_sdr     = MIPSALU<I>::store_addr;
template<typename I> const auto mips_sh      = MIPSALU<I>::store_addr_aligned;
template<typename I> const auto mips_sll     = MIPSALU<I>::template sll<uint32>;
template<typename I> const auto mips_sllv    = MIPSALU<I>::template sllv<uint32>;
template<typename I> const auto mips_slt     = MIPSALU<I>::template set<MIPSALU<I>::lt>;
template<typename I> const auto mips_slti    = MIPSALU<I>::template set<MIPSALU<I>::lti>;
template<typename I> const auto mips_sltiu   = MIPSALU<I>::template set<MIPSALU<I>::ltiu>;
template<typename I> const auto mips_sltu    = MIPSALU<I>::template set<MIPSALU<I>::ltu>;
template<typename I> const auto mips_sra     = MIPSALU<I>::template sra<uint32>;
template<typename I> const auto mips_srav    = MIPSALU<I>::template srav<uint32>;
template<typename I> const auto mips_srl     = MIPSALU<I>::template srl<uint32>;
template<typename I> const auto mips_srlv    = MIPSALU<I>::template srlv<uint32>;
template<typename I> const auto mips_sub     = MIPSALU<I>::template subtraction_overflow<uint32>;
template<typename I> const auto mips_subu    = MIPSALU<I>::template subtraction<uint32>;
template<typename I> const auto mips_sw      = MIPSALU<I>::store_addr_aligned;
template<typename I> const auto mips_swl     = MIPSALU<I>::store_addr_left32;
template<typename I> const auto mips_swr     = MIPSALU<I>::store_addr_right32;
template<typename I> const auto mips_syscall = MIPSALU<I>::syscall;
template<typename I> const auto mips_teq     = MIPSALU<I>::template trap<MIPSALU<I>::eq>;
template<typename I> const auto mips_teqi    = MIPSALU<I>::template trap<MIPSALU<I>::eqi>;
template<typename I> const auto mips_tge     = MIPSALU<I>::template trap<MIPSALU<I>::ge>;
template<typename I> const auto mips_tgei    = MIPSALU<I>::template trap<MIPSALU<I>::gei>;
template<typename I> const auto mips_tgeiu   = MIPSALU<I>::template trap<MIPSALU<I>::geiu>;
template<typename I> const auto mips_tgeu    = MIPSALU<I>::template trap<MIPSALU<I>::geu>;
template<typename I> const auto mips_tlt     = MIPSALU<I>::template trap<MIPSALU<I>::lt>;
template<typename I> const auto mips_tlti    = MIPSALU<I>::template trap<MIPSALU<I>::lti>;
template<typename I> const auto mips_tltiu   = MIPSALU<I>::template trap<MIPSALU<I>::ltiu>;
template<typename I> const auto mips_tltu    = MIPSALU<I>::template trap<MIPSALU<I>::ltu>;
template<typename I> const auto mips_tne     = MIPSALU<I>::template trap<MIPSALU<I>::ne>;
template<typename I> const auto mips_tnei    = MIPSALU<I>::template trap<MIPSALU<I>::nei>;
template<typename I> const auto mips_xor     = MIPSALU<I>::xorv;
template<typename I> const auto mips_xori    = MIPSALU<I>::xori;
template<typename I> const auto mips_unknown = MIPSALU<I>::unknown_instruction;

// Multiplicate/Divide instructions
template<typename I> const auto mips_madd    = MIPSMultALU<I>::template multiplication<int32>;
template<typename I> const auto mips_maddu   = MIPSMultALU<I>::template multiplication<uint32>;
template<typename I> const auto mips_msub    = MIPSMultALU<I>::template multiplication<int32>;
template<typename I> const auto mips_msubu   = MIPSMultALU<I>::template multiplication<uint32>;
template<typename I> const auto mips_dmult   = MIPSMultALU<I>::template multiplication<int64>;
template<typename I> const auto mips_dmultu  = MIPSMultALU<I>::template multiplication<uint64>;
template<typename I> const auto mips_mul     = MIPSMultALU<I>::template multiplication<int32>;
template<typename I> const auto mips_mult    = MIPSMultALU<I>::template multiplication<int32>;
template<typename I> const auto mips_multu   = MIPSMultALU<I>::template multiplication<uint32>;
template<typename I> const auto mips_ddiv    = MIPSMultALU<I>::template division<int64>;
template<typename I> const auto mips_ddivu   = MIPSMultALU<I>::template division<uint64>;
template<typename I> const auto mips_div     = MIPSMultALU<I>::template division<int32>;
template<typename I> const auto mips_divu    = MIPSMultALU<I>::template division<uint32>;

// CP1 instructions
template<typename I> const auto mips_abs_d     = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_abs_s     = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_add_d     = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_add_s     = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_bc1f      = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_bc1t      = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_bc1fl     = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_bc1tl     = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_c_f_d     = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_c_f_s     = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_c_un_d    = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_c_un_s    = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_c_eq_d    = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_c_eq_s    = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_c_ueq_d   = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_c_ueq_s   = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_c_olt_d   = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_c_olt_s   = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_c_ult_d   = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_c_ult_s   = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_c_ole_d   = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_c_ole_s   = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_c_ule_d   = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_c_ule_s   = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_c_sf_d    = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_c_sf_s    = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_c_ngle_d  = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_c_ngle_s  = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_c_seq_d   = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_c_seq_s   = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_c_ngl_d   = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_c_ngl_s   = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_c_lt_d    = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_c_lt_s    = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_c_nge_d   = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_c_nge_s   = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_c_le_d    = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_c_le_s    = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_c_ngt_d   = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_c_ngt_s   = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_ceil_l_d  = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_ceil_l_s  = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_ceil_w_d  = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_ceil_w_s  = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_cfc1      = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_ctc1      = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_cvt_d_l   = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_cvt_d_s   = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_cvt_d_w   = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_cvt_s_d   = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_cvt_s_l   = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_cvt_s_w   = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_cvt_l_d   = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_cvt_l_s   = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_cvt_w_d   = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_cvt_w_s   = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_div_d     = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_div_s     = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_dmfc1     = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_dmtc1     = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_floor_l_d = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_floor_l_s = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_floor_w_d = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_floor_w_s = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_ldc1      = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_lwc1      = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_ldxc1     = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_lwxc1     = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_mfc1      = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_madd_d    = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_madd_s    = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_mov_d     = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_mov_s     = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_movf      = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_movf_d    = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_movf_s    = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_movn_d    = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_movn_s    = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_movt      = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_movt_d    = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_movt_s    = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_movz_d    = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_movz_s    = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_msub_d    = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_msub_s    = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_mtc1      = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_mul_d     = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_mul_s     = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_neg_d     = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_neg_s     = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_nmadd_d   = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_nmadd_s   = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_nmsub_d   = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_nmsub_s   = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_recip_d   = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_recip_s   = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_round_l_d = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_round_l_s = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_round_w_d = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_round_w_s = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_rsqrt_d   = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_rsqrt_s   = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_sdc1      = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_sdxc1     = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_sqrt_d    = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_sqrt_s    = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_sub_d     = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_sub_s     = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_swc1      = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_swxc1     = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_trunc_l_d = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_trunc_l_s = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_trunc_w_d = MIPSALU<I>::unknown_instruction;
template<typename I> const auto mips_trunc_w_s = MIPSALU<I>::unknown_instruction;

template<typename I>
struct MIPSTableEntry
{
	using Execute = typename MIPSALU<I>::Execute;

    std::string_view name = "Unknown instruction";
    Execute function = mips_unknown<I>;
    OperationType operation = OUT_ARITHM;
    uint8 mem_size = 0;
    char imm_type = 'N';
    Imm imm_print_type = Imm::NO;
    std::vector<Src> src = { };
    std::vector<Dst> dst = { Dst::ZERO };
    MIPSVersionMask versions = MIPS_I_Instr;
};

template<typename R>
using Table = std::unordered_map<uint32, MIPSTableEntry<R>>;

//unordered map for R-instructions
template<typename I>
static const Table<I> isaMapR =
{
    // **************** R INSTRUCTIONS ****************
    // Constant shifts
    {0x0, { "sll",  mips_sll<I>,  OUT_ARITHM, 0, 'S', Imm::SHIFT, { Src::RT }, { Dst::RD }, MIPS_I_Instr} },
//  {0x1, { "movci"
    {0x2, { "srl",  mips_srl<I>,  OUT_ARITHM, 0, 'S', Imm::SHIFT, { Src::RT }, { Dst::RD }, MIPS_I_Instr} },
    {0x3, { "sra",  mips_sra<I>,  OUT_ARITHM, 0, 'S', Imm::SHIFT, { Src::RT }, { Dst::RD }, MIPS_I_Instr} },
    // Variable shifts
    {0x4, { "sllv", mips_sllv<I>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::RT, Src::RS }, { Dst::RD }, MIPS_I_Instr} },
//  {0x5
    {0x6, { "srlv", mips_srlv<I>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::RT, Src::RS }, { Dst::RD }, MIPS_I_Instr} },
    {0x7, { "srav", mips_srav<I>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::RT, Src::RS }, { Dst::RD }, MIPS_I_Instr} },
    // Indirect branches
    {0x8, { "jr",   mips_jr<I>  , OUT_R_JUMP, 0, 'N', Imm::NO, { Src::RS }, { Dst::ZERO }, MIPS_I_Instr} },
    {0x9, { "jalr", mips_jalr<I>, OUT_R_JUMP, 0, 'N', Imm::NO, { Src::RS }, { Dst::RD },   MIPS_I_Instr} },
    // Conditional moves (MIPS IV)
    {0xA, { "movz", mips_movz<I>, OUT_R_CONDM, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::RD }, MIPS_IV_Instr | MIPS_32_Instr} },
    {0xB, { "movn", mips_movn<I>, OUT_R_CONDM, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::RD }, MIPS_IV_Instr | MIPS_32_Instr} },
    // System calls
    {0xC, { "syscall", mips_syscall<I>, OUT_SYSCALL, 0, 'N', Imm::NO, { }, { Dst::ZERO }, MIPS_I_Instr} },
    {0xD, { "break",   mips_break<I>,   OUT_BREAK,   0, 'N', Imm::NO, { }, { Dst::ZERO }, MIPS_I_Instr} },
    // Hi/Lo manipulators
    {0x10, { "mfhi", mips_mfhi<I>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::HI }, { Dst::RD }, MIPS_I_Instr} },
    {0x11, { "mthi", mips_mthi<I>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS }, { Dst::HI }, MIPS_I_Instr} },
    {0x12, { "mflo", mips_mflo<I>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::LO }, { Dst::RD }, MIPS_I_Instr} },
    {0x13, { "mtlo", mips_mtlo<I>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS }, { Dst::LO }, MIPS_I_Instr} },
    // Doubleword variable shifts
    {0x14, { "dsllv", mips_dsllv<I>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::RT, Src::RS }, { Dst::RD }, MIPS_III_Instr} },
    {0x16, { "dsrlv", mips_dsrlv<I>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::RT, Src::RS }, { Dst::RD }, MIPS_III_Instr} },
    {0x17, { "dsrav", mips_dsrav<I>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::RT, Src::RS }, { Dst::RD }, MIPS_III_Instr} },
    // Multiplication/Division
    {0x18, { "mult",   mips_mult<I>,  OUT_DIVMULT, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::LO, Dst::HI }, MIPS_I_Instr} },
    {0x19, { "multu",  mips_multu<I>, OUT_DIVMULT, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::LO, Dst::HI }, MIPS_I_Instr} },
    {0x1A, { "div",    mips_div<I>,   OUT_DIVMULT, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::LO, Dst::HI }, MIPS_I_Instr} },
    {0x1B, { "divu",   mips_divu<I>,  OUT_DIVMULT, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::LO, Dst::HI }, MIPS_I_Instr} },
    {0x1C, { "dmult",  mips_dmult<I>, OUT_DIVMULT, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::LO, Dst::HI }, MIPS_III_Instr} },
    {0x1D, { "dmultu", mips_dmultu<I>,OUT_DIVMULT, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::LO, Dst::HI }, MIPS_III_Instr} },
    {0x1E, { "ddiv",   mips_ddiv<I>,  OUT_DIVMULT, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::LO, Dst::HI }, MIPS_III_Instr} },
    {0x1F, { "ddivu",  mips_ddivu<I>, OUT_DIVMULT, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::LO, Dst::HI }, MIPS_III_Instr} },
    // Addition/Subtraction
    {0x20, { "add",  mips_add<I>,  OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::RD }, MIPS_I_Instr} },
    {0x21, { "addu", mips_addu<I>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::RD }, MIPS_I_Instr} },
    {0x22, { "sub",  mips_sub<I>,  OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::RD }, MIPS_I_Instr} },
    {0x23, { "subu", mips_subu<I>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::RD }, MIPS_I_Instr} },
    // Logical operations
    {0x24, { "and", mips_and<I>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::RD }, MIPS_I_Instr} },
    {0x25, { "or",  mips_or<I>,  OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::RD }, MIPS_I_Instr} },
    {0x26, { "xor", mips_xor<I>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::RD }, MIPS_I_Instr} },
    {0x27, { "nor", mips_nor<I>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::RD }, MIPS_I_Instr} },
//  {0x28
//  {0x29
    {0x2A, { "slt",  mips_slt<I>,  OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::RD }, MIPS_I_Instr} },
    {0x2B, { "sltu", mips_sltu<I>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::RD }, MIPS_I_Instr} },
    // Doubleword addition/Subtraction
    {0x2C, { "dadd",  mips_dadd<I>,  OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::RD }, MIPS_III_Instr} },
    {0x2D, { "daddu", mips_daddu<I>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::RD }, MIPS_III_Instr} },
    {0x2E, { "dsub",  mips_dsub<I>,  OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::RD }, MIPS_III_Instr} },
    {0x2F, { "dsubu", mips_dsubu<I>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::RD }, MIPS_III_Instr} },
    // Conditional traps (MIPS II)
    {0x30, { "tge",  mips_tge<I>,  OUT_TRAP, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_II_Instr} },
    {0x31, { "tgeu", mips_tgeu<I>, OUT_TRAP, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_II_Instr} },
    {0x32, { "tlt",  mips_tlt<I>,  OUT_TRAP, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_II_Instr} },
    {0x33, { "tltu", mips_tltu<I>, OUT_TRAP, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_II_Instr} },
    {0x34, { "teq",  mips_teq<I>,  OUT_TRAP, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_II_Instr} },
//  {0x35
    {0x36, { "tne",  mips_tne<I>,  OUT_TRAP, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_II_Instr} },
//  {0x37
    // Doubleword shifts
    {0x38, { "dsll",   mips_dsll<I>  , OUT_ARITHM, 0, 'S', Imm::SHIFT, { Src::RT }, { Dst::RD }, MIPS_III_Instr} },
    {0x3A, { "dsrl",   mips_dsrl<I>  , OUT_ARITHM, 0, 'S', Imm::SHIFT, { Src::RT }, { Dst::RD }, MIPS_III_Instr} },
    {0x3B, { "dsra",   mips_dsra<I>  , OUT_ARITHM, 0, 'S', Imm::SHIFT, { Src::RT }, { Dst::RD }, MIPS_III_Instr} },
    {0x3C, { "dsll32", mips_dsll32<I>, OUT_ARITHM, 0, 'S', Imm::SHIFT, { Src::RT }, { Dst::RD }, MIPS_III_Instr} },
    {0x3E, { "dsrl32", mips_dsrl32<I>, OUT_ARITHM, 0, 'S', Imm::SHIFT, { Src::RT }, { Dst::RD }, MIPS_III_Instr} },
    {0x3F, { "dsra32", mips_dsra32<I>, OUT_ARITHM, 0, 'S', Imm::SHIFT, { Src::RT }, { Dst::RD }, MIPS_III_Instr} }
};

//unordered map for RI-instructions
template<typename I>
static const Table<I> isaMapRI =
{
    // Branches
    {0x0,  { "bltz",  mips_bltz<I>,  OUT_BRANCH,        0, 'I', Imm::ARITH, { Src::RS }, { Dst::ZERO }, MIPS_I_Instr} },
    {0x1,  { "bgez",  mips_bgez<I>,  OUT_BRANCH,        0, 'I', Imm::ARITH, { Src::RS }, { Dst::ZERO }, MIPS_I_Instr} },
    {0x2,  { "bltzl", mips_bltzl<I>, OUT_BRANCH_LIKELY, 0, 'I', Imm::ARITH, { Src::RS }, { Dst::ZERO }, MIPS_II_Instr} },
    {0x3,  { "bgezl", mips_bgezl<I>, OUT_BRANCH_LIKELY, 0, 'I', Imm::ARITH, { Src::RS }, { Dst::ZERO }, MIPS_II_Instr} },
    // Conditional traps
    {0x8,  { "tgei",  mips_tgei<I>,  OUT_TRAP, 0, 'I', Imm::TRAP, { Src::RS }, { Dst::ZERO }, MIPS_II_Instr} },
    {0x9,  { "tgeiu", mips_tgeiu<I>, OUT_TRAP, 0, 'I', Imm::TRAP, { Src::RS }, { Dst::ZERO }, MIPS_II_Instr} },
    {0xA,  { "tlti",  mips_tlti<I>,  OUT_TRAP, 0, 'I', Imm::TRAP, { Src::RS }, { Dst::ZERO }, MIPS_II_Instr} },
    {0xB,  { "tltiu", mips_tltiu<I>, OUT_TRAP, 0, 'I', Imm::TRAP, { Src::RS }, { Dst::ZERO }, MIPS_II_Instr} },
    {0xC,  { "teqi",  mips_teqi<I>,  OUT_TRAP, 0, 'I', Imm::TRAP, { Src::RS }, { Dst::ZERO }, MIPS_II_Instr} },
    {0xE,  { "tnei",  mips_tnei<I>,  OUT_TRAP, 0, 'I', Imm::TRAP, { Src::RS }, { Dst::ZERO }, MIPS_II_Instr} },
    // Linking branches
    {0x10, { "bltzal",  mips_bltzal<I>,  OUT_BRANCH,        0, 'I', Imm::ARITH, { Src::RS }, { Dst::RA }, MIPS_I_Instr} },
    {0x11, { "bgezal",  mips_bgezal<I>,  OUT_BRANCH,        0, 'I', Imm::ARITH, { Src::RS }, { Dst::RA }, MIPS_I_Instr} },
    {0x12, { "bltzall", mips_bltzall<I>, OUT_BRANCH_LIKELY, 0, 'I', Imm::ARITH, { Src::RS }, { Dst::RA }, MIPS_II_Instr} },
    {0x13, { "bgezall", mips_bgezall<I>, OUT_BRANCH_LIKELY, 0, 'I', Imm::ARITH, { Src::RS }, { Dst::RA }, MIPS_II_Instr} }
};

//unordered map for I-instructions and J-instructions
template<typename I>
static const Table<I> isaMapIJ =
{
    // Direct jumps
    {0x2, { "j",   mips_j<I>,   OUT_J_JUMP, 0, 'J', Imm::JUMP, { }, { Dst::ZERO }, MIPS_I_Instr } },
    {0x3, { "jal", mips_jal<I>, OUT_J_JUMP, 0, 'J', Imm::JUMP, { }, { Dst::RA },   MIPS_I_Instr } },
    // Branches
    {0x4, { "beq",  mips_beq<I>,  OUT_BRANCH, 0, 'I', Imm::ARITH, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_I_Instr} },
    {0x5, { "bne",  mips_bne<I>,  OUT_BRANCH, 0, 'I', Imm::ARITH, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_I_Instr} },
    {0x6, { "blez", mips_blez<I>, OUT_BRANCH, 0, 'I', Imm::ARITH, { Src::RS },          { Dst::ZERO }, MIPS_I_Instr} },
    {0x7, { "bgtz", mips_bgtz<I>, OUT_BRANCH, 0, 'I', Imm::ARITH, { Src::RS },          { Dst::ZERO }, MIPS_I_Instr} },
    // Addition/Subtraction
    {0x8, { "addi",  mips_addi<I>,  OUT_ARITHM, 0, 'I', Imm::ARITH, { Src::RS }, { Dst::RT }, MIPS_I_Instr} },
    {0x9, { "addiu", mips_addiu<I>, OUT_ARITHM, 0, 'I', Imm::ARITH, { Src::RS }, { Dst::RT }, MIPS_I_Instr} },
    // Logical operations
    {0xA, { "slti",  mips_slti<I>,  OUT_ARITHM, 0, 'I', Imm::ARITH, { Src::RS }, { Dst::RT }, MIPS_I_Instr} },
    {0xB, { "sltiu", mips_sltiu<I>, OUT_ARITHM, 0, 'I', Imm::ARITH, { Src::RS }, { Dst::RT }, MIPS_I_Instr} },
    {0xC, { "andi",  mips_andi<I>,  OUT_ARITHM, 0, 'L', Imm::LOGIC, { Src::RS }, { Dst::RT }, MIPS_I_Instr} },
    {0xD, { "ori",   mips_ori<I>,   OUT_ARITHM, 0, 'L', Imm::LOGIC, { Src::RS }, { Dst::RT }, MIPS_I_Instr} },
    {0xE, { "xori",  mips_xori<I>,  OUT_ARITHM, 0, 'L', Imm::LOGIC, { Src::RS }, { Dst::RT }, MIPS_I_Instr} },
    {0xF, { "lui",   mips_lui<I>,   OUT_ARITHM, 0, 'I', Imm::LOGIC, { },         { Dst::RT }, MIPS_I_Instr} },
    // 0x10 - 0x13 coprocessor operations
    // Likely branches (MIPS II)
    {0x14, { "beql",  mips_beql<I>,  OUT_BRANCH_LIKELY, 0, 'I', Imm::ARITH, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_II_Instr} },
    {0x15, { "bnel",  mips_bnel<I>,  OUT_BRANCH_LIKELY, 0, 'I', Imm::ARITH, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_II_Instr} },
    {0x16, { "blezl", mips_blezl<I>, OUT_BRANCH_LIKELY, 0, 'I', Imm::ARITH, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_II_Instr} },
    {0x17, { "bgtzl", mips_bgtzl<I>, OUT_BRANCH_LIKELY, 0, 'I', Imm::ARITH, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_II_Instr} },
    // Doubleword unaligned loads
    {0x1A, { "ldl", mips_ldl<I>,  OUT_LOAD, 8, 'I', Imm::ADDR, { Src::RS }, { Dst::RT }, MIPS_III_Instr} },
    {0x1B, { "ldr", mips_ldr<I>,  OUT_LOAD, 8, 'I', Imm::ADDR, { Src::RS }, { Dst::RT }, MIPS_III_Instr} },
    // Doubleword addition
    {0x18, { "daddi",  mips_daddi<I>,  OUT_ARITHM, 0, 'I', Imm::ARITH, { Src::RS }, { Dst::RT }, MIPS_III_Instr} },
    {0x19, { "daddiu", mips_daddiu<I>, OUT_ARITHM, 0, 'I', Imm::ARITH, { Src::RS }, { Dst::RT }, MIPS_III_Instr} },
    // Loads
    {0x20, { "lb",  mips_lb<I>,  OUT_LOAD, 1, 'I',         Imm::ADDR, { Src::RS }, { Dst::RT }, MIPS_I_Instr} },
    {0x21, { "lh",  mips_lh<I>,  OUT_LOAD, 2, 'I',         Imm::ADDR, { Src::RS }, { Dst::RT }, MIPS_I_Instr} },
    {0x22, { "lwl", mips_lwl<I>, OUT_PARTIAL_LOAD, 4, 'I', Imm::ADDR, { Src::RS }, { Dst::RT }, MIPS_I_Instr} },
    {0x23, { "lw",  mips_lw<I>,  OUT_LOAD, 4, 'I',         Imm::ADDR, { Src::RS }, { Dst::RT }, MIPS_I_Instr} },
    {0x24, { "lbu", mips_lbu<I>, OUT_LOADU, 1, 'I',        Imm::ADDR, { Src::RS }, { Dst::RT }, MIPS_I_Instr} },
    {0x25, { "lhu", mips_lhu<I>, OUT_LOADU, 2, 'I',        Imm::ADDR, { Src::RS }, { Dst::RT }, MIPS_I_Instr} },
    {0x26, { "lwr", mips_lwr<I>, OUT_PARTIAL_LOAD, 4, 'I', Imm::ADDR, { Src::RS }, { Dst::RT }, MIPS_I_Instr} },
    {0x27, { "lwu", mips_lwu<I>, OUT_LOADU, 4, 'I',        Imm::ADDR, { Src::RS }, { Dst::RT }, MIPS_I_Instr} },
    // Stores
    {0x28, { "sb",  mips_sb<I>,  OUT_STORE, 1, 'I', Imm::ADDR, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_I_Instr} },
    {0x29, { "sh",  mips_sh<I>,  OUT_STORE, 2, 'I', Imm::ADDR, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_I_Instr} },
    {0x2A, { "swl", mips_swl<I>, OUT_STORE, 4, 'I', Imm::ADDR, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_I_Instr} },
    {0x2B, { "sw",  mips_sw<I>,  OUT_STORE, 4, 'I', Imm::ADDR, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_I_Instr} },
    {0x2C, { "sdl", mips_sdl<I>, OUT_STORE, 8, 'I', Imm::ADDR, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_III_Instr} },
    {0x2D, { "sdr", mips_sdr<I>, OUT_STORE, 8, 'I', Imm::ADDR, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_III_Instr} },
    {0x2E, { "swr", mips_swr<I>, OUT_STORE, 4, 'I', Imm::ADDR, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_I_Instr} },
//  {0x2F, { "cache"
    // Advanced loads and stores
    {0x30, { "ll",   mips_ll<I>,   OUT_LOAD,  2, 'I', Imm::ADDR, { Src::RS },          { Dst::RT },   MIPS_I_Instr} },
    {0x31, { "lwc1", mips_lwc1<I>, OUT_LOAD,  4, 'I', Imm::ADDR, { Src::RS },          { Dst::FT },   MIPS_I_Instr} },
    {0x35, { "ldc1", mips_ldc1<I>, OUT_LOAD,  8, 'I', Imm::ADDR, { Src::RS },          { Dst::FT },   MIPS_II_Instr} },
    {0x37, { "ld",   mips_ld<I>,   OUT_LOAD,  8, 'I', Imm::ADDR, { Src::RS },          { Dst::RT },   MIPS_III_Instr} },
    {0x38, { "sc",   mips_sc<I>,   OUT_STORE, 2, 'I', Imm::ADDR, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_I_Instr} },
    {0x39, { "swc1", mips_swc1<I>, OUT_STORE, 4, 'I', Imm::ADDR, { Src::RS },          { Dst::FT },   MIPS_I_Instr} },
    {0x3D, { "sdc1", mips_sdc1<I>, OUT_STORE, 8, 'I', Imm::ADDR, { Src::RS },          { Dst::FT },   MIPS_II_Instr} },
    {0x3F, { "sd",   mips_sd<I>,   OUT_STORE, 8, 'I', Imm::ADDR, { Src::RS, Src::RT }, { Dst::ZERO }, MIPS_III_Instr} },
};

template<typename I>
static const Table<I> isaMapMIPS32 =
{
    // Advanced multiplication
    {0x00, { "madd",  mips_madd<I>,  OUT_R_ACCUM, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::LO, Dst::HI }, MIPS_32_Instr} },
    {0x01, { "maddu", mips_maddu<I>, OUT_R_ACCUM, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::LO, Dst::HI }, MIPS_32_Instr} },
    {0x02, { "mul",   mips_mul<I>,   OUT_ARITHM,  0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::RD },          MIPS_32_Instr} },
    {0x04, { "msub",  mips_msub<I>,  OUT_R_SUBTR, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::LO, Dst::HI }, MIPS_32_Instr} },
    {0x05, { "msubu", mips_msubu<I>, OUT_R_SUBTR, 0, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::LO, Dst::HI }, MIPS_32_Instr} },
    // Count leading zeroes/ones
    {0x20, { "clz",  mips_clz<I>,  OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS }, { Dst::RD }, MIPS_32_Instr} },
    {0x21, { "clo",  mips_clo<I>,  OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS }, { Dst::RD }, MIPS_32_Instr} },
    {0x24, { "dclz", mips_dclz<I>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS }, { Dst::RD }, MIPS_64_Instr} },
    {0x25, { "dclo", mips_dclo<I>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::RS }, { Dst::RD }, MIPS_64_Instr} }
};

template<typename I>
static const Table<I> isaMapCOP0_rs =
{
    {0x00, { "mfc0",  mips_mfc0<I>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::CP0_RD }, { Dst::RT },     MIPS_I_Instr} },
    {0x04, { "mtc0",  mips_mtc0<I>, OUT_ARITHM, 0, 'N', Imm::NO, { Src::RT },     { Dst::CP0_RD }, MIPS_I_Instr} },
};

template<typename I>
static const Table<I> isaMapCOP0_funct =
{
    {0x18, { "eret",  mips_eret<I>, OUT_R_JUMP, 0, 'N', Imm::NO, { Src::EPC, Src::SR }, { Dst::SR }, MIPS_I_Instr} },
};

template<typename I>
static const Table<I> isaMapCOP1 =
{
    // Moves from Floating Point
    {0x00, { "mfc1",  mips_mfc1<I>,  OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::RT }, MIPS_I_Instr} },
    {0x01, { "dmfc1", mips_dmfc1<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::RT }, MIPS_III_Instr} },
    {0x02, { "cfc1",  mips_cfc1<I>,  OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::RT }, MIPS_I_Instr} },
    // 0x03
    // Moves to Floating Point
    {0x04, { "mtc1",  mips_mtc1<I>,  OUT_FPU, 0, 'N', Imm::NO, { Src::RT }, { Dst::FS }, MIPS_I_Instr} },
    {0x05, { "dmtc1", mips_dmtc1<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::RT }, { Dst::FS }, MIPS_III_Instr} },
    {0x06, { "ctc1",  mips_ctc1<I>,  OUT_FPU, 0, 'N', Imm::NO, { Src::RT }, { Dst::FS }, MIPS_III_Instr} },
};

template<typename I>
static const Table<I> isaMapCOP1_s =
{
    // Formatted basic instructions
    {0x00, { "add.s",     mips_add_s<I>,     OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FD }, MIPS_I_Instr} },
    {0x01, { "sub.s",     mips_sub_s<I>,     OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FD }, MIPS_I_Instr} },
    {0x02, { "mul.s",     mips_mul_s<I>,     OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FD }, MIPS_I_Instr} },
    {0x03, { "div.s",     mips_div_s<I>,     OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FD }, MIPS_I_Instr} },
    // FPU specific instructions
    {0x04, { "sqrt.s",    mips_sqrt_s<I>,    OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_II_Instr} },
    {0x05, { "abs.s",     mips_abs_s<I>,     OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_I_Instr} },
    {0x06, { "mov.s",     mips_mov_s<I>,     OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_I_Instr} },
    {0x07, { "neg.s",     mips_neg_s<I>,     OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_I_Instr} },
    {0x08, { "round.l.s", mips_round_l_s<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_III_Instr} },
    {0x09, { "trunc.l.s", mips_trunc_l_s<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_III_Instr} },
    {0x0A, { "ceil.l.s",  mips_ceil_l_s<I>,  OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_III_Instr} },
    {0x0B, { "floor.l.s", mips_floor_l_s<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_III_Instr} },
    {0x0C, { "round.w.s", mips_round_w_s<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_II_Instr} },
    {0x0D, { "trunc.w.s", mips_trunc_w_s<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_II_Instr} },
    {0x0E, { "ceil.w.s",  mips_ceil_w_s<I>,  OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_II_Instr} },
    {0x0F, { "floor.w.s", mips_floor_w_s<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_II_Instr} },
    // 0x10 - 0x11
    {0x12, { "movz.s",    mips_movz_s<I>,    OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::RT }, { Dst::FD }, MIPS_IV_Instr} },
    {0x13, { "movn.s",    mips_movn_s<I>,    OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::RT }, { Dst::FD }, MIPS_IV_Instr} },
    // 0x14
    {0x15, { "recip.s",   mips_recip_s<I>,   OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_IV_Instr} },
    {0x16, { "rsqrt.s",   mips_rsqrt_s<I>,   OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_IV_Instr} },
    // Converts
    {0x21, { "cvt.d.s",   mips_cvt_d_s<I>,   OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_I_Instr} },
    // 0x22 - 0x23
    {0x24, { "cvt.w.s",   mips_cvt_w_s<I>,   OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_I_Instr} },
    {0x25, { "cvt.l.s",   mips_cvt_l_s<I>,   OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_III_Instr} },
    // 0x26 - 0x2F
    // Conditions
    {0x30, { "c.f.s",     mips_c_f_s<I>,     OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x31, { "c.un.s",    mips_c_un_s<I>,    OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x32, { "c.eq.s",    mips_c_eq_s<I>,    OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x33, { "c.ueq.s",   mips_c_ueq_s<I>,   OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x34, { "c.olt.s",   mips_c_olt_s<I>,   OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x35, { "c.ult.s",   mips_c_ult_s<I>,   OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x36, { "c.ole.s",   mips_c_ole_s<I>,   OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x37, { "c.ule.s",   mips_c_ule_s<I>,   OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x38, { "c.sf.s",    mips_c_sf_s<I>,    OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x39, { "c.ngle.s",  mips_c_ngle_s<I>,  OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x3A, { "c.seq.s",   mips_c_seq_s<I>,   OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x3B, { "c.ngl.s",   mips_c_ngl_s<I>,   OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x3C, { "c.lt.s",    mips_c_lt_s<I>,    OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x3D, { "c.nge.s",   mips_c_nge_s<I>,   OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x3E, { "c.le.s",    mips_c_le_s<I>,    OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x3F, { "c.ngt.s",   mips_c_ngt_s<I>,   OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
};

template<typename I>
static const Table<I> isaMapCOP1_d =
{
    // Formatted basic instructions
    {0x00, { "add.d",     mips_add_d<I>,     OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FD }, MIPS_I_Instr} },
    {0x01, { "sub.d",     mips_sub_d<I>,     OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FD }, MIPS_I_Instr} },
    {0x02, { "mul.d",     mips_mul_d<I>,     OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FD }, MIPS_I_Instr} },
    {0x03, { "div.d",     mips_div_d<I>,     OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FD }, MIPS_I_Instr} },
    // FPU specific instructions
    {0x04, { "sqrt.d",    mips_sqrt_d<I>,    OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_II_Instr} },
    {0x05, { "abs.d",     mips_abs_d<I>,     OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_I_Instr} },
    {0x06, { "mov.d",     mips_mov_d<I>,     OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_I_Instr} },
    {0x07, { "neg.d",     mips_neg_d<I>,     OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_I_Instr} },
    {0x08, { "round.l.d", mips_round_l_d<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_III_Instr} },
    {0x09, { "trunc.l.d", mips_trunc_l_d<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_III_Instr} },
    {0x0A, { "ceil.l.d",  mips_ceil_l_d<I>,  OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_III_Instr} },
    {0x0B, { "floor.l.d", mips_floor_l_d<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_III_Instr} },
    {0x0C, { "round.w.d", mips_round_w_d<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_II_Instr} },
    {0x0D, { "trunc.w.d", mips_trunc_w_d<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_II_Instr} },
    {0x0E, { "ceil.w.d",  mips_ceil_w_d<I>,  OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_II_Instr} },
    {0x0F, { "floor.w.d", mips_floor_w_d<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_II_Instr} },
    // 0x10 - 0x11
    {0x12, { "movz.d",    mips_movz_d<I>,    OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::RT }, { Dst::FD }, MIPS_IV_Instr} },
    {0x13, { "movn.d",    mips_movn_d<I>,    OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::RT }, { Dst::FD }, MIPS_IV_Instr} },
    // 0x14
    {0x15, { "recip.d",   mips_recip_d<I>,   OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_IV_Instr} },
    {0x16, { "rsqrt.d",   mips_rsqrt_d<I>,   OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_IV_Instr} },
    // 0x17 - 0x20
    // Converts
    {0x20, { "cvt.s.d",   mips_cvt_s_d<I>,   OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_I_Instr} },
    // 0x21 - 0x23
    {0x24, { "cvt.w.d",   mips_cvt_w_d<I>,   OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_I_Instr} },
    {0x25, { "cvt.l.d",   mips_cvt_l_d<I>,   OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_III_Instr} },
    // 0x26 - 0x2F
    // Conditions
    {0x30, { "c.f.d",     mips_c_f_d<I>,     OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x31, { "c.un.d",    mips_c_un_d<I>,    OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x32, { "c.eq.d",    mips_c_eq_d<I>,    OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x33, { "c.ueq.d",   mips_c_ueq_d<I>,   OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x34, { "c.olt.d",   mips_c_olt_d<I>,   OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x35, { "c.ult.d",   mips_c_ult_d<I>,   OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x36, { "c.ole.d",   mips_c_ole_d<I>,   OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x37, { "c.ule.d",   mips_c_ule_d<I>,   OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x38, { "c.sf.d",    mips_c_sf_d<I>,    OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x39, { "c.ngle.d",  mips_c_ngle_d<I>,  OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x3A, { "c.seq.d",   mips_c_seq_d<I>,   OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x3B, { "c.ngl.d",   mips_c_ngl_d<I>,   OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x3C, { "c.lt.d",    mips_c_lt_d<I>,    OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x3D, { "c.nge.d",   mips_c_nge_d<I>,   OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x3E, { "c.le.d",    mips_c_le_d<I>,    OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
    {0x3F, { "c.ngt.d",   mips_c_ngt_d<I>,   OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FT }, { Dst::FCSR }, MIPS_I_Instr} },
};

template<typename I>
static const Table<I> isaMapCOP1_l =
{
    // Converts
    {0x20, { "cvt.s.l", mips_cvt_s_l<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_III_Instr} },
    {0x21, { "cvt.d.l", mips_cvt_d_l<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_III_Instr} },
};

template<typename I>
static const Table<I> isaMapCOP1_w =
{
    // Converts
    {0x20, { "cvt.s.w", mips_cvt_s_w<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_I_Instr} },
    {0x21, { "cvt.d.w", mips_cvt_d_w<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS }, { Dst::FD }, MIPS_I_Instr} },
};

template<typename I>
static const Table<I> isaMapCOP1I =
{
    // Branches
    {0x0, { "bc1f",  mips_bc1f<I>,  OUT_BRANCH, 0, 'I', Imm::ARITH, { Src::FCSR }, { Dst::ZERO }, MIPS_I_Instr} },
    {0x1, { "bc1t",  mips_bc1t<I>,  OUT_BRANCH, 0, 'I', Imm::ARITH, { Src::FCSR }, { Dst::ZERO }, MIPS_I_Instr} },
    {0x2, { "bc1fl", mips_bc1fl<I>, OUT_BRANCH, 0, 'I', Imm::ARITH, { Src::FCSR }, { Dst::ZERO }, MIPS_I_Instr} },
    {0x3, { "bc1tl", mips_bc1tl<I>, OUT_BRANCH, 0, 'I', Imm::ARITH, { Src::FCSR }, { Dst::ZERO }, MIPS_I_Instr} },
};

template<typename I>
static const Table<I> isaMapCOP1X =
{
    // Loads
    {0x0, { "lwxc1", mips_lwxc1<I>,  OUT_LOAD,  4, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::FD }, MIPS_IV_Instr} },
    {0x1, { "ldxc1", mips_ldxc1<I>,  OUT_LOAD,  8, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::FD }, MIPS_IV_Instr} },
    // 0x2 - 0x7
    // Stores
    {0x8, { "swxc1", mips_swxc1<I>,  OUT_STORE, 4, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::FD }, MIPS_IV_Instr} },
    {0x9, { "sdxc1", mips_sdxc1<I>,  OUT_STORE, 8, 'N', Imm::NO, { Src::RS, Src::RT }, { Dst::FD }, MIPS_IV_Instr} },
    // 0xa - 0xe
    // 0xf PREFX
    // Advanced multiplication
    {0x20, { "madd.s", mips_madd_s<I>,   OUT_FPU, 0, 'N', Imm::NO, { Src::FR, Src::FS, Src::FT }, { Dst::FD }, MIPS_IV_Instr} },
    {0x21, { "madd.d", mips_madd_d<I>,   OUT_FPU, 0, 'N', Imm::NO, { Src::FR, Src::FS, Src::FT }, { Dst::FD }, MIPS_IV_Instr} },
    // 0x22 - 0x27
    {0x28, { "msub.s", mips_msub_s<I>,   OUT_FPU, 0, 'N', Imm::NO, { Src::FR, Src::FS, Src::FT }, { Dst::FD }, MIPS_IV_Instr} },
    {0x29, { "msub.d", mips_msub_d<I>,   OUT_FPU, 0, 'N', Imm::NO, { Src::FR, Src::FS, Src::FT }, { Dst::FD }, MIPS_IV_Instr} },
    // 0x2A - 0x2F
    {0x30, { "nmadd.s", mips_nmadd_s<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FR, Src::FS, Src::FT }, { Dst::FD }, MIPS_IV_Instr} },
    {0x31, { "nmadd.d", mips_nmadd_d<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FR, Src::FS, Src::FT }, { Dst::FD }, MIPS_IV_Instr} },
    // 0x32 - 0x37
    {0x38, { "nmsub.s", mips_nmsub_s<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FR, Src::FS, Src::FT }, { Dst::FD }, MIPS_IV_Instr} },
    {0x39, { "nmsub.d", mips_nmsub_d<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FR, Src::FS, Src::FT }, { Dst::FD }, MIPS_IV_Instr} },
};

template<typename I>
static const Table<I> isaMapMOVCI =
{
    // Moves on FP condition
    {0x0, { "movf",  mips_movf<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::RS, Src::FCSR }, { Dst::RD }, MIPS_IV_Instr} },
    {0x1, { "movt",  mips_movt<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::RS, Src::FCSR }, { Dst::RD }, MIPS_IV_Instr} },
};

template<typename I>
static const Table<I> isaMapMOVCF_d =
{
    // Moves on FP condition
    {0x0, { "movf.d",  mips_movf_d<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FCSR }, { Dst::FD }, MIPS_IV_Instr} },
    {0x1, { "movt.d",  mips_movt_d<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FCSR }, { Dst::FD }, MIPS_IV_Instr} },
};

template<typename I>
static const Table<I> isaMapMOVCF_s =
{
    // Moves on FP condition
    {0x0, { "movf.s",  mips_movf_s<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FCSR }, { Dst::FD }, MIPS_IV_Instr} },
    {0x1, { "movt.s",  mips_movt_s<I>, OUT_FPU, 0, 'N', Imm::NO, { Src::FS, Src::FCSR }, { Dst::FD }, MIPS_IV_Instr} },
};

template<typename I>
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

template<typename I>
static const MIPSTableEntry<I> unknown_instruction = { };

template<typename I>
static const MIPSTableEntry<I> instr_nop =
{ "nop" , do_nothing<I>, OUT_ARITHM, 0, 'N', Imm::NO, { }, { Dst::ZERO }, MIPS_I_Instr};

template<typename I>
static MIPSTableEntry<I> get_table_entry( const Table<I>& table, uint32 key)
{
    auto it = table.find( key);
    return it == table.end() ? unknown_instruction<I> : it->second;
}

template<typename I>
static MIPSTableEntry<I> get_opcode_special_entry( const MIPSInstrDecoder& instr)
{
    if ( instr.funct == 0x1)
        return get_table_entry( isaMapMOVCI<I>, instr.ft);
    return get_table_entry( isaMapR<I>, instr.funct);
}

template<typename I>
static MIPSTableEntry<I> get_COP1_s_entry( const MIPSInstrDecoder& instr)
{
    if ( instr.funct == 0x11)
        return get_table_entry( isaMapMOVCF_s<I>, instr.ft);
    return get_table_entry( isaMapCOP1_s<I>, instr.funct);
}

template<typename I>
static MIPSTableEntry<I> get_COP1_d_entry( const MIPSInstrDecoder& instr)
{
    if ( instr.funct == 0x11)
        return get_table_entry( isaMapMOVCF_d<I>, instr.ft);
    return get_table_entry( isaMapCOP1_d<I>,  instr.funct);
}

template<typename I>
static MIPSTableEntry<I> get_cp0_entry( const MIPSInstrDecoder& instr)
{
    switch ( instr.funct)
    {
        case 0x0:  return get_table_entry( isaMapCOP0_rs<I>,    instr.rs);
        default:   return get_table_entry( isaMapCOP0_funct<I>, instr.funct);
    }
}

template<typename I>
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

template<typename I>
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

template<typename I>
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

template<typename R>
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
    this->set_type( entry.operation);
    this->mem_size   = entry.mem_size;
    this->executor   = entry.versions.is_supported(version) ? entry.function : mips_unknown<MyDatapath>;
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
    return std::move( oss).str();
}

template<typename R>
std::string BaseMIPSInstr<R>::bytes_dump() const
{
     std::ostringstream oss;
     oss << "Bytes:" << std::hex;
     const auto& bytes = endian == std::endian::little ? unpack_array<uint32, std::endian::little>( raw) : unpack_array<uint32, std::endian::big>( raw);
     for ( const auto& b : bytes)
         oss << " 0x" << std::setfill( '0') << std::setw( 2) << static_cast<uint16>( b);
     return std::move( oss).str();
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
