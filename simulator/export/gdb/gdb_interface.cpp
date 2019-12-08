/**
 * gdb_interface.cpp - Functional simulator interfaces for GDB
 * @author Vyacheslav Kompan
 * Copyright 2018-2019 MIPT-MIPS
 */

/*
 * This file is designed to a be a very thin adapter between
 * GDB simulation format and MIPT-MIPS format; basically, it does
 * only the type conversions between pointer types, Trap types etc.
 *
 * If you need to add new functionality for GDB interaction,
 * please consider adding it to gdb_wrapper.cpp as much as you can
 */

#include <bfd/config.h>

#include <infra/argv.h>
#include <infra/types.h>

#include <tuple>

#include "gdb_wrapper.h"
#include "sim-main.h"

#include <gdb/remote-sim.h>

#include <sim-base.h>
#include <sim-config.h>
#include <sim-types.h>

/* Trap converter */
using GDBTrap = std::pair<enum sim_stop, int>;

static GDBTrap translate_trap( Trap mipt_trap, int exit_code)
{
    if ( mipt_trap == Trap::HALT)
        return GDBTrap( sim_exited, exit_code);
    auto ret = mipt_trap.to_gdb_format();
    if ( ret == GDB_SIGNAL_0)
        return GDBTrap( sim_polling, GDB_SIGNAL_0);
    return GDBTrap( sim_stopped, ret);
}

/* Holder of simulation instances */
static GDBSimVector simInstances;
static GDBSim& get_sim( SIM_DESC sd)
{
    return simInstances.at( sd->instanceId);
}

/* Target values stub */
extern "C" {
CB_TARGET_DEFS_MAP cb_init_syscall_map[1] = {};
CB_TARGET_DEFS_MAP cb_init_errno_map[1] = {};
CB_TARGET_DEFS_MAP cb_init_open_map[1] = {};
}

/*
 * Here and below are implementations of GDB functions defined in
 * '$gdb_workspace/include/gdb/remote-sim.h'
 */

SIM_DESC sim_open( SIM_OPEN_KIND kind, struct host_callback_struct *callback, struct bfd * /* abfd */, char *const *argv)
{
    // TODO(kryukov): define ISA based on GDB configuration
    auto idx = simInstances.allocate_new( "mips32", argv_cast( argv));
    if ( idx == -1)
        return nullptr;

    auto sd = sim_state_alloc( kind, callback);
    sd->instanceId = idx;
    return sd;
}

void sim_close( SIM_DESC sd, int /* quitting */)
{
    get_sim( sd).shutdown();
    sim_state_free( sd);
}

SIM_RC sim_load( SIM_DESC sd, const char * prog_name, struct bfd * /* abfd */, int /* from_tty */)
{
    return get_sim( sd).load( prog_name) ? SIM_RC_OK : SIM_RC_FAIL;
}

SIM_RC sim_create_inferior( SIM_DESC sd, struct bfd * abfd, char *const *argv, char *const *env)
{
    return get_sim( sd).create_inferior( bfd_get_start_address( abfd), argv, env) ? SIM_RC_OK : SIM_RC_FAIL;
}

int sim_read( SIM_DESC sd, SIM_ADDR mem, unsigned char *buf, int length)
{
    return get_sim( sd).memory_read( byte_cast( buf), mem, static_cast<size_t>( length));
}

int sim_write( SIM_DESC sd, SIM_ADDR mem, const unsigned char *buf, int length)
{
    return get_sim( sd).memory_write( mem, byte_cast( buf), static_cast<size_t>( length));
}

int sim_fetch_register( SIM_DESC sd, int regno, unsigned char *buf, int length)
{
    return get_sim( sd).read_register( regno, byte_cast( buf), length);
}

int sim_store_register( SIM_DESC sd, int regno, unsigned char *buf, int length)
{
    return get_sim( sd).write_register( regno, byte_cast( buf), length);
}

void sim_info( SIM_DESC sd, int verbose)
{
    get_sim( sd).info( verbose);
}

void sim_resume( SIM_DESC sd, int step, int /* siggnal */)
{
    get_sim( sd).resume( step);
}

int sim_stop( SIM_DESC sd)
{
    return get_sim( sd).stop();
}

void sim_stop_reason( SIM_DESC sd, enum sim_stop *reason, int *sigrc)
{
    std::tie(*reason, *sigrc) = translate_trap( get_sim( sd).get_trap(), get_sim( sd).get_exit_code());
}

void sim_do_command (SIM_DESC sd, const char *cmd)
{
    return get_sim( sd).do_command( cmd);
}

char **sim_complete_command (SIM_DESC sd, const char *text, const char *word)
{
    return get_sim( sd).sim_complete_command( text, word);
}
