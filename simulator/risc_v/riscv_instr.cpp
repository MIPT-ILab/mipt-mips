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

template<typename I> const auto do_nothing = ALU::unknown_instruction<I>;
// I
template<typename I> const auto execute_lui = ALU::upper_immediate<I, 12>;
template<typename I> const auto execute_auipc = ALU::auipc<I>;
template<typename I> const auto execute_jal = ALU::jump_and_link<I, ALU::j<I>>;
template<typename I> const auto execute_jalr = ALU::jump_and_link<I, ALU::riscv_jr<I>>;
template<typename I> const auto execute_beq = ALU::branch<I, ALU::eq<I>>;
template<typename I> const auto execute_bne = ALU::branch<I, ALU::ne<I>>;
template<typename I> const auto execute_blt = ALU::branch<I, ALU::lt<I>>;
template<typename I> const auto execute_bge = ALU::branch<I, ALU::ge<I>>;
template<typename I> const auto execute_bltu = ALU::branch<I, ALU::ltu<I>>;
template<typename I> const auto execute_bgeu = ALU::branch<I, ALU::geu<I>>;
template<typename I> const auto execute_load = ALU::addr<I>;
template<typename I> const auto execute_store = ALU::store_addr<I>;
template<typename I> const auto execute_addi = ALU::riscv_addition_imm<I, typename I::RegisterUInt>;
template<typename I> const auto execute_addiw = ALU::riscv_addition_imm<I, uint32>;
template<typename I> const auto execute_addid = ALU::riscv_addition_imm<I, uint64>;
template<typename I> const auto execute_slti = ALU::set<I, ALU::lti<I>>;
template<typename I> const auto execute_sltiu = ALU::set<I, ALU::ltiu<I>>;
template<typename I> const auto execute_xori = ALU::xori<I>;
template<typename I> const auto execute_ori = ALU::ori<I>;
template<typename I> const auto execute_andi = ALU::andi<I>;
template<typename I> const auto execute_slli = ALU::sll<I, typename I::RegisterUInt>;
template<typename I> const auto execute_slliw = ALU::sll<I, uint32>;
template<typename I> const auto execute_sllid = ALU::sll<I, uint64>;
template<typename I> const auto execute_srli = ALU::srl<I, typename I::RegisterUInt>;
template<typename I> const auto execute_srliw = ALU::srl<I, uint32>;
template<typename I> const auto execute_srlid = ALU::srl<I, uint64>;
template<typename I> const auto execute_srai = ALU::sra<I, typename I::RegisterUInt>;
template<typename I> const auto execute_sraiw = ALU::sra<I, uint32>;
template<typename I> const auto execute_sraid = ALU::sra<I, uint64>;
template<typename I> const auto execute_add = ALU::riscv_addition<I, typename I::RegisterUInt>;
template<typename I> const auto execute_addw = ALU::riscv_addition<I, uint32>;
template<typename I> const auto execute_sub = ALU::riscv_subtraction<I, typename I::RegisterUInt>;
template<typename I> const auto execute_subw = ALU::riscv_subtraction<I, uint32>;
template<typename I> const auto execute_sll = ALU::sllv<I, typename I::RegisterUInt>;
template<typename I> const auto execute_sllw = ALU::sllv<I, uint32>;
template<typename I> const auto execute_slt = ALU::set<I, ALU::lt<I>>;
template<typename I> const auto execute_sltu = ALU::set<I, ALU::ltu<I>>;
template<typename I> const auto execute_xor = ALU::xorv<I>;
template<typename I> const auto execute_srl = ALU::srlv<I, typename I::RegisterUInt>;
template<typename I> const auto execute_srlw = ALU::srlv<I, uint32>;
template<typename I> const auto execute_sra = ALU::srav<I, typename I::RegisterUInt>;
template<typename I> const auto execute_sraw = ALU::srav<I, uint32>;
template<typename I> const auto execute_or = ALU::orv<I>;
template<typename I> const auto execute_and = ALU::andv<I>;
// System I
template<typename I> const auto execute_ecall = ALU::syscall<I>;
template<typename I> const auto execute_ebreak = ALU::breakpoint<I>;
template<typename I> const auto execute_uret = ALU::jump_and_link<I, ALU::riscv_jr<I>>;
template<typename I> const auto execute_sret = ALU::jump_and_link<I, ALU::riscv_jr<I>>;
template<typename I> const auto execute_mret = ALU::jump_and_link<I, ALU::riscv_jr<I>>;
template<typename I> const auto execute_wfi = do_nothing<I>;
template<typename I> const auto execute_fence = ALU::addr<I>;
template<typename I> const auto execute_csrrw = ALU::csrrw<I>;
template<typename I> const auto execute_csrrs = ALU::csrrs<I>;
template<typename I> const auto execute_csrrc = do_nothing<I>;
template<typename I> const auto execute_csrrwi = ALU::csrrwi<I>;
template<typename I> const auto execute_csrrsi = do_nothing<I>;
template<typename I> const auto execute_csrrci = do_nothing<I>;
// M
template<typename I> const auto execute_mul = RISCVMultALU::mult_l<I, typename I::RegisterUInt>;
template<typename I> const auto execute_mulh = RISCVMultALU::mult_h_ss<I, typename I::RegisterUInt>;
template<typename I> const auto execute_mulhsu = RISCVMultALU::mult_h_su<I, typename I::RegisterUInt>;
template<typename I> const auto execute_mulhu = RISCVMultALU::mult_h_uu<I, typename I::RegisterUInt>;
template<typename I> const auto execute_div = RISCVMultALU::div<I, sign_t<typename I::RegisterUInt>>;
template<typename I> const auto execute_divu = RISCVMultALU::div<I, typename I::RegisterUInt>;
template<typename I> const auto execute_rem = RISCVMultALU::rem<I, sign_t<typename I::RegisterUInt>>;
template<typename I> const auto execute_remu = RISCVMultALU::rem<I, typename I::RegisterUInt>;
// B
template<typename I> const auto execute_bfp = ALU::bit_field_place<I>;
template<typename I> const auto execute_clmul = ALU::clmul<I, typename I::RegisterUInt>;
template<typename I> const auto execute_clz = ALU::clz<I, typename I::RegisterUInt>;
template<typename I> const auto execute_ctz = ALU::ctz<I, typename I::RegisterUInt>;
template<typename I> const auto execute_gorc = ALU::gorc<I>;
template<typename I> const auto execute_gorci = ALU::gorci<I>;
template<typename I> const auto execute_grev = ALU::grev<I>;
template<typename I> const auto execute_max = ALU::max<I>;
template<typename I> const auto execute_maxu = ALU::maxu<I>;
template<typename I> const auto execute_orn = ALU::orn<I>;
template<typename I> const auto execute_pack = ALU::pack<I, typename I::RegisterUInt>;
template<typename I> const auto execute_pcnt = ALU::pcnt<I, typename I::RegisterUInt>;
template<typename I> const auto execute_rol = ALU::rol<I>;
template<typename I> const auto execute_ror = ALU::ror<I>;
template<typename I> const auto execute_sbext = ALU::sbext<I>;
template<typename I> const auto execute_slo = ALU::slo<I>;
template<typename I> const auto execute_sro = ALU::sro<I>;
template<typename I> const auto execute_unshfl = ALU::riscv_unshfl<I>;
template<typename I> const auto execute_xnor = ALU::xnor<I>;

using Src = Reg;
using Dst = Reg;

struct RISCVAutogeneratedTableEntry
{
    std::string_view name;
    uint32 match;
    uint32 mask;

    bool check_mask( uint32 bytes) const noexcept
    {
        return ( bytes & mask) == match;
    }
};

#define PASTER(x,y) x ## y
#define EVALUATOR(x,y) PASTER(x,y)

#define DECLARE_INSN(name, match, mask) \
static const RISCVAutogeneratedTableEntry instr_ ## name = { #name, match, mask };
#include <riscv.opcode.gen.h>
#undef DECLARE_INSN

static const RISCVAutogeneratedTableEntry instr_invalid = { "unknown", 0x0, 0xffff };

template<typename I>
static const RISCVTableEntry<I> invalid_instr = {'I', instr_invalid, do_nothing<I>, OUT_ARITHM, ' ', Imm::NO, { Src::ZERO, Src::ZERO }, { Dst::ZERO }, 0, 32 | 64 | 128}; // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977

template<typename I>
struct RISCVTableEntry
{
    char subset = 'I';
    RISCVAutogeneratedTableEntry entry = instr_invalid;
    ALU::Execute<I> function = do_nothing<I>;
    OperationType type = OUT_ARITHM;
    char immediate_type = ' ';
    Imm immediate_print_type = Imm::NO;
    std::vector<Src::Type> src = { Src::ZERO, Src::ZERO };
    std::vector<Dst::Type> dst = { Dst::ZERO };
    uint32 mem_size = 0;
    uint32 bit_width = 32 | 64 | 128; // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977;

    bool check_print_dst( size_t dst_id) const
    {
        Dst::Type reg = dst.at( dst_id);
        return reg != Reg::ZERO
            && reg != Reg::MEPC
            && reg != Reg::SEPC
            && reg != Reg::RA;
    }

    bool check_print_src( size_t src_id) const
    {
        Src::Type reg = src.at( src_id);

        if ( subset == 'C' && reg == dst.at( 0))
            return false;

        return reg != Reg::ZERO
            && reg != Reg::SEPC
            && reg != Reg::MEPC
            && reg != Reg::CSR;
    }

    size_t num_dst() const { return dst.size(); }
    size_t num_src() const { return src.size(); }

    bool check_mask_and_type( uint32 bytes) const noexcept
    {
        return entry.check_mask( bytes) && (bitwidth<typename I::RegisterUInt> & bit_width) != 0;
    }
};

template<typename I>
static const std::vector<RISCVTableEntry<I>> cmd_desc =
{
    /*-------------- I --------------*/
    { }, // invalid instruction
    {'I', instr_lui,        execute_lui<I>,    OUT_ARITHM, 'U', Imm::LOGIC, { Src::ZERO, Src::ZERO }, { Dst::RD },            0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_auipc,      execute_auipc<I>,  OUT_ARITHM, 'U', Imm::LOGIC, { Src::ZERO, Src::ZERO }, { Dst::RD },            0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    // Jumps and branches
    {'I', instr_jal,        execute_jal<I>,    OUT_J_JUMP, 'J', Imm::ARITH, { Src::ZERO, Src::ZERO }, { Dst::RD },            0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_jalr,       execute_jalr<I>,   OUT_R_JUMP, 'I', Imm::LOGIC, { Src::RS1,  Src::ZERO }, { Dst::RD },            0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_beq,        execute_beq<I>,    OUT_BRANCH, 'B', Imm::ARITH, { Src::RS1,  Src::RS2 },  { Dst::ZERO },          0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_bne,        execute_bne<I>,    OUT_BRANCH, 'B', Imm::ARITH, { Src::RS1,  Src::RS2 },  { Dst::ZERO },          0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_blt,        execute_blt<I>,    OUT_BRANCH, 'B', Imm::ARITH, { Src::RS1,  Src::RS2 },  { Dst::ZERO },          0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_bge,        execute_bge<I>,    OUT_BRANCH, 'B', Imm::ARITH, { Src::RS1,  Src::RS2 },  { Dst::ZERO },          0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_bltu,       execute_bltu<I>,   OUT_BRANCH, 'B', Imm::ARITH, { Src::RS1,  Src::RS2 },  { Dst::ZERO },          0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_bgeu,       execute_bgeu<I>,   OUT_BRANCH, 'B', Imm::ARITH, { Src::RS1,  Src::RS2 },  { Dst::ZERO },          0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    // Loads
    {'I', instr_lb,         execute_load<I>,   OUT_LOAD,   'I', Imm::ADDR,  { Src::RS1,  Src::ZERO }, { Dst::RD },            1, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_lh,         execute_load<I>,   OUT_LOAD,   'I', Imm::ADDR,  { Src::RS1,  Src::ZERO }, { Dst::RD },            2, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_lw,         execute_load<I>,   OUT_LOAD,   'I', Imm::ADDR,  { Src::RS1,  Src::ZERO }, { Dst::RD },            4, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_ld,         execute_load<I>,   OUT_LOAD,   'I', Imm::ADDR,  { Src::RS1,  Src::ZERO }, { Dst::RD },            8,      64 | 128},
    // Unsigned loads
    {'I', instr_lbu,        execute_load<I>,   OUT_LOADU,  'I', Imm::ADDR,  { Src::RS1,  Src::ZERO }, { Dst::RD },            1, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_lhu,        execute_load<I>,   OUT_LOADU,  'I', Imm::ADDR,  { Src::RS1,  Src::ZERO }, { Dst::RD },            2, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_lwu,        execute_load<I>,   OUT_LOADU,  'I', Imm::ADDR,  { Src::RS1,  Src::ZERO }, { Dst::RD },            4, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    // Stores
    {'I', instr_sb,         execute_store<I>,  OUT_STORE,  'S', Imm::ADDR,  { Src::RS1,  Src::RS2 },  { Dst::ZERO },          1, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_sh,         execute_store<I>,  OUT_STORE,  'S', Imm::ADDR,  { Src::RS1,  Src::RS2 },  { Dst::ZERO },          2, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_sw,         execute_store<I>,  OUT_STORE,  'S', Imm::ADDR,  { Src::RS1,  Src::RS2 },  { Dst::ZERO },          4, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_sd,         execute_store<I>,  OUT_STORE,  'S', Imm::ADDR,  { Src::RS1,  Src::RS2 },  { Dst::ZERO },          8,      64 | 128},
    // Immediate arithmetics
    {'I', instr_addi,       execute_addi<I>,   OUT_ARITHM, 'I', Imm::ARITH, { Src::RS1,  Src::ZERO }, { Dst::RD },            0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_slli,       execute_slli<I>,   OUT_ARITHM, '7', Imm::ARITH, { Src::RS1,  Src::ZERO }, { Dst::RD },            0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_srli,       execute_srli<I>,   OUT_ARITHM, '7', Imm::ARITH, { Src::RS1,  Src::ZERO }, { Dst::RD },            0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_srai,       execute_srai<I>,   OUT_ARITHM, '7', Imm::ARITH, { Src::RS1,  Src::ZERO }, { Dst::RD },            0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_addiw,      execute_addiw<I>,  OUT_ARITHM, 'I', Imm::ARITH, { Src::RS1,  Src::ZERO }, { Dst::RD },            0,      64 | 128},
    {'I', instr_slliw,      execute_slliw<I>,  OUT_ARITHM, '5', Imm::ARITH, { Src::RS1,  Src::ZERO }, { Dst::RD },            0,      64 | 128},
    {'I', instr_srliw,      execute_srliw<I>,  OUT_ARITHM, '5', Imm::ARITH, { Src::RS1,  Src::ZERO }, { Dst::RD },            0,      64 | 128},
    {'I', instr_sraiw,      execute_sraiw<I>,  OUT_ARITHM, '5', Imm::ARITH, { Src::RS1,  Src::ZERO }, { Dst::RD },            0,      64 | 128},
    {'I', instr_addid,      execute_addid<I>,  OUT_ARITHM, 'I', Imm::ARITH, { Src::RS1,  Src::ZERO }, { Dst::RD },            0,           128},
    {'I', instr_sllid,      execute_sllid<I>,  OUT_ARITHM, '6', Imm::ARITH, { Src::RS1,  Src::ZERO }, { Dst::RD },            0,           128},
    {'I', instr_srlid,      execute_srlid<I>,  OUT_ARITHM, '6', Imm::ARITH, { Src::RS1,  Src::ZERO }, { Dst::RD },            0,           128},
    {'I', instr_sraid,      execute_sraid<I>,  OUT_ARITHM, '6', Imm::ARITH, { Src::RS1,  Src::ZERO }, { Dst::RD },            0,           128},
    // Immediate logic and comparison
    {'I', instr_slti,       execute_slti<I>,   OUT_ARITHM, 'I', Imm::ARITH, { Src::RS1,  Src::ZERO }, { Dst::RD },            0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_sltiu,      execute_sltiu<I>,  OUT_ARITHM, 'I', Imm::ARITH, { Src::RS1,  Src::ZERO }, { Dst::RD },            0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_xori,       execute_xori<I>,   OUT_ARITHM, 'I', Imm::LOGIC, { Src::RS1,  Src::ZERO }, { Dst::RD },            0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_ori,        execute_ori<I>,    OUT_ARITHM, 'I', Imm::LOGIC, { Src::RS1,  Src::ZERO }, { Dst::RD },            0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_andi,       execute_andi<I>,   OUT_ARITHM, 'I', Imm::LOGIC, { Src::RS1,  Src::ZERO }, { Dst::RD },            0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    // Register-register arithmetics
    {'I', instr_add,        execute_add<I>,    OUT_ARITHM, ' ', Imm::NO,    { Src::RS1,  Src::RS2 },  { Dst::RD },            0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_sub,        execute_sub<I>,    OUT_ARITHM, ' ', Imm::NO,    { Src::RS1,  Src::RS2 },  { Dst::RD },            0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_sll,        execute_sll<I>,    OUT_ARITHM, ' ', Imm::NO,    { Src::RS1,  Src::RS2 },  { Dst::RD },            0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_sra,        execute_sra<I>,    OUT_ARITHM, ' ', Imm::NO,    { Src::RS1,  Src::RS2 },  { Dst::RD },            0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_srl,        execute_srl<I>,    OUT_ARITHM, ' ', Imm::NO,    { Src::RS1,  Src::RS2 },  { Dst::RD },            0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_addw,       execute_addw<I>,   OUT_ARITHM, ' ', Imm::NO,    { Src::RS1,  Src::RS2 },  { Dst::RD },            0,      64 | 128},
    {'I', instr_subw,       execute_subw<I>,   OUT_ARITHM, ' ', Imm::NO,    { Src::RS1,  Src::RS2 },  { Dst::RD },            0,      64 | 128},
    {'I', instr_sllw,       execute_sllw<I>,   OUT_ARITHM, ' ', Imm::NO,    { Src::RS1,  Src::RS2 },  { Dst::RD },            0,      64 | 128},
    {'I', instr_sraw,       execute_sraw<I>,   OUT_ARITHM, ' ', Imm::NO,    { Src::RS1,  Src::RS2 },  { Dst::RD },            0,      64 | 128},
    {'I', instr_srlw,       execute_srlw<I>,   OUT_ARITHM, ' ', Imm::NO,    { Src::RS1,  Src::RS2 },  { Dst::RD },            0,      64 | 128},
    // Register-register logic and comparison
    {'I', instr_slt,        execute_slt<I>,    OUT_ARITHM, ' ', Imm::NO,    { Src::RS1,  Src::RS2 },  { Dst::RD },            0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_sltu,       execute_sltu<I>,   OUT_ARITHM, ' ', Imm::NO,    { Src::RS1,  Src::RS2 },  { Dst::RD },            0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_and,        execute_and<I>,    OUT_ARITHM, ' ', Imm::NO,    { Src::RS1,  Src::RS2 },  { Dst::RD },            0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_xor,        execute_xor<I>,    OUT_ARITHM, ' ', Imm::NO,    { Src::RS1,  Src::RS2 },  { Dst::RD },            0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_or,         execute_or<I>,     OUT_ARITHM, ' ', Imm::NO,    { Src::RS1,  Src::RS2 },  { Dst::RD },            0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    // CSR
    {'I', instr_ecall,      execute_ecall<I>,  OUT_TRAP,   ' ', Imm::NO,    { Src::ZERO, Src::ZERO }, { Dst::ZERO },          0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_ebreak,     execute_ebreak<I>, OUT_TRAP,   ' ', Imm::NO,    { Src::ZERO, Src::ZERO }, { Dst::ZERO },          0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_sret,       execute_sret<I>,   OUT_R_JUMP, ' ', Imm::NO,    { Src::SEPC, Src::ZERO }, { Dst::ZERO },          0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_mret,       execute_mret<I>,   OUT_R_JUMP, ' ', Imm::NO,    { Src::MEPC, Src::ZERO }, { Dst::ZERO },          0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_csrrw,      execute_csrrw<I>,  OUT_ARITHM, ' ', Imm::NO,    { Src::RS1,  Src::CSR },  { Dst::CSR, Dst::RD },  0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_csrrs,      execute_csrrs<I>,  OUT_ARITHM, ' ', Imm::NO,    { Src::RS1,  Src::CSR },  { Dst::CSR, Dst::RD },  0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_csrrwi,     execute_csrrwi<I>, OUT_ARITHM, 'C', Imm::LOGIC, { Src::ZERO, Src::CSR },  { Dst::CSR, Dst::RD },  0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_fence,      execute_fence<I>,  OUT_LOAD,   'I', Imm::ADDR,  { Src::RS1,  Src::ZERO }, { Dst::ZERO },          0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'I', instr_fence_i,    execute_fence<I>,  OUT_LOAD,   'I', Imm::ADDR,  { Src::RS1,  Src::ZERO }, { Dst::ZERO },          0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    /*-------------- M --------------*/
    {'M', instr_mul,        execute_mul<I>,    OUT_ARITHM, ' ', Imm::NO,    { Src::RS1,  Src::RS2 },  { Dst::RD },            0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'M', instr_mulh,       execute_mulh<I>,   OUT_ARITHM, ' ', Imm::NO,    { Src::RS1,  Src::RS2 },  { Dst::RD },            0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'M', instr_mulhsu,     execute_mulhsu<I>, OUT_ARITHM, ' ', Imm::NO,    { Src::RS1,  Src::RS2 },  { Dst::RD },            0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'M', instr_mulhu,      execute_mulhu<I>,  OUT_ARITHM, ' ', Imm::NO,    { Src::RS1,  Src::RS2 },  { Dst::RD },            0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'M', instr_div,        execute_div<I>,    OUT_ARITHM, ' ', Imm::NO,    { Src::RS1,  Src::RS2 },  { Dst::RD },            0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'M', instr_divu,       execute_divu<I>,   OUT_ARITHM, ' ', Imm::NO,    { Src::RS1,  Src::RS2 },  { Dst::RD },            0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'M', instr_rem,        execute_rem<I>,    OUT_ARITHM, ' ', Imm::NO,    { Src::RS1,  Src::RS2 },  { Dst::RD },            0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'M', instr_remu,       execute_remu<I>,   OUT_ARITHM, ' ', Imm::NO,    { Src::RS1,  Src::RS2 },  { Dst::RD },            0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    /*-------------- C --------------*/
    // Breakpoint
    {'C', instr_c_ebreak,   execute_ebreak<I>, OUT_BREAK,  ' ',                       Imm::NO,       { Src::ZERO,     Src::ZERO },     { Dst::ZERO },     0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    // NOP
    {'C', instr_c_nop,      execute_or<I>,     OUT_ARITHM, ' ',                       Imm::NO,       { Src::ZERO,     Src::ZERO },     { Dst::ZERO },     0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    // Jumps and branches
    {'C', instr_c_j,        execute_jal<I>,    OUT_BRANCH, ImmediateType::C_J,        Imm::JUMP_REL, { Src::ZERO,     Src::ZERO },     { Dst::ZERO },     0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'C', instr_c_jal,      execute_jal<I>,    OUT_BRANCH, ImmediateType::C_J,        Imm::JUMP_REL, { Src::ZERO,     Src::ZERO },     { Dst::RA },       0, 32           },
    {'C', instr_c_jr,       execute_jalr<I>,   OUT_BRANCH, ' ',                       Imm::NO,       { Src::RD,       Src::ZERO },     { Dst::ZERO },     0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'C', instr_c_jalr,     execute_jalr<I>,   OUT_BRANCH, ' ',                       Imm::NO,       { Src::RD,       Src::ZERO },     { Dst::RA },       0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'C', instr_c_beqz,     execute_beq<I>,    OUT_BRANCH, ImmediateType::C_B,        Imm::ARITH,    { Src::RS1_3BIT, Src::ZERO },     { Dst::ZERO },     0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'C', instr_c_bnez,     execute_bne<I>,    OUT_BRANCH, ImmediateType::C_B,        Imm::ARITH,    { Src::RS1_3BIT, Src::ZERO },     { Dst::ZERO },     0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    // Loads
    {'C', instr_c_lwsp,     execute_load<I>,   OUT_LOAD,   ImmediateType::C_LWSP,     Imm::ADDR,     { Src::SP,       Src::ZERO },     { Dst::RD },       4, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'C', instr_c_ldsp,     execute_load<I>,   OUT_LOAD,   ImmediateType::C_LDSP,     Imm::ADDR,     { Src::SP,       Src::ZERO },     { Dst::RD },       8,      64 | 128},
    {'C', instr_c_lqsp,     execute_load<I>,   OUT_LOAD,   ImmediateType::C_LQSP,     Imm::ADDR,     { Src::SP,       Src::ZERO },     { Dst::RD },       16,          128},
    {'C', instr_c_lw,       execute_load<I>,   OUT_LOAD,   ImmediateType::C_LW,       Imm::ADDR,     { Src::RS1_3BIT, Src::ZERO },     { Dst::RD_3BIT },  4, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'C', instr_c_ld,       execute_load<I>,   OUT_LOAD,   ImmediateType::C_LD,       Imm::ADDR,     { Src::RS1_3BIT, Src::ZERO },     { Dst::RD_3BIT },  8,      64 | 128},
    {'C', instr_c_lq,       execute_load<I>,   OUT_LOAD,   ImmediateType::C_LQ,       Imm::ADDR,     { Src::RS1_3BIT, Src::ZERO },     { Dst::RD_3BIT },  16,          128},
    // Stores
    {'C', instr_c_swsp,     execute_store<I>,  OUT_STORE,  ImmediateType::C_SWSP,     Imm::ADDR,     { Src::SP,       Src::RS2_CMP },  { Dst::ZERO },     4, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'C', instr_c_sdsp,     execute_store<I>,  OUT_STORE,  ImmediateType::C_SDSP,     Imm::ADDR,     { Src::SP,       Src::RS2_CMP },  { Dst::ZERO },     8,      64 | 128},
    {'C', instr_c_sqsp,     execute_store<I>,  OUT_STORE,  ImmediateType::C_SQSP,     Imm::ADDR,     { Src::SP,       Src::RS2_CMP },  { Dst::ZERO },     16,          128},
    {'C', instr_c_sw,       execute_store<I>,  OUT_STORE,  ImmediateType::C_LW,       Imm::ADDR,     { Src::RS1_3BIT, Src::RS2_3BIT }, { Dst::ZERO },     4, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'C', instr_c_sd,       execute_store<I>,  OUT_STORE,  ImmediateType::C_LD,       Imm::ADDR,     { Src::RS1_3BIT, Src::RS2_3BIT }, { Dst::ZERO },     8,      64 | 128},
    {'C', instr_c_sq,       execute_store<I>,  OUT_STORE,  ImmediateType::C_LQ,       Imm::ADDR,     { Src::RS1_3BIT, Src::RS2_3BIT }, { Dst::ZERO },     16,          128},
    // Immediate arithmetics
    {'C', instr_c_addi16sp, execute_addi<I>,   OUT_ARITHM, ImmediateType::C_ADDI16SP, Imm::ARITH,    { Src::SP,       Src::ZERO },     { Dst::SP },       0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'C', instr_c_addi,     execute_addi<I>,   OUT_ARITHM, ImmediateType::C_I,        Imm::ARITH,    { Src::RD,       Src::ZERO },     { Dst::RD },       0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'C', instr_c_srli,     execute_srli<I>,   OUT_ARITHM, ImmediateType::C_S,        Imm::ARITH,    { Src::RS1_3BIT, Src::ZERO },     { Dst::RS1_3BIT }, 0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'C', instr_c_srai,     execute_srai<I>,   OUT_ARITHM, ImmediateType::C_S,        Imm::ARITH,    { Src::RS1_3BIT, Src::ZERO },     { Dst::RS1_3BIT }, 0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'C', instr_c_slli,     execute_slli<I>,   OUT_ARITHM, ImmediateType::C_S,        Imm::ARITH,    { Src::RD,       Src::ZERO },     { Dst::RD },       0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'C', instr_c_addi4spn, execute_addi<I>,   OUT_ARITHM, ImmediateType::C_ADDI4SPN, Imm::ARITH,    { Src::SP,       Src::ZERO },     { Dst::RD_3BIT },  0, 32 | 64      },
    {'C', instr_c_addiw,    execute_addiw<I>,  OUT_ARITHM, ImmediateType::C_I,        Imm::ARITH,    { Src::RD,       Src::ZERO },     { Dst::RD },       0,      64 | 128},
    // Constant-Generation
    {'C', instr_c_li,       execute_addi<I>,   OUT_ARITHM, ImmediateType::C_I,        Imm::ARITH,    { Src::ZERO,     Src::ZERO },     { Dst::RD },       0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'C', instr_c_lui,      execute_lui<I>,    OUT_ARITHM, ImmediateType::C_I,        Imm::LOGIC,    { Src::ZERO,     Src::ZERO },     { Dst::RD },       0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    // Immediate logic and comparison
    {'C', instr_c_andi,     execute_andi<I>,   OUT_ARITHM, ImmediateType::C_I,        Imm::ARITH,    { Src::RS1_3BIT, Src::ZERO },     { Dst::RS1_3BIT }, 0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    // Register-register arithmetics
    {'C', instr_c_sub,      execute_sub<I>,    OUT_ARITHM, ' ',                       Imm::NO,       { Src::RS1_3BIT, Src::RS2_3BIT }, { Dst::RS1_3BIT }, 0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'C', instr_c_addw,     execute_addw<I>,   OUT_ARITHM, ' ',                       Imm::NO,       { Src::RS1_3BIT, Src::RS2_3BIT }, { Dst::RS1_3BIT }, 0,      64 | 128},
    {'C', instr_c_subw,     execute_subw<I>,   OUT_ARITHM, ' ',                       Imm::NO,       { Src::RS1_3BIT, Src::RS2_3BIT }, { Dst::RS1_3BIT }, 0,      64 | 128},
    {'C', instr_c_mv,       execute_or<I>,     OUT_ARITHM, ' ',                       Imm::NO,       { Src::ZERO,     Src::RS2_CMP },  { Dst::RD },       0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'C', instr_c_add,      execute_add<I>,    OUT_ARITHM, ' ',                       Imm::NO,       { Src::RD,       Src::RS2_CMP },  { Dst::RD },       0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    // Register-register logic and comparison
    {'C', instr_c_xor,      execute_xor<I>,    OUT_ARITHM, ' ',                       Imm::NO,       { Src::RS1_3BIT, Src::RS2_3BIT }, { Dst::RS1_3BIT }, 0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'C', instr_c_or,       execute_or<I>,     OUT_ARITHM, ' ',                       Imm::NO,       { Src::RS1_3BIT, Src::RS2_3BIT }, { Dst::RS1_3BIT }, 0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'C', instr_c_and,      execute_and<I>,    OUT_ARITHM, ' ',                       Imm::NO,       { Src::RS1_3BIT, Src::RS2_3BIT }, { Dst::RS1_3BIT }, 0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    /*-------------- B --------------*/
    // Bit manipulation
    {'B', instr_slo,        execute_slo<I>,    OUT_ARITHM, ' ', Imm::NO, { Src::RS1, Src::RS2 },  { Dst::RD }, 0, 32 | 64      },
    {'B', instr_sro,        execute_sro<I>,    OUT_ARITHM, ' ', Imm::NO, { Src::RS1, Src::RS2 },  { Dst::RD }, 0, 32 | 64      },
    {'B', instr_orn,        execute_orn<I>,    OUT_ARITHM, ' ', Imm::NO, { Src::RS1, Src::RS2 },  { Dst::RD }, 0, 32 | 64      },
    {'B', instr_sbext,      execute_sbext<I>,  OUT_ARITHM, ' ', Imm::NO, { Src::RS1, Src::RS2 },  { Dst::RD }, 0, 32 | 64      },
    {'B', instr_pack,       execute_pack<I>,   OUT_ARITHM, ' ', Imm::NO, { Src::RS1, Src::RS2 },  { Dst::RD }, 0, 32 | 64      },
    {'B', instr_xnor,       execute_xnor<I>,   OUT_ARITHM, ' ', Imm::NO, { Src::RS1, Src::RS2 },  { Dst::RD }, 0, 32 | 64      },
    {'B', instr_bfp,        execute_bfp<I>,    OUT_ARITHM, ' ', Imm::NO, { Src::RS1, Src::RS2 },  { Dst::RD }, 0, 32 | 64      },
    {'B', instr_grev,       execute_grev<I>,   OUT_ARITHM, ' ', Imm::NO, { Src::RS1, Src::RS2 },  { Dst::RD }, 0, 32 | 64      },
    {'B', instr_pcnt,       execute_pcnt<I>,   OUT_ARITHM, ' ', Imm::NO, { Src::RS1, Src::RS2 },  { Dst::RD }, 0, 32 | 64      },
    {'B', instr_clz,        execute_clz<I>,    OUT_ARITHM, ' ', Imm::NO, { Src::RS1, Src::ZERO }, { Dst::RD }, 0, 32 | 64      },
    {'B', instr_ctz,        execute_ctz<I>,    OUT_ARITHM, ' ', Imm::NO, { Src::RS1, Src::ZERO }, { Dst::RD }, 0, 32 | 64      },
    {'B', instr_rol,        execute_rol<I>,    OUT_ARITHM, ' ', Imm::NO, { Src::RS1, Src::RS2 },  { Dst::RD }, 0, 32 | 64      },
    {'B', instr_ror,        execute_ror<I>,    OUT_ARITHM, ' ', Imm::NO, { Src::RS1, Src::RS2 },  { Dst::RD }, 0, 32 | 64      },
    {'B', instr_clmul,      execute_clmul<I>,  OUT_ARITHM, ' ', Imm::NO, { Src::RS1, Src::RS2 },  { Dst::RD }, 0, 32 | 64      },
    {'B', instr_max,        execute_max<I>,    OUT_ARITHM, ' ', Imm::NO, { Src::RS1, Src::RS2 },  { Dst::RD }, 0, 32 | 64      },
    {'B', instr_maxu,       execute_maxu<I>,   OUT_ARITHM, ' ', Imm::NO, { Src::RS1, Src::RS2 },  { Dst::RD }, 0, 32 | 64      },
    {'B', instr_gorc,       execute_gorc<I>,   OUT_ARITHM, ' ', Imm::NO, { Src::RS1, Src::RS2 },  { Dst::RD }, 0, 32 | 64      },
    {'B', instr_gorci,      execute_gorci<I>,  OUT_ARITHM, '7', Imm::ARITH, { Src::RS1, Src::ZERO }, { Dst::RD }, 0, 32 | 64   },
    {'B', instr_unshfl,     execute_unshfl<I>, OUT_ARITHM, ' ', Imm::NO, { Src::RS1, Src::RS2 },  { Dst::RD }, 0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
};

template<typename I>
const auto& find_entry( uint32 bytes)
{
    for ( const auto& e : cmd_desc<I>)
        if ( e.check_mask_and_type( bytes))
            return e;

    return invalid_instr<I>;
}

template<typename I>
const auto& find_entry( std::string_view name)
{
    for ( const auto& e : cmd_desc<I>)
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

    for ( size_t i = 0; i < entry.num_src(); i++)
        this->src.at( i) = decoder.get_register( entry.src.at( i));
    for ( size_t i = 0; i < entry.num_dst(); i++)
        this->dst.at( i) = decoder.get_register( entry.dst.at( i));

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
    if (entry.subset == 'C')
        this->new_PC = this->PC + 2;

    this->imm_print_type = entry.immediate_print_type;
    this->mem_size = entry.mem_size;
    this->set_type( entry.type);
    this->executor = entry.function;
    this->opname = entry.entry.name;

    for ( size_t i = 0; i < entry.num_dst(); i++)
        if ( entry.check_print_dst( i))
            this->print_dst.set( i);
    for ( size_t i = 0; i < entry.num_src(); i++)
        if ( entry.check_print_src( i))
            this->print_src.set( i);
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
     for ( const auto& b : unpack_array<uint32, std::endian::little>( instr))
         oss << " 0x" << std::setfill( '0') << std::setw( 2) << static_cast<uint16>( b);
     return oss.str();
}

template class RISCVInstr<uint32>;
template class RISCVInstr<uint64>;
template class RISCVInstr<uint128>;
