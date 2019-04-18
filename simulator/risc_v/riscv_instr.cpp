/*
 * riscv_instr.cpp - instruction parser for risc_v
 * Copyright 2018 MIPT-MIPS
 */

#include "riscv_decoder.h"
#include "riscv_instr.h"

#include <func_sim/alu.h>
#include <func_sim/operation.h>

#include <iomanip>
#include <sstream>
#include <vector>

template<typename I> auto do_nothing = ALU::unknown_instruction<I>;
// I
template<typename I> auto execute_lui = ALU::upper_immediate<I, 12>;
template<typename I> auto execute_auipc = ALU::auipc<I>;
template<typename I> auto execute_jal = ALU::jump_and_link<I, ALU::j<I>>;
template<typename I> auto execute_jalr = ALU::jump_and_link<I, ALU::riscv_jr<I>>;
template<typename I> auto execute_beq = ALU::branch<I, ALU::eq<I>>;
template<typename I> auto execute_bne = ALU::branch<I, ALU::ne<I>>;
template<typename I> auto execute_blt = ALU::branch<I, ALU::lt<I>>;
template<typename I> auto execute_bge = ALU::branch<I, ALU::ge<I>>;
template<typename I> auto execute_bltu = ALU::branch<I, ALU::ltu<I>>;
template<typename I> auto execute_bgeu = ALU::branch<I, ALU::geu<I>>;
template<typename I> auto execute_lb = ALU::addr<I>;
template<typename I> auto execute_lh = ALU::addr<I>;
template<typename I> auto execute_lw = ALU::addr<I>;
template<typename I> auto execute_lbu = ALU::addr<I>;
template<typename I> auto execute_lhu = ALU::addr<I>;
template<typename I> auto execute_lwu = ALU::addr<I>;
template<typename I> auto execute_ld = ALU::addr<I>;
template<typename I> auto execute_sb = ALU::store_addr<I>;
template<typename I> auto execute_sh = ALU::store_addr<I>;
template<typename I> auto execute_sw = ALU::store_addr<I>;
template<typename I> auto execute_sd = ALU::store_addr<I>;
template<typename I> auto execute_addi = ALU::riscv_addition_imm<I, typename I::RegisterUInt>;
template<typename I> auto execute_addiw = ALU::riscv_addition_imm<I, uint32>;
template<typename I> auto execute_slti = ALU::set<I, ALU::lti<I>>;
template<typename I> auto execute_sltiu = ALU::set<I, ALU::ltiu<I>>;
template<typename I> auto execute_xori = ALU::xori<I>;
template<typename I> auto execute_ori = ALU::ori<I>;
template<typename I> auto execute_andi = ALU::andi<I>;
template<typename I> auto execute_slli = ALU::sll<I, typename I::RegisterUInt>;
template<typename I> auto execute_slliw = ALU::sll<I, uint32>;
template<typename I> auto execute_srli = ALU::srl<I, typename I::RegisterUInt>;
template<typename I> auto execute_srliw = ALU::srl<I, uint32>;
template<typename I> auto execute_srai = ALU::sra<I, typename I::RegisterUInt>;
template<typename I> auto execute_sraiw = ALU::sra<I, uint32>;
template<typename I> auto execute_add = ALU::riscv_addition<I, typename I::RegisterUInt>;
template<typename I> auto execute_addw = ALU::riscv_addition<I, uint32>;
template<typename I> auto execute_sub = ALU::riscv_subtraction<I, typename I::RegisterUInt>;
template<typename I> auto execute_subw = ALU::riscv_subtraction<I, uint32>;
template<typename I> auto execute_sll = ALU::sllv<I, typename I::RegisterUInt>;
template<typename I> auto execute_sllw = ALU::sllv<I, uint32>;
template<typename I> auto execute_slt = ALU::set<I, ALU::lt<I>>;
template<typename I> auto execute_sltu = ALU::set<I, ALU::ltu<I>>;
template<typename I> auto execute_xor = ALU::xorv<I>;
template<typename I> auto execute_srl = ALU::srlv<I, typename I::RegisterUInt>;
template<typename I> auto execute_srlw = ALU::srlv<I, uint32>;
template<typename I> auto execute_sra = ALU::srav<I, typename I::RegisterUInt>;
template<typename I> auto execute_sraw = ALU::srav<I, uint32>;
template<typename I> auto execute_or = ALU::orv<I>;
template<typename I> auto execute_and = ALU::andv<I>;
// System I
template<typename I> auto execute_ecall = ALU::halt<I>;
template<typename I> auto execute_ebreak = do_nothing<I>;
template<typename I> auto execute_uret = ALU::jump_and_link<I, ALU::riscv_jr<I>>;
template<typename I> auto execute_sret = ALU::jump_and_link<I, ALU::riscv_jr<I>>;
template<typename I> auto execute_mret = ALU::jump_and_link<I, ALU::riscv_jr<I>>;
template<typename I> auto execute_wfi = do_nothing<I>;
template<typename I> auto execute_fence = ALU::addr<I>;
template<typename I> auto execute_csrrw = ALU::csrrw<I>;
template<typename I> auto execute_csrrs = ALU::csrrs<I>;
template<typename I> auto execute_csrrc = do_nothing<I>;
template<typename I> auto execute_csrrwi = ALU::csrrwi<I>;
template<typename I> auto execute_csrrsi = do_nothing<I>;
template<typename I> auto execute_csrrci = do_nothing<I>;
// M
template<typename I> auto execute_mul = do_nothing<I>;
template<typename I> auto execute_mulh = do_nothing<I>;
template<typename I> auto execute_mulhsu = do_nothing<I>;
template<typename I> auto execute_mulhu = do_nothing<I>;
template<typename I> auto execute_div = do_nothing<I>;
template<typename I> auto execute_divu = do_nothing<I>;
template<typename I> auto execute_rem = do_nothing<I>;
template<typename I> auto execute_remu = do_nothing<I>;
// Compressed
template<typename I> auto execute_c_lwsp = do_nothing<I>;
template<typename I> auto execute_c_ldsp = do_nothing<I>;
template<typename I> auto execute_c_lqsp = do_nothing<I>;
template<typename I> auto execute_c_swsp = do_nothing<I>;
template<typename I> auto execute_c_sdsp = do_nothing<I>;
template<typename I> auto execute_c_sqsp = do_nothing<I>;
template<typename I> auto execute_c_lw = do_nothing<I>;
template<typename I> auto execute_c_ld = do_nothing<I>;
template<typename I> auto execute_c_lq = do_nothing<I>;
template<typename I> auto execute_c_sw = do_nothing<I>;
template<typename I> auto execute_c_j = do_nothing<I>;
template<typename I> auto execute_c_jal = do_nothing<I>;
template<typename I> auto execute_c_jr = do_nothing<I>;
template<typename I> auto execute_c_jalr = do_nothing<I>;
template<typename I> auto execute_c_beqz = do_nothing<I>;
template<typename I> auto execute_c_bnez = do_nothing<I>;
template<typename I> auto execute_c_li = do_nothing<I>;
template<typename I> auto execute_c_lui = do_nothing<I>;
template<typename I> auto execute_c_addi = do_nothing<I>;
template<typename I> auto execute_c_addiw = do_nothing<I>;
template<typename I> auto execute_c_addi16sp = do_nothing<I>;
template<typename I> auto execute_c_addi4spn = do_nothing<I>;
template<typename I> auto execute_c_slli = do_nothing<I>;
template<typename I> auto execute_c_srli = do_nothing<I>;
template<typename I> auto execute_c_srai = do_nothing<I>;
template<typename I> auto execute_c_andi = do_nothing<I>;
template<typename I> auto execute_c_mv = do_nothing<I>;
template<typename I> auto execute_c_add = do_nothing<I>;
template<typename I> auto execute_c_and = do_nothing<I>;
template<typename I> auto execute_c_or = do_nothing<I>;
template<typename I> auto execute_c_xor = do_nothing<I>;
template<typename I> auto execute_c_sub = do_nothing<I>;
template<typename I> auto execute_c_addw = do_nothing<I>;
template<typename I> auto execute_c_subw = do_nothing<I>;
template<typename I> auto execute_c_ebreak = do_nothing<I>;

using Src1 = Reg;
using Src2 = Reg;
using Dst  = Reg;

struct RISCVAutogeneratedTableEntry
{
    std::string_view name;
    uint32 match;
    uint32 mask;
    RISCVAutogeneratedTableEntry() = delete;
    constexpr bool check_mask( uint32 bytes) const noexcept { return ( bytes & mask) == match; }
};

#define PASTER(x,y) x ## y
#define EVALUATOR(x,y) PASTER(x,y)

#define DECLARE_INSN(name, match, mask) \
static const RISCVAutogeneratedTableEntry instr_ ## name = { #name, match, mask };
#include <riscv.opcode.gen.h>
#undef DECLARE_INSN

template<typename I>
struct RISCVTableEntry
{
    char subset;
    RISCVAutogeneratedTableEntry entry;
    ALU::Execute<I> function;
    OperationType type;
    char immediate_type;
    Imm immediate_print_type;
    Src1 src1;
    Src2 src2;
    Dst  dst;
    uint32 mem_size;
    RISCVTableEntry() = delete;
};

template<typename I>
static const std::vector<RISCVTableEntry<I>> cmd_desc =
{
    /*-------------- I --------------*/
    {'I', instr_lui,    execute_lui<I>,    OUT_ARITHM, 'U',                   Imm::LOGIC, Src1::ZERO,       Src2::ZERO,       Dst::RD,         0},
    {'I', instr_auipc,  execute_auipc<I>,  OUT_ARITHM, 'U',                   Imm::LOGIC, Src1::ZERO,       Src2::ZERO,       Dst::RD,         0},
    {'I', instr_c_li,   execute_c_li<I>,   OUT_ARITHM, ImmediateType::C_LI,   Imm::ARITH, Src1::ZERO,       Src2::ZERO,       Dst::RD,         0},
    {'I', instr_c_lui,  execute_c_lui<I>,  OUT_ARITHM, ImmediateType::C_LUI,  Imm::LOGIC, Src1::ZERO,       Src2::ZERO,       Dst::RD,         0},
    // Jumps and branches
    {'I', instr_jal,    execute_jal<I>,    OUT_J_JUMP, 'J',                   Imm::ARITH, Src1::ZERO,       Src2::ZERO,       Dst::RD,         0},
    {'I', instr_jalr,   execute_jalr<I>,   OUT_R_JUMP, 'I',                   Imm::LOGIC, Src1::RS1,        Src2::ZERO,       Dst::RD,         0},
    {'I', instr_beq,    execute_beq<I>,    OUT_BRANCH, 'B',                   Imm::ARITH, Src1::RS1,        Src2::RS2,        Dst::ZERO,       0},
    {'I', instr_bne,    execute_bne<I>,    OUT_BRANCH, 'B',                   Imm::ARITH, Src1::RS1,        Src2::RS2,        Dst::ZERO,       0},
    {'I', instr_blt,    execute_blt<I>,    OUT_BRANCH, 'B',                   Imm::ARITH, Src1::RS1,        Src2::RS2,        Dst::ZERO,       0},
    {'I', instr_bge,    execute_bge<I>,    OUT_BRANCH, 'B',                   Imm::ARITH, Src1::RS1,        Src2::RS2,        Dst::ZERO,       0},
    {'I', instr_bltu,   execute_bltu<I>,   OUT_BRANCH, 'B',                   Imm::ARITH, Src1::RS1,        Src2::RS2,        Dst::ZERO,       0},
    {'I', instr_bgeu,   execute_bgeu<I>,   OUT_BRANCH, 'B',                   Imm::ARITH, Src1::RS1,        Src2::RS2,        Dst::ZERO,       0},
    {'I', instr_c_jal,  execute_c_jal<I>,  OUT_BRANCH, ImmediateType::C_JAL,  Imm::ARITH, Src1::ZERO,       Src2::ZERO,       Dst::ZERO,       0},
    // Loads and stores
    {'I', instr_lb,     execute_lb<I>,     OUT_LOAD,   'I',                   Imm::ADDR,  Src1::RS1,        Src2::ZERO,       Dst::RD,         1},
    {'I', instr_lh,     execute_lh<I>,     OUT_LOAD,   'I',                   Imm::ADDR,  Src1::RS1,        Src2::ZERO,       Dst::RD,         2},
    {'I', instr_lw,     execute_lw<I>,     OUT_LOAD,   'I',                   Imm::ADDR,  Src1::RS1,        Src2::ZERO,       Dst::RD,         4},
    {'I', instr_ld,     execute_ld<I>,     OUT_LOAD,   'I',                   Imm::ADDR,  Src1::RS1,        Src2::ZERO,       Dst::RD,         8},
    {'I', instr_lbu,    execute_lbu<I>,    OUT_LOADU,  'I',                   Imm::ADDR,  Src1::RS1,        Src2::ZERO,       Dst::RD,         1},
    {'I', instr_lhu,    execute_lhu<I>,    OUT_LOADU,  'I',                   Imm::ADDR,  Src1::RS1,        Src2::ZERO,       Dst::RD,         2},
    {'I', instr_lwu,    execute_lwu<I>,    OUT_LOADU,  'I',                   Imm::ADDR,  Src1::RS1,        Src2::ZERO,       Dst::RD,         4},
    {'I', instr_sb,     execute_sb<I>,     OUT_STORE,  'S',                   Imm::ADDR,  Src1::RS1,        Src2::RS2,        Dst::ZERO,       1},
    {'I', instr_sh,     execute_sh<I>,     OUT_STORE,  'S',                   Imm::ADDR,  Src1::RS1,        Src2::RS2,        Dst::ZERO,       2},
    {'I', instr_sw,     execute_sw<I>,     OUT_STORE,  'S',                   Imm::ADDR,  Src1::RS1,        Src2::RS2,        Dst::ZERO,       4},
    {'I', instr_sd,     execute_sd<I>,     OUT_STORE,  'S',                   Imm::ADDR,  Src1::RS1,        Src2::RS2,        Dst::ZERO,       8},
    {'I', instr_c_lwsp, execute_c_lwsp<I>, OUT_LOAD,   ImmediateType::C_LWSP, Imm::ADDR,  Src1::SP,         Src2::ZERO,       Dst::RD,         4},
    {'I', instr_c_lw,   execute_c_lw<I>,   OUT_LOAD,   ImmediateType::C_LW,   Imm::ADDR,  Src1::RS1_3_BITS, Src2::ZERO,       Dst::RD_3_BITS,  4},
    {'I', instr_c_swsp, execute_c_swsp<I>, OUT_STORE,  ImmediateType::C_SWSP, Imm::ADDR,  Src1::SP,         Src2::RS2_COMPR,  Dst::ZERO,       4},
    {'I', instr_c_sw,   execute_c_sw<I>,   OUT_STORE,  ImmediateType::C_SW,   Imm::ADDR,  Src1::RS1_3_BITS, Src2::RS2_3_BITS, Dst::ZERO,       4},
    // Immediate arithmetics
    {'I', instr_addi,   execute_addi<I>,   OUT_ARITHM, 'I',                   Imm::ARITH, Src1::RS1,        Src2::ZERO,       Dst::RD,         0},
    {'I', instr_slli,   execute_slli<I>,   OUT_ARITHM, 'I',                   Imm::ARITH, Src1::RS1,        Src2::ZERO,       Dst::RD,         0},
    {'I', instr_srli,   execute_srli<I>,   OUT_ARITHM, 'I',                   Imm::ARITH, Src1::RS1,        Src2::ZERO,       Dst::RD,         0},
    {'I', instr_srai,   execute_srai<I>,   OUT_ARITHM, 'I',                   Imm::ARITH, Src1::RS1,        Src2::ZERO,       Dst::RD,         0},
    {'I', instr_addiw,  execute_addiw<I>,  OUT_ARITHM, 'I',                   Imm::ARITH, Src1::RS1,        Src2::ZERO,       Dst::RD,         0},
    {'I', instr_slliw,  execute_slliw<I>,  OUT_ARITHM, 'I',                   Imm::ARITH, Src1::RS1,        Src2::ZERO,       Dst::RD,         0},
    {'I', instr_srliw,  execute_srliw<I>,  OUT_ARITHM, 'I',                   Imm::ARITH, Src1::RS1,        Src2::ZERO,       Dst::RD,         0},
    {'I', instr_sraiw,  execute_sraiw<I>,  OUT_ARITHM, 'I',                   Imm::ARITH, Src1::RS1,        Src2::ZERO,       Dst::RD,         0},
    {'I', instr_c_addi, execute_c_addi<I>, OUT_ARITHM, ImmediateType::C_ADDI, Imm::ARITH, Src1::ZERO,       Src2::ZERO,       Dst::RD,         0},
    {'I', instr_c_srli, execute_c_srli<I>, OUT_ARITHM, ImmediateType::C_SRLI, Imm::ARITH, Src1::ZERO,       Src2::ZERO,       Dst::RS1_3_BITS, 0},
    {'I', instr_c_srai, execute_c_srai<I>, OUT_ARITHM, ImmediateType::C_SRAI, Imm::ARITH, Src1::ZERO,       Src2::ZERO,       Dst::RS1_3_BITS, 0},
    // Immediate logic and comparison
    {'I', instr_slti,   execute_slti<I>,   OUT_ARITHM, 'I',                   Imm::ARITH, Src1::RS1,        Src2::ZERO,       Dst::RD,         0},
    {'I', instr_sltiu,  execute_sltiu<I>,  OUT_ARITHM, 'I',                   Imm::ARITH, Src1::RS1,        Src2::ZERO,       Dst::RD,         0},
    {'I', instr_xori,   execute_xori<I>,   OUT_ARITHM, 'I',                   Imm::LOGIC, Src1::RS1,        Src2::ZERO,       Dst::RD,         0},
    {'I', instr_ori,    execute_ori<I>,    OUT_ARITHM, 'I',                   Imm::LOGIC, Src1::RS1,        Src2::ZERO,       Dst::RD,         0},
    {'I', instr_andi,   execute_andi<I>,   OUT_ARITHM, 'I',                   Imm::LOGIC, Src1::RS1,        Src2::ZERO,       Dst::RD,         0},
    {'I', instr_c_andi, execute_c_andi<I>, OUT_ARITHM, ImmediateType::C_ANDI, Imm::ARITH, Src1::ZERO,       Src2::ZERO,       Dst::RS1_3_BITS, 0},
    // Register-register arithmetics
    {'I', instr_add,    execute_add<I>,    OUT_ARITHM, ' ',                   Imm::NO,    Src1::RS1,        Src2::RS2,        Dst::RD,         0},
    {'I', instr_sub,    execute_sub<I>,    OUT_ARITHM, ' ',                   Imm::NO,    Src1::RS1,        Src2::RS2,        Dst::RD,         0},
    {'I', instr_sll,    execute_sll<I>,    OUT_ARITHM, ' ',                   Imm::NO,    Src1::RS1,        Src2::RS2,        Dst::RD,         0},
    {'I', instr_sra,    execute_sra<I>,    OUT_ARITHM, ' ',                   Imm::NO,    Src1::RS1,        Src2::RS2,        Dst::RD,         0},
    {'I', instr_srl,    execute_srl<I>,    OUT_ARITHM, ' ',                   Imm::NO,    Src1::RS1,        Src2::RS2,        Dst::RD,         0},
    {'I', instr_addw,   execute_addw<I>,   OUT_ARITHM, ' ',                   Imm::NO,    Src1::RS1,        Src2::RS2,        Dst::RD,         0},
    {'I', instr_subw,   execute_subw<I>,   OUT_ARITHM, ' ',                   Imm::NO,    Src1::RS1,        Src2::RS2,        Dst::RD,         0},
    {'I', instr_sllw,   execute_sllw<I>,   OUT_ARITHM, ' ',                   Imm::NO,    Src1::RS1,        Src2::RS2,        Dst::RD,         0},
    {'I', instr_sraw,   execute_sraw<I>,   OUT_ARITHM, ' ',                   Imm::NO,    Src1::RS1,        Src2::RS2,        Dst::RD,         0},
    {'I', instr_srlw,   execute_srlw<I>,   OUT_ARITHM, ' ',                   Imm::NO,    Src1::RS1,        Src2::RS2,        Dst::RD,         0},
    {'I', instr_c_sub,  execute_c_sub<I>,  OUT_ARITHM, ' ',                   Imm::NO,    Src1::ZERO,       Src2::RS2_3_BITS, Dst::RS1_3_BITS, 0},
    // Register-register logic and comparison
    {'I', instr_slt,    execute_slt<I>,    OUT_ARITHM, ' ',                   Imm::NO,    Src1::RS1,        Src2::RS2,        Dst::RD,         0},
    {'I', instr_sltu,   execute_sltu<I>,   OUT_ARITHM, ' ',                   Imm::NO,    Src1::RS1,        Src2::RS2,        Dst::RD,         0},
    {'I', instr_and,    execute_and<I>,    OUT_ARITHM, ' ',                   Imm::NO,    Src1::RS1,        Src2::RS2,        Dst::RD,         0},
    {'I', instr_xor,    execute_xor<I>,    OUT_ARITHM, ' ',                   Imm::NO,    Src1::RS1,        Src2::RS2,        Dst::RD,         0},
    {'I', instr_or,     execute_or<I>,     OUT_ARITHM, ' ',                   Imm::NO,    Src1::RS1,        Src2::RS2,        Dst::RD,         0},
    {'I', instr_c_xor,  execute_c_xor<I>,  OUT_ARITHM, ' ',                   Imm::NO,    Src1::ZERO,       Src2::RS2_3_BITS, Dst::RS1_3_BITS, 0},
    {'I', instr_c_or,   execute_c_or<I>,   OUT_ARITHM, ' ',                   Imm::NO,    Src1::ZERO,       Src2::RS2_3_BITS, Dst::RS1_3_BITS, 0},
    // CSR
    {'I', instr_ecall,  execute_ecall<I>,  OUT_BREAK,  ' ',                   Imm::NO,    Src1::ZERO,       Src2::ZERO,       Dst::ZERO,       0},
    {'I', instr_sret,   execute_sret<I>,   OUT_R_JUMP, ' ',                   Imm::NO,    Src1::SEPC,       Src2::ZERO,       Dst::ZERO,       0},
    {'I', instr_mret,   execute_mret<I>,   OUT_R_JUMP, ' ',                   Imm::NO,    Src1::MEPC,       Src2::ZERO,       Dst::ZERO,       0},
    {'I', instr_csrrw,  execute_csrrw<I>,  OUT_ARITHM, ' ',                   Imm::NO,    Src1::RS1,        Src2::CSR,        Dst::CSR,        0},
    {'I', instr_csrrs,  execute_csrrs<I>,  OUT_ARITHM, ' ',                   Imm::NO,    Src1::RS1,        Src2::CSR,        Dst::CSR,        0},
    {'I', instr_csrrwi, execute_csrrwi<I>, OUT_ARITHM, 'C',                   Imm::LOGIC, Src1::ZERO,       Src2::CSR,        Dst::CSR,        0},
    {'I', instr_fence,  execute_fence<I>,  OUT_LOAD,   'I',                   Imm::ADDR,  Src1::RS1,        Src2::ZERO,       Dst::ZERO,       0},
    {'I', instr_fence_i,execute_fence<I>,  OUT_LOAD,   'I',                   Imm::ADDR,  Src1::RS1,        Src2::ZERO,       Dst::ZERO,       0},
    /*-------------- M --------------*/
    {'M', instr_mul,    execute_mul<I>,    OUT_ARITHM, ' ',                   Imm::NO,    Src1::RS1,        Src2::RS2,        Dst::RD,         0},
    {'M', instr_mulh,   execute_mulh<I>,   OUT_ARITHM, ' ',                   Imm::NO,    Src1::RS1,        Src2::RS2,        Dst::RD,         0},
    {'M', instr_mulhsu, execute_mulhsu<I>, OUT_ARITHM, ' ',                   Imm::NO,    Src1::RS1,        Src2::RS2,        Dst::RD,         0},
    {'M', instr_mulhu,  execute_mulhu<I>,  OUT_ARITHM, ' ',                   Imm::NO,    Src1::RS1,        Src2::RS2,        Dst::RD,         0},
    {'M', instr_div,    execute_div<I>,    OUT_ARITHM, ' ',                   Imm::NO,    Src1::RS1,        Src2::RS2,        Dst::RD,         0},
    {'M', instr_divu,   execute_divu<I>,   OUT_ARITHM, ' ',                   Imm::NO,    Src1::RS1,        Src2::RS2,        Dst::RD,         0},
    {'M', instr_rem,    execute_rem<I>,    OUT_ARITHM, ' ',                   Imm::NO,    Src1::RS1,        Src2::RS2,        Dst::RD,         0},
    {'M', instr_remu,   execute_remu<I>,   OUT_ARITHM, ' ',                   Imm::NO,    Src1::RS1,        Src2::RS2,        Dst::RD,         0},
};

template<typename I>
static const RISCVTableEntry<I> invalid_instr =
{'I', {"unknown", 0, 0}, &ALU::unknown_instruction<I>, OUT_ARITHM, ' ', Imm::NO, Src1::ZERO, Src2::ZERO, Dst::ZERO, 0};

template<typename I>
const auto& find_entry( uint32 bytes)
{
    for (const auto& e : cmd_desc<I>)
        if ( e.entry.check_mask( bytes))
            return e;
        
    return invalid_instr<I>;
}

template<typename I>
const auto& find_entry( std::string_view name)
{
    for (const auto& e : cmd_desc<I>)
        if ( e.entry.name == name)
            return e;
        
    return invalid_instr<I>;
}

template<typename T>
RISCVInstr<T>::RISCVInstr( uint32 bytes, Addr PC)
    : BaseInstruction<T, RISCVRegister>( PC, PC + 4), instr( bytes)
{
    const auto& entry = find_entry<MyDatapath>( bytes);
    init( entry);

    RISCVInstrDecoder decoder( bytes);
    this->v_imm = RISCVInstrDecoder::get_immediate<T>( entry.immediate_type, decoder.get_immediate_value( entry.immediate_type));
    this->src1  = decoder.get_register( entry.src1);
    this->src2  = decoder.get_register( entry.src2);
    this->dst   = decoder.get_register( entry.dst);
    if ( entry.dst == Dst::CSR)
        this->dst2 = decoder.get_register( Dst::RD);
    init_target();
}


template<typename T>
RISCVInstr<T>::RISCVInstr( std::string_view name, uint32 immediate, Addr PC)
    : BaseInstruction<T, RISCVRegister>( PC, PC + 4)
{
    const auto& entry = find_entry<MyDatapath>( name);
    init( entry);

    this->v_imm = RISCVInstrDecoder::get_immediate<T>( entry.immediate_type, immediate);
    init_target();
}

template<typename T>
void RISCVInstr<T>::init_target()
{
    if ( this->is_branch())
        this->target = this->PC + sign_extension<12>( narrow_cast<Addr>( this->v_imm));
    else if ( this->is_direct_jump())
        this->target = this->PC + sign_extension<20>( narrow_cast<Addr>( this->v_imm));
}

template<typename T>
void RISCVInstr<T>::init( const RISCVTableEntry<MyDatapath>& entry)
{
    this->imm_print_type = entry.immediate_print_type;
    this->mem_size  = entry.mem_size;
    this->operation = entry.type;
    this->executor  = entry.function;
    this->opname  = entry.entry.name;
    this->print_dst  = ( entry.dst != Dst::ZERO)
                    && ( entry.dst != Dst::MEPC)
                    && ( entry.dst != Dst::SEPC);
    this->print_dst2 = entry.dst == Dst::CSR;
    this->print_src1 = ( entry.src1 != Src1::ZERO)
                    && ( entry.src1 != Src1::SEPC)
                    && ( entry.src1 != Src1::MEPC)
                    && ( entry.src1 != Src1::CSR);
    this->print_src2 = ( entry.src2 != Src2::ZERO)
                    && ( entry.src2 != Src2::SEPC)
                    && ( entry.src2 != Src2::MEPC)
                    && ( entry.src2 != Src2::CSR);
}

template<typename T>
std::string RISCVInstr<T>::get_disasm() const
{
    return this->generate_disasm();
}

template<typename R>
std::string RISCVInstr<R>::string_dump() const
{
    std::ostringstream oss;
    this->dump_content( oss, get_disasm());
    return oss.str();
}

template<typename R>
std::string RISCVInstr<R>::bytes_dump() const
{
     std::ostringstream oss;
     oss << "Bytes:" << std::hex;
     for ( const auto& b : unpack_array<uint32, Endian::little>( instr))
         oss << " 0x" << std::setfill( '0') << std::setw( 2) << static_cast<uint16>( b);
     return oss.str();
}

template class RISCVInstr<uint32>;
template class RISCVInstr<uint64>;
template class RISCVInstr<uint128>;
