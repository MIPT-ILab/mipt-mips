/**
 * sim_wrapper.cpp - Functional simulator wrappers for GDB
 * @author Vyacheslav Kompan
 * Copyright 2018 MIPT-MIPS
 */

#include "sim_wrapper.h"

/* MIPT-MIPS simulator interfaces */
#include <simulator.h>
#include <infra/config/config.h>
#include <mips/mips.h>
#include <func_sim/func_sim.h>

#define ISA_NAME "mips32"

typedef struct {
    std::unique_ptr<Simulator> ptr = nullptr;
    std::string filename;
} SimulatorInstance;
static SimulatorInstance simInstance;


/* Create simulator */
void mips_sim_create (const char **argv, const char *filename) try {
    /* Get argc
     * Passed arguments start at argv[2], end with NULL */
    int argc = 0;
    while (argv[2 + argc++]);
    /* However, POPL seems to skips first passed argument, so we pass starting at
     * argv[1], which is "--sysroot=" */
    config::handleArgs (argc, &argv[1]);

    /* Using dynamic_cast here would be a huge overhead */
    //TODO: add simulator arguments
    simInstance.ptr = Simulator::create_simulator (ISA_NAME, true, true);
    simInstance.filename = filename;
}
catch (const config::HelpOption &e) {
    std::cout << "Functional simulator for MIPS-based CPU (GDB)"
              << std::endl << std::endl << e.what () << std::endl;
}
catch (const std::exception &e) {
    std::cerr << e.what () << std::endl;
}
catch (...) {
    std::cerr << "Unknown exception\n";
}


void mips_sim_destroy () {
    simInstance.ptr.reset();
}


void mips_sim_load () {
    simInstance.ptr->gdb_load (simInstance.filename);
}


void mips_sim_prepare () {
    simInstance.ptr->gdb_prepare ();
}


void mips_sim_resume (int steps) {
    simInstance.ptr->gdb_resume (steps);
}


int mips_sim_mem_read (unsigned int mem, unsigned char *buf, int length) {
    return simInstance.ptr->gdb_mem_read (mem, buf, length);
}


int mips_sim_mem_write (unsigned int mem, const unsigned char *buf, int length) {
    return simInstance.ptr->gdb_mem_write (mem, buf, length);
}
