/**
 * bp_interface.h - perfoming BP updates using ports
 * @author Aleksandr Misevich
 * Copyright 2018 MIPT-MIPS
 */

#ifndef BP_INTERFACE_H_
#define BP_INTERFACE_H_

#include <infra/types.h>

/*the structure of data sent from memory to fetch stage */
struct BPInterface {
    bool is_taken = false;
    Addr branch_ip = NO_VAL32;
    Addr target = NO_VAL32;
};

#endif // BP_INTERFACE_H_


