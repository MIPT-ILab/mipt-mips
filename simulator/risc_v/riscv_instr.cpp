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

/*  Reducing number of ALU instantiations. ALU modifies
 * only Datapath, so we do not need different instantiations
 * for RISCV and MIPS, if register sizes are the same */
template <typename I>
using RISCVALU = ALU<Datapath<typename I::RegisterUInt>>;

/*  Note. Error "expected primary-expression before '...'"
 * on GCC means that you forgot temlate keyword after RISCVALU<I>
 * while refering to template function " */

template<typename I> const auto do_nothing = RISCVALU<I>::unknown_instruction;
// I
template<typename I> const auto execute_lui = RISCVALU<I>::template upper_immediate<12>;
template<typename I> const auto execute_auipc = RISCVALU<I>::auipc;
template<typename I> const auto execute_jal = RISCVALU<I>::template jump_and_link<RISCVALU<I>::j>;
template<typename I> const auto execute_jalr = RISCVALU<I>::template jump_and_link<RISCVALU<I>::riscv_jr>;
template<typename I> const auto execute_beq = RISCVALU<I>::template branch<RISCVALU<I>::eq>;
template<typename I> const auto execute_bne = RISCVALU<I>::template branch<RISCVALU<I>::ne>;
template<typename I> const auto execute_blt = RISCVALU<I>::template branch<RISCVALU<I>::lt>;
template<typename I> const auto execute_bge = RISCVALU<I>::template branch<RISCVALU<I>::ge>;
template<typename I> const auto execute_bltu = RISCVALU<I>::template branch<RISCVALU<I>::ltu>;
template<typename I> const auto execute_bgeu = RISCVALU<I>::template branch<RISCVALU<I>::geu>;
template<typename I> const auto execute_load = RISCVALU<I>::addr;
template<typename I> const auto execute_store = RISCVALU<I>::store_addr;
template<typename I> const auto execute_addi = RISCVALU<I>::template riscv_addition_imm<typename I::RegisterUInt>;
template<typename I> const auto execute_addiw = RISCVALU<I>::template riscv_addition_imm<uint32>;
template<typename I> const auto execute_addid = RISCVALU<I>::template riscv_addition_imm<uint64>;
template<typename I> const auto execute_slti = RISCVALU<I>::template set<RISCVALU<I>::lti>;
template<typename I> const auto execute_sltiu = RISCVALU<I>::template set<RISCVALU<I>::ltiu>;
template<typename I> const auto execute_xori = RISCVALU<I>::xori;
template<typename I> const auto execute_ori = RISCVALU<I>::ori;
template<typename I> const auto execute_andi = RISCVALU<I>::andi;
template<typename I> const auto execute_slli = RISCVALU<I>::template sll<typename I::RegisterUInt>;
template<typename I> const auto execute_slliw = RISCVALU<I>::template sll<uint32>;
template<typename I> const auto execute_sllid = RISCVALU<I>::template sll<uint64>;
template<typename I> const auto execute_srli = RISCVALU<I>::template srl<typename I::RegisterUInt>;
template<typename I> const auto execute_srliw = RISCVALU<I>::template srl<uint32>;
template<typename I> const auto execute_srlid = RISCVALU<I>::template srl<uint64>;
template<typename I> const auto execute_srai = RISCVALU<I>::template sra<typename I::RegisterUInt>;
template<typename I> const auto execute_sraiw = RISCVALU<I>::template sra<uint32>;
template<typename I> const auto execute_sraid = RISCVALU<I>::template sra<uint64>;
template<typename I> const auto execute_add = RISCVALU<I>::template riscv_addition<typename I::RegisterUInt>;
template<typename I> const auto execute_addw = RISCVALU<I>::template riscv_addition<uint32>;
template<typename I> const auto execute_sub = RISCVALU<I>::template riscv_subtraction<typename I::RegisterUInt>;
template<typename I> const auto execute_subw = RISCVALU<I>::template riscv_subtraction<uint32>;
template<typename I> const auto execute_sll = RISCVALU<I>::template sllv<typename I::RegisterUInt>;
template<typename I> const auto execute_sllw = RISCVALU<I>::template sllv<uint32>;
template<typename I> const auto execute_slt = RISCVALU<I>::template set<RISCVALU<I>::lt>;
template<typename I> const auto execute_sltu = RISCVALU<I>::template set<RISCVALU<I>::ltu>;
template<typename I> const auto execute_xor = RISCVALU<I>::xorv;
template<typename I> const auto execute_srl = RISCVALU<I>::template srlv<typename I::RegisterUInt>;
template<typename I> const auto execute_srlw = RISCVALU<I>::template srlv<uint32>;
template<typename I> const auto execute_sra = RISCVALU<I>::template srav<typename I::RegisterUInt>;
template<typename I> const auto execute_sraw = RISCVALU<I>::template srav<uint32>;
template<typename I> const auto execute_or = RISCVALU<I>::orv;
template<typename I> const auto execute_and = RISCVALU<I>::andv;
// System I
template<typename I> const auto execute_ecall = RISCVALU<I>::syscall;
template<typename I> const auto execute_ebreak = RISCVALU<I>::breakpoint;
template<typename I> const auto execute_uret = RISCVALU<I>::template jump_and_link<RISCVALU<I>::riscv_jr>;
template<typename I> const auto execute_sret = RISCVALU<I>::template jump_and_link<RISCVALU<I>::riscv_jr>;
template<typename I> const auto execute_mret = RISCVALU<I>::template jump_and_link<RISCVALU<I>::riscv_jr>;
template<typename I> const auto execute_wfi = do_nothing<I>;
template<typename I> const auto execute_fence = RISCVALU<I>::addr;
template<typename I> const auto execute_csrrw = RISCVALU<I>::csrrw;
template<typename I> const auto execute_csrrs = RISCVALU<I>::csrrs;
template<typename I> const auto execute_csrrc = do_nothing<I>;
template<typename I> const auto execute_csrrwi = RISCVALU<I>::csrrwi;
template<typename I> const auto execute_csrrsi = do_nothing<I>;
template<typename I> const auto execute_csrrci = do_nothing<I>;
// M
template<typename I> const auto execute_mul = RISCVMultALU<I>::template mult_l<typename I::RegisterUInt>;
template<typename I> const auto execute_mulh = RISCVMultALU<I>::template mult_h_ss<typename I::RegisterUInt>;
template<typename I> const auto execute_mulhsu = RISCVMultALU<I>::template mult_h_su<typename I::RegisterUInt>;
template<typename I> const auto execute_mulhu = RISCVMultALU<I>::template mult_h_uu<typename I::RegisterUInt>;
template<typename I> const auto execute_div = RISCVMultALU<I>::template div<sign_t<typename I::RegisterUInt>>;
template<typename I> const auto execute_divu = RISCVMultALU<I>::template div<typename I::RegisterUInt>;
template<typename I> const auto execute_rem = RISCVMultALU<I>::template rem<sign_t<typename I::RegisterUInt>>;
template<typename I> const auto execute_remu = RISCVMultALU<I>::template rem<typename I::RegisterUInt>;
// B
template<typename I> const auto execute_bext = RISCVALU<I>::template sbext<typename I::RegisterUInt>;
template<typename I> const auto execute_bfp = RISCVALU<I>::bit_field_place;
template<typename I> const auto execute_binv = RISCVALU<I>::template sbinv<typename I::RegisterUInt>;
template<typename I> const auto execute_clmul = RISCVALU<I>::template clmul<typename I::RegisterUInt>;
template<typename I> const auto execute_clz = RISCVALU<I>::template clz<typename I::RegisterUInt>;
template<typename I> const auto execute_cpop = RISCVALU<I>::template pcnt<typename I::RegisterUInt>;
template<typename I> const auto execute_ctz = RISCVALU<I>::template ctz<typename I::RegisterUInt>;
template<typename I> const auto execute_gorc = RISCVALU<I>::template gorc<typename I::RegisterUInt>;
template<typename I> const auto execute_gorci = RISCVALU<I>::gorci;
template<typename I> const auto execute_grev = RISCVALU<I>::grev;
template<typename I> const auto execute_max = RISCVALU<I>::max;
template<typename I> const auto execute_maxu = RISCVALU<I>::maxu;
template<typename I> const auto execute_min = RISCVALU<I>::min;
template<typename I> const auto execute_minu = RISCVALU<I>::minu;
template<typename I> const auto execute_orn = RISCVALU<I>::orn;
template<typename I> const auto execute_pack = RISCVALU<I>::template pack<typename I::RegisterUInt>;
template<typename I> const auto execute_packu = RISCVALU<I>::template packu<typename I::RegisterUInt>;
template<typename I> const auto execute_rol = RISCVALU<I>::rol;
template<typename I> const auto execute_ror = RISCVALU<I>::ror;
template<typename I> const auto execute_rori = RISCVALU<I>::rori;
template<typename I> const auto execute_shfl = RISCVALU<I>::riscv_shfl;
template<typename I> const auto execute_slo = RISCVALU<I>::template slo<typename I::RegisterUInt>;
template<typename I> const auto execute_sloi = RISCVALU<I>::template sloi<typename I::RegisterUInt>;
template<typename I> const auto execute_sro = RISCVALU<I>::template sro<typename I::RegisterUInt>;
template<typename I> const auto execute_sroi = RISCVALU<I>::sroi;
template<typename I> const auto execute_unshfl = RISCVALU<I>::riscv_unshfl;
template<typename I> const auto execute_xnor = RISCVALU<I>::xnor;


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
    using Execute = typename RISCVALU<I>::Execute;

    char subset = 'I';
    RISCVAutogeneratedTableEntry entry = instr_invalid;
    Execute function = do_nothing<I>;
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
    {'B', instr_binv,       execute_binv<I>,   OUT_ARITHM, ' ', Imm::NO, { Src::RS1, Src::RS2 },  { Dst::RD }, 0, 32 | 64      },
    {'B', instr_bext,       execute_bext<I>,   OUT_ARITHM, ' ', Imm::NO, { Src::RS1, Src::RS2 },  { Dst::RD }, 0, 32 | 64      },
    {'B', instr_pack,       execute_pack<I>,   OUT_ARITHM, ' ', Imm::NO, { Src::RS1, Src::RS2 },  { Dst::RD }, 0, 32 | 64      },
    {'B', instr_packu,      execute_packu<I>,  OUT_ARITHM, ' ', Imm::NO, { Src::RS1, Src::RS2 },  { Dst::RD }, 0, 32 | 64      },
    {'B', instr_xnor,       execute_xnor<I>,   OUT_ARITHM, ' ', Imm::NO, { Src::RS1, Src::RS2 },  { Dst::RD }, 0, 32 | 64      },
    {'B', instr_bfp,        execute_bfp<I>,    OUT_ARITHM, ' ', Imm::NO, { Src::RS1, Src::RS2 },  { Dst::RD }, 0, 32 | 64      },
    {'B', instr_grev,       execute_grev<I>,   OUT_ARITHM, ' ', Imm::NO, { Src::RS1, Src::RS2 },  { Dst::RD }, 0, 32 | 64      },
    {'B', instr_cpop,       execute_cpop<I>,   OUT_ARITHM, ' ', Imm::NO, { Src::RS1, Src::RS2 },  { Dst::RD }, 0, 32 | 64      },
    {'B', instr_clz,        execute_clz<I>,    OUT_ARITHM, ' ', Imm::NO, { Src::RS1, Src::ZERO }, { Dst::RD }, 0, 32 | 64      },
    {'B', instr_ctz,        execute_ctz<I>,    OUT_ARITHM, ' ', Imm::NO, { Src::RS1, Src::ZERO }, { Dst::RD }, 0, 32 | 64      },
    {'B', instr_rol,        execute_rol<I>,    OUT_ARITHM, ' ', Imm::NO, { Src::RS1, Src::RS2 },  { Dst::RD }, 0, 32 | 64      },
    {'B', instr_ror,        execute_ror<I>,    OUT_ARITHM, ' ', Imm::NO, { Src::RS1, Src::RS2 },  { Dst::RD }, 0, 32 | 64      },
    {'B', instr_clmul,      execute_clmul<I>,  OUT_ARITHM, ' ', Imm::NO, { Src::RS1, Src::RS2 },  { Dst::RD }, 0, 32 | 64      },
    {'B', instr_max,        execute_max<I>,    OUT_ARITHM, ' ', Imm::NO, { Src::RS1, Src::RS2 },  { Dst::RD }, 0, 32 | 64      },
    {'B', instr_maxu,       execute_maxu<I>,   OUT_ARITHM, ' ', Imm::NO, { Src::RS1, Src::RS2 },  { Dst::RD }, 0, 32 | 64      },
    {'B', instr_min,        execute_min<I>,    OUT_ARITHM, ' ', Imm::NO, { Src::RS1, Src::RS2 },  { Dst::RD }, 0, 32 | 64      },
    {'B', instr_minu,       execute_minu<I>,   OUT_ARITHM, ' ', Imm::NO, { Src::RS1, Src::RS2 },  { Dst::RD }, 0, 32 | 64      },
    {'B', instr_gorc,       execute_gorc<I>,   OUT_ARITHM, ' ', Imm::NO, { Src::RS1, Src::RS2 },  { Dst::RD }, 0, 32 | 64      },
    {'B', instr_gorci,      execute_gorci<I>,  OUT_ARITHM, '7', Imm::ARITH, { Src::RS1, Src::ZERO }, { Dst::RD }, 0, 32 | 64   },
    {'B', instr_unshfl,     execute_unshfl<I>, OUT_ARITHM, ' ', Imm::NO, { Src::RS1, Src::RS2 },  { Dst::RD }, 0, 32 | 64 | 128}, // NOLINT(hicpp-signed-bitwise) https://bugs.llvm.org/show_bug.cgi?id=44977
    {'B', instr_rori,       execute_rori<I>,   OUT_ARITHM, '7', Imm::ARITH, { Src::RS1, Src::ZERO },  { Dst::RD }, 0, 32 | 64      },
    {'B', instr_shfl,       execute_shfl<I>,   OUT_ARITHM, ' ', Imm::NO, { Src::RS1, Src::RS2 },  { Dst::RD }, 0, 32 | 64      },
    {'B', instr_sloi,       execute_sloi<I>,   OUT_ARITHM, '7', Imm::ARITH, { Src::RS1, Src::ZERO }, { Dst::RD }, 0, 32 | 64   },
    {'B', instr_sroi,       execute_sroi<I>,   OUT_ARITHM, '7', Imm::ARITH, { Src::RS1, Src::ZERO }, { Dst::RD }, 0, 32 | 64   },
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
