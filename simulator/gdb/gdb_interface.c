/**
 * gdb_interface.c - Functional simulator interfaces for GDB
 * @author Vyacheslav Kompan
 * Copyright 2018 MIPT-MIPS
 */


/* Binary file can be set as GDB argument:
 *   $ gdb /path/to/binary
 *   target sim
 *   load
 * Or as simulator argument:
 *   $ gdb
 *   target sim -b /path/to/binary
 *   load
 */


/* BFD config */
#include <config.h>

/* Simulator Interface */
#include "sim_wrapper.h"

/* GDB Interface */
#include <gdb/remote-sim.h>
#include <gdb/callback.h>
/* GDB Simulator utilities */
#include <sim-config.h>
#include <sim-types.h>
#include <sim-inline.h>
#include <sim-arange.h>
#include <sim-base.h>


SIM_DESC sim_open (SIM_OPEN_KIND kind, struct host_callback_struct *callback,
                   struct bfd *abfd, char *const *argv) {
    SIM_DESC sd = sim_state_alloc (kind, callback);
    mips_sim_create ((const char **) argv, abfd ? abfd->filename : NULL);
    return sd;
}


void sim_close (SIM_DESC sd, int quitting) {
    mips_sim_destroy ();
};


SIM_RC sim_load (SIM_DESC sd, const char *prog, struct bfd *abfd, int from_tty) {
    mips_sim_load ();
    return SIM_RC_OK;
};


SIM_RC sim_create_inferior (SIM_DESC sd, struct bfd *abfd,
                            char *const *argv, char *const *env) {
    mips_sim_prepare ();
    return SIM_RC_OK;
};


int sim_read (SIM_DESC sd, SIM_ADDR mem, unsigned char *buf, int length) {};


int sim_write (SIM_DESC sd, SIM_ADDR mem, const unsigned char *buf, int length) {};


int sim_fetch_register (SIM_DESC sd, int regno, unsigned char *buf, int length) {};


int sim_store_register (SIM_DESC sd, int regno, unsigned char *buf, int length) {};


void sim_info (SIM_DESC sd, int verbose) {};


void sim_resume (SIM_DESC sd, int step, int siggnal) {
    mips_sim_resume (step);
};


int sim_stop (SIM_DESC sd) {};


void sim_stop_reason (SIM_DESC sd, enum sim_stop *reason, int *sigrc) {};


void sim_do_command (SIM_DESC sd, const char *cmd) {};


char **sim_complete_command (SIM_DESC sd, const char *text, const char *word) {};
