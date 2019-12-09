/*
 * cen64_intf.cpp - thin interface between MIPT-MIPS and CEN64
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#include "cen64_intf.h"
#include "cen64_wrapper.h"
#include <memory/cen64/cen64_memory.h>

struct vr4300* vr4300_alloc()
{
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory) CEN64 owns memory itself, and we fully trust it
    return new vr4300();
}

void vr4300_free(struct vr4300* ptr)
{
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory) CEN64 owns memory itself, and we fully trust it
    delete ptr;
}

struct vr4300_stats* vr4300_stats_alloc()
{
    return nullptr;
}

void vr4300_stats_free(struct vr4300_stats* /* ptr */)
{
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory) CEN64 owns memory itself, and we fully trust it
    // delete ptr;
}

int vr4300_init(struct vr4300 *vr4300, struct bus_controller * bus)
{
    // Here we bind our CEN64 interfaces
    return vr4300->init( create_cen64_memory( bus));
}

void vr4300_cp1_init(struct vr4300* vr4300)
{
    (void)vr4300;
}

void vr4300_cycle(struct vr4300 * vr4300)
{
    vr4300->clock();
}

void vr4300_cycle_extra(struct vr4300* vr4300, struct vr4300_stats* /* stats */)
{
    (void)vr4300;
}

uint64 vr4300_get_register(struct vr4300* vr4300, size_t i)
{
    return vr4300->read_cpu_register( narrow_cast<uint8>( i));
}

uint64 vr4300_get_pc(struct vr4300* vr4300)
{
    return vr4300->get_pc();
}

int read_mi_regs( struct vr4300* vr4300, uint32_t address, uint32_t* word)
{
    return vr4300->read_mi_regs( address, word);
}

int write_mi_regs( struct vr4300* vr4300, uint32 address, uint32 word, uint32 dqm)
{
    return vr4300->write_mi_regs( address, word, dqm);
}

void clear_rcp_interrupt(struct vr4300* vr4300, enum rcp_interrupt_mask mask)
{
    vr4300->clear_rcp_interrupt( mask);
}

void signal_rcp_interrupt(struct vr4300* vr4300, enum rcp_interrupt_mask mask)
{
    vr4300->signal_rcp_interrupt( mask);
}

void clear_dd_interrupt(struct vr4300* vr4300)
{
    vr4300->reset_mask_to_cause( 0x800);
}

void signal_dd_interrupt(struct vr4300* vr4300)
{
    vr4300->apply_mask_to_cause( 0x800);
}

uint64_t get_profile_sample(struct vr4300 const* /* vr4300 */, size_t /* i */)
{
    return 0;
}

int has_profile_samples(struct vr4300 const* /* vr4300 */)
{
    return 0;
}
