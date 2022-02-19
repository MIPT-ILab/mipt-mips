/**
 * isa.h - ISA concept
 * @author Pavel Kryukov
 * Copyright 2022 MIPT-V
 */
 
#ifndef ISA_H
#define ISA_H
 
#include <func_sim/operation.h> 

class Driver;
class Simulator;

template<typename T>
concept ISA = Register<typename T::Register>
    && Unsigned<typename T::RegisterUInt>
    && std::is_base_of_v<BaseInstruction<typename T::RegisterUInt, typename T::Register>, typename T::FuncInstr>
    && requires (T) {
        { T::create_instr(0, std::endian::little, 0) } -> std::same_as<typename T::FuncInstr>;
        { T::create_driver(nullptr).get() } -> std::same_as<Driver*>;
    };

#endif
