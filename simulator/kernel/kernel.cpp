/*
 * kernel.cpp - base kernel class
 * @author Vyacheslav Kompan kompan.vo@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#include "base_kernel.h"
#include "mars/mars_kernel.h"

#include <infra/config/config.h>
#include <func_sim/operation.h>

namespace config {
    static Switch use_mars = {"mars", "use MARS syscalls"};
} // namespace config

class DummyKernel : public Kernel
{
public:
    Trap execute() final { return Trap( Trap::SYSCALL); }
    void set_simulator( const std::shared_ptr<CPUModel>&) final { };
    void connect_memory( std::shared_ptr<FuncMemory>) final { };
    void add_replica_simulator( const std::shared_ptr<CPUModel>&) final { };
    void add_replica_memory( const std::shared_ptr<FuncMemory>&) final { };
};

std::shared_ptr<Kernel> Kernel::create_dummy_kernel() {
    return std::make_shared<DummyKernel>();
}

std::shared_ptr<Kernel> Kernel::create_mars_kernel() {
    return ::create_mars_kernel();
}

std::shared_ptr<Kernel> Kernel::create_configured_kernel() {
    return config::use_mars ? create_mars_kernel() : Kernel::create_dummy_kernel();
}

Trap Kernel::execute_interactive()
{
    do try {
        return execute();
    }
    catch (const BadInputValue& e) {
        std::cerr << e.what();
    } while (true);

    return Trap( Trap::UNSUPPORTED_SYSCALL);
}

void Kernel::handle_instruction( Operation* instr)
{
    if ( instr->trap_type() != Trap::SYSCALL)
        return;

    auto result = execute_interactive();
    instr->set_trap( result);
}
