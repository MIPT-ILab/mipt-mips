/**
 * sim-main.h - GDB simulator state stub
 * @author Vyacheslav Kompan
 * Copyright 2018 MIPT-MIPS
 */

#define WITH_PROFILE 0

/* BFD config */
#include <bfd/config.h>

/* GDB simulator utilities */
#include "sim-basics.h"
#include "sim-base.h"


#define CPU_NUM 1


struct _sim_cpu {
    // Add new here, before base
    sim_cpu_base base;
};


struct sim_state {
    sim_cpu *cpu[CPU_NUM];
    // Add new here, after cpu and before base
    unsigned long instanceId;
    sim_state_base base;
};
