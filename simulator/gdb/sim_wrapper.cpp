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

#define ISA MIPS32
#define ISA_NAME "mips32"

typedef struct {
    /* Use of unique_ptr would lead to messed up conversion, a raw pointer should be enough */
    FuncSim<ISA> *ptr = nullptr;
    std::string filename;
} SimulatorInstance;
static SimulatorInstance simInstance;


namespace config {
    /* Input file may be set by GDB, so this key is not necessary */
    static AliasedValue<std::string> binary_filename = {"b", "binary", "No input file", "input binary file"};
    static AliasedValue<uint64> num_steps = {"n", "numsteps", MAX_VAL64, "number of instructions to run"};
}


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
    simInstance.ptr = static_cast<FuncSim<ISA> *> (Simulator::create_simulator (ISA_NAME, true, true).release ());

    /* Set filename */
    if (filename)
        simInstance.filename = filename;
    else
        simInstance.filename = config::binary_filename;
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
    delete simInstance.ptr;
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
