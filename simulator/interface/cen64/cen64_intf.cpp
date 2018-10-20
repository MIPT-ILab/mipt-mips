/*
 * cen64_intf.cpp - CPU interface between MIPT-MIPS and CEN64
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#include "cen64_intf.h" 
#include <memory/memory.h>
#include <simulator.h>

extern std::unique_ptr<FuncMemory> generate_cen64_memory( bus_controller * bus_ptr);

struct vr4300
{
    std::unique_ptr<CycleAccurateSimulator> sim;
    std::unique_ptr<FuncMemory> bus;

    int init( struct bus_controller * bus_ptr)
    {
        sim = CycleAccurateSimulator::create_simulator("mips64", true);
        bus = generate_cen64_memory( bus_ptr);
        sim->set_memory( bus.get());
        sim->set_pc(0x1fc00000ull);
        return 0;
    }
};

// NOLINTNEXTLINE(cppcoreguidelines-owning-memory) CEN64 owns memory itself, and we fully trust it
struct vr4300* vr4300_alloc() { return new vr4300; }

// NOLINTNEXTLINE(cppcoreguidelines-owning-memory) CEN64 owns memory itself, and we fully trust it
void vr4300_free(struct vr4300* ptr) { delete ptr; }

struct vr4300_stats* vr4300_stats_alloc() { return nullptr; }
void vr4300_stats_free(struct vr4300_stats* /* ptr */) {}

int vr4300_init(struct vr4300 *vr4300, struct bus_controller * bus) { return vr4300->init( bus); }
void vr4300_cp1_init(struct vr4300* /* vr4300 */ ) {}

void vr4300_cycle(struct vr4300 * vr4300) { vr4300->sim->clock(); }
void vr4300_cycle_extra(struct vr4300* /* vr4300 */, struct vr4300_stats* /* stats */) { }

uint64_t vr4300_get_register(struct vr4300* /* vr4300 */, size_t /* index */) { return 0; }
uint64_t vr4300_get_pc(struct vr4300* /* vr4300 */) { return 0; }

int read_mi_regs(void * /* opaque */, uint32_t /* address */, uint32_t * /* word */) { return 0; }
int write_mi_regs(void * /* opaque */, uint32_t /* address */, uint32_t /* word */, uint32_t /* dqm */) { return 0; }

void clear_rcp_interrupt(struct vr4300* /* vr4300 */, enum rcp_interrupt_mask /* mask */) {}
void signal_rcp_interrupt(struct vr4300* /* vr4300 */, enum rcp_interrupt_mask /* mask */) {}

void clear_dd_interrupt(struct vr4300* /* vr4300 */) {}
void signal_dd_interrupt(struct vr4300* /* vr4300 */) {}
