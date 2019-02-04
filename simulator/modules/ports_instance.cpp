/**
 * ports_instance.cpp - extern speciailization of commonly used ports
 * to speed up build.
 * This file may look like black magic, please ask Pavel Kryukov
 * if you need help here
 * Copyright 2019 MIPT-MIPS
 */

#include "ports_instance.h"

#include <infra/target.h>
#include <mips/mips.h>
#include <modules/core/perf_instr.h>
#include <modules/decode/bypass/data_bypass_interface.h>
#include <modules/fetch/bpu/bp_interface.h>
#include <risc_v/risc_v.h>

#define PORT_TOKEN(x) \
    template class PortQueue<std::pair<x, Cycle>>; \
    template class ReadPort<x>; \
    template class WritePort<x>;
#include "ports_instance.def"
#undef PORT_TOKEN
