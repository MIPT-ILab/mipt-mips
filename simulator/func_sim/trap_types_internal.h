#ifndef TRAP_TYPES_INTERNAL_H
#define TRAP_TYPES_INTERNAL_H

enum GDB_TrapType : uint8
{
    GDB_SIGNAL_0    = 0,
    GDB_SIGNAL_TRAP = 5,
    GDB_SIGNAL_SYS  = 12,
    GDB_SIGNAL_BUS  = 10,
    GDB_SIGNAL_ILL  = 4,
};

#endif // TRAP_TYPES_INTERNAL_H
