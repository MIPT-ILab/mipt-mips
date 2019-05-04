/**
 * bp_interface.h - perfoming BP updates using ports
 * @author Aleksandr Misevich
 * Copyright 2018 MIPT-MIPS
 */

#ifndef BP_INTERFACE_H_
#define BP_INTERFACE_H_

#include <infra/target.h>
#include <infra/types.h>

/*the structure of data sent from memory to fetch stage */
struct BPInterface {
    Addr pc = NO_VAL32;
    bool is_taken = false;
    Addr target = NO_VAL32;
    bool is_hit = true;

    BPInterface() = default;

    BPInterface( Addr pc, bool is_taken, Addr target, bool is_hit = true)
        : pc( pc)
        , is_taken( is_taken)
        , target( target)
        , is_hit( is_hit)
    { }
};

#endif // BP_INTERFACE_H_


