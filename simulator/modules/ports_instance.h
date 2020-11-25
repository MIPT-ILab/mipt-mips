/**
 * ports_instance.h - extern speciailization of commonly used ports
 * to speed up build. Include that file instead of port.h.
 * Copyright 2019 MIPT-MIPS
 */

#ifndef PORT_INSTANCE_H
#define PORT_INSTANCE_H

#include <func_sim/operation.h>
#include <infra/ports/module.h>
#include <infra/uint128.h>

/*
 * Let's describe the black magic occuring here. The problem of Port
 * classes is that they have to be instantiated in translation units
 * berelated to each of pipeline modules (so far we have 6). However,
 * since the code is duplicated, as all modules contain ports of the
 * same types (instructions, bp queries, target queries etc) there is
 * no need to build it several time. So, we prevent building the code
 * in the translation units by writing 'extern template class ...;'
 * magic message. We've to write it for each commonly used port token,
 * and to simplify the job, we do predeclarations of tokens here and
 * maintaining a list of tokens inside "ports_instance.def".
 *
 * In CPP file, we explicitly instanciate the same templates using the
 * 'template class ...;' magin message. As a result, ports_instance.cpp
 * is the only translation units which expands these templates and has
 * the templated code compiled.
 *
 * If there is a port with a new query type, it would be compiled as
 * usual, instanting the ports in translation units that request it.
 * If you want to do the same trick, simply add the query name to the
 * 'ports_instance.def' file wrapped with PORT_TOKEN macro file.
 */

struct BPInterface;
class Target;
template<typename T> class BaseMIPSInstr;
template<typename T> class RISCVInstr;
template<typename T> class PerfInstr;
class MIPSRegister;
class RISCVRegister;
template<typename T> class BypassCommand;

#define PORT_TOKEN(x) \
    extern template class PortQueue<std::pair<x, Cycle>>; \
    extern template class ReadPort<x>; \
    extern template class WritePort<x>;
#include "ports_instance.def"
#undef PORT_TOKEN

#endif
