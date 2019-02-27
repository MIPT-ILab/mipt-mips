#ifndef TRAP_H
#define TRAP_H

#include <infra/types.h>

class Trap
{
    private:
        enum TrapType : uint8
        {
        /* Macro should be used here */
            NO_TRAP,
            HALT,
            EXPLICIT_TRAP,
            BREAKPOINT,
            SYSCALL,
            UNSUPPORTED_SYSCALL,
            UNALIGNED_ADDRESS,
            UNKNOWN_INSTRUCTION
        };
        
        TrapType value;

    public:
        Trap( uint8 value_) { value = value_; }
        Trap( std::string str_trap);
        
        uint8 get_value() { return value }
        std::string string_dump();
};

#endif //TRAP_H
