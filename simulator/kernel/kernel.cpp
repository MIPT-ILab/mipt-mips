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

#include <iostream>

namespace config {
    static const Switch use_mars = {"mars", "use MARS syscalls"};
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
    explicit DummyKernel( std::ostream& cerr) : BaseKernel( cerr) { }
    Trap execute() final { return Trap( Trap::SYSCALL); }
};

static std::shared_ptr<Kernel> create_dummy_kernel( std::ostream& cerr) {
    return std::make_shared<DummyKernel>( cerr);
}

std::shared_ptr<Kernel> Kernel::create_kernel( bool is_mars, std::istream& cin, std::ostream& cout, std::ostream& cerr) {
    return is_mars ? create_mars_kernel( cin, cout, cerr) : create_dummy_kernel( cerr);
}

std::shared_ptr<Kernel> Kernel::create_configured_kernel()
{
    // NOLINTNEXTLINE(clang-analyzer-cplusplus.NewDeleteLeaks) https://bugs.llvm.org/show_bug.cgi?id=45514
    return Kernel::create_kernel( config::use_mars, std::cin, std::cout, std::cerr);
}

Trap Kernel::execute_interactive()
{
    static const constexpr size_t MAX_ATTEMPTS = 100;
    size_t attempts = 0;

    do try {
        return execute();
    }
    catch (const BadInputValue& e) {
        cerr << e.what();
        ++attempts;
    } while (attempts < MAX_ATTEMPTS);

    throw BadInteraction();
}

void Kernel::handle_instruction( Operation* instr)
{
    if ( instr->trap_type() != Trap::SYSCALL)
        return;

    auto result = execute_interactive();
    instr->set_trap( result);
}
