/*
 * kernel.cpp - base kernel class
 * @author Vyacheslav Kompan kompan.vo@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#include "base_kernel.h"
#include "mars/mars_kernel.h"

#include <func_sim/operation.h>
#include <infra/config/config.h>
#include <memory/elf/elf_loader.h>

namespace config {
    static Switch use_mars = {"mars", "use MARS syscalls"};
} // namespace config

void BaseKernel::load_file( const std::string& name)
{
    ElfLoader loader( name);
    loader.load_to( mem.get());
    start_pc = loader.get_startPC();
}

class DummyKernel : public BaseKernel
{
public:
    Trap execute() final { return Trap( Trap::SYSCALL); }
};

std::shared_ptr<Kernel> Kernel::create_dummy_kernel() {
    return std::make_shared<DummyKernel>();
}

std::shared_ptr<Kernel> Kernel::create_mars_kernel( std::istream& cin, std::ostream& cout, std::ostream& cerr) {
    return ::create_mars_kernel( cin, cout, cerr);
}

std::shared_ptr<Kernel> Kernel::create_configured_kernel() {
    return config::use_mars
        ? Kernel::create_mars_kernel( std::cin, std::cout, std::cerr)
        : Kernel::create_dummy_kernel();
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
