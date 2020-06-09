/*
 * checker.h - wrapper around functional simulator
 * to check state of performance simulator
 * Copyright 2015-2019 MIPT-MIPS
 */
 
#ifndef CHECKER_H
#define CHECKER_H
 
#include <func_sim/func_sim.h>

struct CheckerMismatch final : Exception
{
    explicit CheckerMismatch(const std::string& msg)
        : Exception("Performance simulator and functional simulator executed different instructions\n"
                    "Usually it indicates a bug in performance simulator\n"
                    "The different instructions are\n"
                    , msg)
    { }
};

template<typename ISA>
class Checker {
    using FuncInstr = typename ISA::FuncInstr;
public:
    void disable() { active = false; }
    void check( const FuncInstr& instr);
    void init( std::endian endian, Kernel* kernel, std::string_view isa);
    void set_target( const Target& value);
    void driver_step( const FuncInstr& instr);
private:
    std::shared_ptr<FuncSim<ISA>> sim;
    bool active = false;
};

#endif // CHECKER_H
