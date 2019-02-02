/**
 * ports_instance.h - extern speciailization of commonly used ports
 * to speed up build. Include that file instead of port.h.
 * This file may look like black magic, please ask Pavel Kryukov
 * if you need help here
 * Copyright 2019 MIPT-MIPS
 */

#ifndef PORT_INSTANCE_H
#define PORT_INSTANCE_H

#include <infra/ports/ports.h>

struct BPInterface;
class Target;
template<typename T> class BaseMIPSInstr;
template<typename T> class RISCVInstr;
template<typename T> class PerfInstr;
class MIPSRegister;
class RISCVRegister;
template<typename T> class BypassCommand;

#define PORT_TOKEN(x) \
    extern template class ReadPort<x>; \
    extern template class WritePort<x>;
#include "ports_instance.def"
#undef PORT_TOKEN

#endif
