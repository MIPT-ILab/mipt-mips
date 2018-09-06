/**
 * sim-main.h - GDB simulator state stub
 * @author Vyacheslav Kompan
 * Copyright 2018 MIPT-MIPS
 */


/* BFD config */
#include "config.h"

/* GDB simulator utilities */
#include "sim-basics.h"
#include "sim-base.h"


#define CPU_NUM 1


struct _sim_cpu {

    sim_cpu_base base;
};


struct sim_state {
    sim_cpu *cpu[CPU_NUM];

    sim_state_base base;
};
