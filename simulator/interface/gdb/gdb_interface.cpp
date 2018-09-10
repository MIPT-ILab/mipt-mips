/**
 * gdb_interface.c - Functional simulator interfaces for GDB
 * @author Vyacheslav Kompan
 * Copyright 2018 MIPT-MIPS
 */


/* BFD config */
#include <bfd/config.h>
/* GDB Interface */
#include <gdb/remote-sim.h>
#include <gdb/callback.h>
/* GDB Simulator utilities */
#include <sim-config.h>
#include <sim-types.h>
#include <sim-inline.h>
#include <sim-arange.h>
#include <sim-base.h>
#include "sim-main.h"
/* MIPT-MIPS simulator interfaces */
#include <simulator.h>
#include <infra/config/config.h>
#include <mips/mips.h>
#include <func_sim/func_sim.h>
/* STL */
#include <vector>


struct SimulatorInstance {
    std::unique_ptr<Simulator> ptr = nullptr;
    std::string filename;
    size_t id = 0;

    SimulatorInstance (std::unique_ptr<Simulator> simPtr, const char *binary_filename, size_t id)
            : ptr (std::move (simPtr)), filename (binary_filename), id (id) {}
};

static std::vector<SimulatorInstance> simInstances;


int count_argc (char *const *argv) {
    /* Passed arguments start at argv[2], end with NULL */
    int argc = 0;
    while (argv[2 + argc])
        argc++;
    return argc;
}


/* struct bfd *abfd is a Binary File Descriptor for a target program */
SIM_DESC sim_open (SIM_OPEN_KIND kind, struct host_callback_struct *callback,
                   struct bfd *abfd, char *const *argv) {

    if (!abfd) {
        std::cerr << "Input file not set; please re-run GDB with input file" << std::endl;
        return NULL;
    }

    int argc = count_argc (argv);
    SIM_DESC sd = sim_state_alloc (kind, callback);

    try {
        /* POPL seems to skips first passed argument, so we pass starting at
         * argv[1], which is "--sysroot=" */
        config::handleArgs (argc, const_cast<const char **>(&argv[1]));

        /* Create simulator instance */
        //TODO: add simulator arguments
        sd->instanceId = simInstances.size ();
        simInstances.emplace_back (Simulator::create_simulator ("mips32", true, true),
                                   abfd->filename, sd->instanceId);
    }
    catch (const config::HelpOption &e) {
        std::cout << "Functional simulator for MIPS-based CPU (GDB)"
                  << std::endl << std::endl << e.what () << std::endl;
        sim_state_free (sd);
        return NULL;
    }
    catch (const std::exception &e) {
        std::cerr << e.what () << std::endl;
        sim_state_free (sd);
        return NULL;
    }
    catch (...) {
        std::cerr << "Unknown exception\n";
        sim_state_free (sd);
        return NULL;
    }

    std::cout << "MIPT-MIPS simulator instance created, id " << sd->instanceId << std::endl;

    return sd;
}


void sim_close (SIM_DESC sd, int) {
    simInstances.at (sd->instanceId).ptr.reset ();
    sim_state_free (sd);
}


SIM_RC sim_load (SIM_DESC sd, const char *, struct bfd *, int) {
    SimulatorInstance &simInst = simInstances.at (sd->instanceId);
    simInst.ptr->gdb_load (simInst.filename);
    return SIM_RC_OK;
}


SIM_RC sim_create_inferior (SIM_DESC sd, struct bfd *,
                            char *const *, char *const *) {
    simInstances.at (sd->instanceId).ptr->gdb_prepare ();
    return SIM_RC_OK;
}


int sim_read (SIM_DESC sd, SIM_ADDR mem, unsigned char *buf, int length) {
    SimulatorInstance &simInst = simInstances.at (sd->instanceId);
    return static_cast<int> (simInst.ptr->gdb_mem_read (mem, buf, static_cast<size_t> (length)));
}


int sim_write (SIM_DESC sd, SIM_ADDR mem, const unsigned char *buf, int length) {
    SimulatorInstance &simInst = simInstances.at (sd->instanceId);
    return static_cast<int> (simInst.ptr->gdb_mem_write (mem, buf, static_cast<size_t> (length)));
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


void sim_resume (SIM_DESC sd, int step, int) try {
    simInstances.at (sd->instanceId).ptr->gdb_resume (step);
}
catch (BearingLost) {
    std::cout << "MIPS-MIPS: Execution finished: 10 nops in a row" << std::endl;
}


int sim_stop (SIM_DESC sd) {
    (void) sd;
    return 0;
}


void sim_stop_reason (SIM_DESC sd, enum sim_stop *reason, int *sigrc) {
    (void) sd;
    (void) reason;
    (void) sigrc;
}


void sim_do_command (SIM_DESC sd, const char *cmd) {
    (void) sd;
    (void) cmd;
}


char **sim_complete_command (SIM_DESC sd, const char *text, const char *word) {
    (void) sd;
    (void) text;
    (void) word;
    return NULL;
}
