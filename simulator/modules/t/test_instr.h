/**
 * Simplified instructions for modules testing
 * @author Graudt Vladimir
 * Copyright 2020 MIPT-MIPS
 */

#include <func_sim/operation.h>
#include <infra/types.h>

class BranchTestInstr : public Datapath<uint32> {
public:
    using RegisterUInt = uint32;

    BranchTestInstr( Addr pc, Addr new_pc, Addr branch_target, bool is_taken) :
        Datapath<uint32>(pc, new_pc)
    {
    	target = branch_target;
    	is_taken_branch = is_taken;
    }
};

inline std::ostream&
operator <<( std::ostream& os, const BranchTestInstr& /* unused */) { return os << "BranchTestInstr"; }
