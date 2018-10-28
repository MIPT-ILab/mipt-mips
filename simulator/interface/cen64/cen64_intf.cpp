/*
 * cen64_intf.cpp - CPU interface between MIPT-MIPS and CEN64
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#include "cen64_intf.h" 
#include <memory/memory.h>
#include <modules/core/perf_sim.h>
#include <mips/mips.h>

extern std::shared_ptr<FuncMemory> generate_cen64_memory( bus_controller* bus_ptr);

static const constexpr uint64 MI_REGS_BASE_ADDRESS = 0x04300000;

enum MiRegister : uint64 {
    MI_INIT_MODE_REG,
    MI_VERSION_REG,
    MI_INTR_REG,
    MI_INTR_MASK_REG,
    NUM_MI_REGISTERS,
};

struct vr4300 : private PerfSim<MIPS64>
{
    using PerfSim<MIPS64>::clock;
    using PerfSim<MIPS64>::read_cpu_register;

    std::array<int32, NUM_MI_REGISTERS> mi_regs = {};

    template<typename ... Args>
    vr4300( Args ... args) : PerfSim<MIPS64>( std::move( args)...) { }

    int init( struct bus_controller* bus_ptr)
    {
        set_memory( generate_cen64_memory( bus_ptr));
        set_pc( 0x1fc00000ull);
        mi_regs[MI_VERSION_REG] = 0x01010101;
        mi_regs[MI_INIT_MODE_REG] = 0x80;
        return 0;
    }

    void apply_mask_to_cause( uint64 mask)
    {
        uint64 cause = read_cause_register();
        write_cause_register( cause | mask);
    }

    void reset_mask_to_cause( uint64 mask)
    {
        uint64 cause = read_cause_register();
        write_cause_register( cause & ~mask);
    }

    // Checks for interrupts, possibly sets the cause bit.
    void check_for_interrupts()
    {
        if (mi_regs[MI_INTR_REG] & mi_regs[MI_INTR_MASK_REG])
            apply_mask_to_cause( 0x400);
        else
            reset_mask_to_cause( 0x400);
    }
    
    void write_mi_init_mode_reg( uint32 word);
    void write_mi_intr_mask_reg( uint32 word);
};

// NOLINTNEXTLINE(cppcoreguidelines-owning-memory) CEN64 owns memory itself, and we fully trust it
struct vr4300* vr4300_alloc() { return new vr4300( true); }

// NOLINTNEXTLINE(cppcoreguidelines-owning-memory) CEN64 owns memory itself, and we fully trust it
void vr4300_free(struct vr4300* ptr) { delete ptr; }

struct vr4300_stats* vr4300_stats_alloc() { return nullptr; }
void vr4300_stats_free(struct vr4300_stats* /* ptr */) {}

int vr4300_init(struct vr4300 *vr4300, struct bus_controller * bus) { return vr4300->init( bus); }
void vr4300_cp1_init(struct vr4300* /* vr4300 */ ) {}

void vr4300_cycle(struct vr4300 * vr4300) { vr4300->clock(); }
void vr4300_cycle_extra(struct vr4300* /* vr4300 */, struct vr4300_stats* /* stats */) { }

uint64 vr4300_get_register(struct vr4300* vr4300, size_t i) { return vr4300->read_cpu_register( i); }
uint64 vr4300_get_pc(struct vr4300* /* vr4300 */) { return 0; }

int read_mi_regs( void* opaque, uint32_t address, uint32_t* word)
{
    auto reg = static_cast<MiRegister>( ( address - MI_REGS_BASE_ADDRESS) >> 2);
    *word = reinterpret_cast<const vr4300*>( opaque)->mi_regs[reg];
    return 0;
}

int write_mi_regs( void* opaque, uint32 address, uint32 word, uint32 dqm)
{
    auto vr4300 = reinterpret_cast<struct vr4300*>( opaque);
    auto reg = static_cast<MiRegister>( ( address - MI_REGS_BASE_ADDRESS) >> 2);

    switch (reg) {
    case MI_INIT_MODE_REG: vr4300->write_mi_init_mode_reg( word); break;
    case MI_INTR_MASK_REG: vr4300->write_mi_intr_mask_reg( word); break;
    default: vr4300->mi_regs[reg] = (vr4300->mi_regs[reg] & ~dqm) | word; break;
    }

    return 0;
}

void clear_rcp_interrupt(struct vr4300* vr4300, enum rcp_interrupt_mask mask)
{
    vr4300->mi_regs[MI_INTR_REG] &= ~mask;
    vr4300->check_for_interrupts();
}

void signal_rcp_interrupt(struct vr4300* vr4300, enum rcp_interrupt_mask mask)
{
    vr4300->mi_regs[MI_INTR_REG] |= mask;
    vr4300->check_for_interrupts();
}

void clear_dd_interrupt(struct vr4300* vr4300)  { vr4300->reset_mask_to_cause( 0x800); }
void signal_dd_interrupt(struct vr4300* vr4300) { vr4300->apply_mask_to_cause( 0x800); }

void vr4300::write_mi_init_mode_reg( uint32 word)
{
    static const constexpr uint64 MI_EBUS_TEST_MODE = 0x0080;
    static const constexpr uint64 MI_INIT_MODE = 0x0100;
    static const constexpr uint64 MI_RDRAM_REG_MODE = 0x0200;
    uint32 result = word & 0x3FF;

    if (word & 0x0080)
        result &= ~MI_INIT_MODE;
    else if (word & 0x0100)
        result |= MI_INIT_MODE;

    if (word & 0x0200)
        result &= ~MI_EBUS_TEST_MODE;
    else if (word & 0x0400)
        result |= MI_EBUS_TEST_MODE;

    if (word & 0x0800) {
        mi_regs[MI_INTR_REG] &= ~MI_INTR_DP;
        check_for_interrupts();
    }

    if (word & 0x1000)
        result &= ~MI_RDRAM_REG_MODE;
    else if (word & 0x2000)
        result |= MI_RDRAM_REG_MODE;

    mi_regs[MI_INIT_MODE_REG] = result; 
}

void vr4300::write_mi_intr_mask_reg( uint32 word)
{
    if (word & 0x0001)
        mi_regs[MI_INTR_MASK_REG] &= ~MI_INTR_SP;
    else if (word & 0x0002)
        mi_regs[MI_INTR_MASK_REG] |= MI_INTR_SP;

    if (word & 0x0004)
        mi_regs[MI_INTR_MASK_REG] &= ~MI_INTR_SI;
    else if (word & 0x0008)
        mi_regs[MI_INTR_MASK_REG] |= MI_INTR_SI;

    if (word & 0x0010)
        mi_regs[MI_INTR_MASK_REG] &= ~MI_INTR_AI;
    else if (word & 0x0020)
        mi_regs[MI_INTR_MASK_REG] |= MI_INTR_AI;

    if (word & 0x0040)
        mi_regs[MI_INTR_MASK_REG] &= ~MI_INTR_VI;
    else if (word & 0x0080)
        mi_regs[MI_INTR_MASK_REG] |= MI_INTR_VI;

    if (word & 0x0100)
        mi_regs[MI_INTR_MASK_REG] &= ~MI_INTR_PI;
    else if (word & 0x0200)
        mi_regs[MI_INTR_MASK_REG] |= MI_INTR_PI;

    if (word & 0x0400)
        mi_regs[MI_INTR_MASK_REG] &= ~MI_INTR_DP;
    else if (word & 0x0800)
        mi_regs[MI_INTR_MASK_REG] |= MI_INTR_DP;

    check_for_interrupts();
}
