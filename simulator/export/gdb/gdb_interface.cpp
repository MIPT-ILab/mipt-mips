/**
 * gdb_interface.c - Functional simulator interfaces for GDB
 * @author Vyacheslav Kompan
 * Copyright 2018 MIPT-MIPS
 */

#include "sim-main.h"

/* GDB Interface */
#include <gdb/remote-sim.h>
#include <gdb/callback.h>
/* GDB Simulator utilities */
#include <sim-config.h>
#include <sim-types.h>
#include <sim-inline.h>
#include <sim-arange.h>
#include <sim-base.h>
/* MIPT-MIPS simulator interfaces */
#include <memory/elf/elf_loader.h>
#include <memory/memory.h>
#include <simulator.h>
#include <infra/config/config.h>
/* Generic C++ */
#include <vector>


struct SimulatorInstance {
    std::shared_ptr<Simulator> cpu = nullptr;
    std::shared_ptr<FuncMemory> memory = nullptr;
    std::string filename = {};
    size_t id = 0;
    Trap trap = Trap::NO_TRAP;
};

static std::vector<SimulatorInstance> simInstances;

static SimulatorInstance create_new_env(size_t id, const std::string& filename) {
    //TODO: add simulator arguments
    SimulatorInstance i;
    i.cpu = Simulator::create_simulator("mips32", true, true);
    i.memory = FuncMemory::create_hierarchied_memory();
    i.filename = filename;
    i.id = id;
    return i;
}

int count_argc (char *const *argv) {
    /* Passed arguments start at argv[2], end with NULL */
    int argc = 0;
    while (argv[2 + argc] != nullptr)
        argc++;
    return argc;
}

/* struct bfd *abfd is a Binary File Descriptor for a target program */
SIM_DESC sim_open (SIM_OPEN_KIND kind, struct host_callback_struct *callback,
                   struct bfd *abfd, char *const *argv) {
    if (!abfd) {
        std::cerr << "Input file not set; please re-run GDB with input file" << std::endl;
        return nullptr;
    }

    int argc = count_argc (argv);
    SIM_DESC sd = sim_state_alloc (kind, callback);

    try {
        config::handleArgs (argc, static_cast<const char* const*> (argv), 2);
        sd->instanceId = simInstances.size();
        simInstances.emplace_back(create_new_env(sd->instanceId, abfd->filename));
    }
    catch (const config::HelpOption &e) {
        std::cout << "Functional simulator for MIPS-based CPU (GDB)"
                  << std::endl << std::endl << e.what () << std::endl;
        sim_state_free (sd);
        return nullptr;
    }
    catch (const std::exception &e) {
        std::cerr << e.what () << std::endl;
        sim_state_free (sd);
        return nullptr;
    }
    catch (...) {
        std::cerr << "Unknown exception\n";
        sim_state_free (sd);
        return nullptr;
    }

    std::cout << "MIPT-MIPS simulator instance created, id " << sd->instanceId << std::endl;

    return sd;
}


void sim_close (SIM_DESC sd, int) {
    simInstances.at (sd->instanceId).cpu.reset();
    simInstances.at (sd->instanceId).memory.reset();
    sim_state_free (sd);
}

SIM_RC sim_load (SIM_DESC sd, const char *, struct bfd *, int) {
    SimulatorInstance &simInst = simInstances.at (sd->instanceId);
    ElfLoader elf(simInst.filename);
    elf.load_to(simInst.memory.get());
    simInst.cpu->set_pc( elf.get_startPC());
    std::cout << "MIPT-MIPS: Binary file " << simInst.filename << " loaded" << std::endl;
    return SIM_RC_OK;
}


SIM_RC sim_create_inferior (SIM_DESC sd, struct bfd *,
                            char *const *, char *const *) {
    SimulatorInstance &simInst = simInstances.at (sd->instanceId);
    simInst.cpu->set_memory( simInst.memory);
    std::cout << "MIPT-MIPS: prepared to run" << std::endl;
    return SIM_RC_OK;
}


int sim_read (SIM_DESC sd, SIM_ADDR mem, unsigned char *buf, int length) {
    return simInstances.at( sd->instanceId).memory->memcpy_guest_to_host( byte_cast( buf), mem, static_cast<size_t> (length));
}


int sim_write (SIM_DESC sd, SIM_ADDR mem, const unsigned char *buf, int length) {
    return simInstances.at( sd->instanceId).memory->memcpy_host_to_guest( mem, byte_cast( buf), static_cast<size_t> (length));
}


int sim_fetch_register (SIM_DESC sd, int regno, unsigned char *buf, int length) {
    (void) sd;
    (void) regno;
    (void) buf;
    (void) length;
    return 0;
}


int sim_store_register (SIM_DESC sd, int regno, unsigned char *buf, int length) {
    (void) sd;
    (void) regno;   
    (void) buf;
    (void) length;
    return 0;
}


void sim_info (SIM_DESC sd, int verbose) {
    (void) sd;
    (void) verbose;
}


void sim_resume (SIM_DESC sd, int step, int) {
    SimulatorInstance &simInst = simInstances.at (sd->instanceId);

    std::cout << "MIPT-MIPS: resuming, steps: " << step << std::endl;
    uint64 instrs_to_run = (step == 0) ? MAX_VAL64 : step;
    try {
        if (instrs_to_run == 1)
            simInst.trap = simInst.cpu->run_single_step ();
        else
            simInst.trap = simInst.cpu->run_until_trap( instrs_to_run);
    }
    catch (const BearingLost &e) {
        simInst.trap = Trap::HALT;
        std::cout << "MIPS-MIPS: execution finished: 10 nops in a row" << std::endl;
    }
    catch (const std::exception &e) {
        std::cerr << e.what () << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown exception\n";
    }
}


int sim_stop (SIM_DESC sd) {
    (void) sd;
    return 0;
}


void sim_stop_reason (SIM_DESC sd, enum sim_stop *reason, int *sigrc) {
    switch (simInstances.at(sd->instanceId).trap) {
    case Trap::HALT:
        *reason = sim_exited;
        return;
    case Trap::BREAKPOINT:
    case Trap::EXPLICIT_TRAP:
        *reason = sim_stopped;
        *sigrc = GDB_SIGNAL_TRAP;
        return;
    case Trap::UNALIGNED_ADDRESS:
        *reason = sim_stopped;
        *sigrc = GDB_SIGNAL_BUS;
        return;
    default:
        *reason = sim_polling;
        std::cerr << "Wrong or unhandled trap" << std::endl;
    }
}


void sim_do_command (SIM_DESC sd, const char *cmd) {
    (void) sd;
    (void) cmd;
}


char **sim_complete_command (SIM_DESC sd, const char *text, const char *word) {
    (void) sd;
    (void) text;
    (void) word;
    return nullptr;
}

/* Target values stub */
extern "C" {
CB_TARGET_DEFS_MAP cb_init_syscall_map[1] = {};
CB_TARGET_DEFS_MAP cb_init_errno_map[1] = {};
CB_TARGET_DEFS_MAP cb_init_open_map[1] = {};
}
