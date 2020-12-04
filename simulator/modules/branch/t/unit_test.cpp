/**
 * Unit tests for Branch module
 * @author Graudt Vladimir
 * Copyright 2020 MIPT-MIPS
 */

#include <catch.hpp>
#include <modules/branch/branch.h>
#include <modules/t/test_instr.h>

namespace Test {

/* Emulates all modules, which can communicate with Branch */
class BranchTestingEnvironment : public Module {
public:
    using Instr = PerfInstr<BranchTestInstr>;
    using RegisterUInt = typename BranchTestInstr::RegisterUInt;
    using InstructionOutput = std::array< RegisterUInt, MAX_DST_NUM>;

	ReadPort<bool> *rp_flush = nullptr;
	WritePort<bool> *wp_trap = nullptr;

	ReadPort<Target> *rp_flush_target = nullptr;
	ReadPort<BPInterface> *rp_bp_update = nullptr;

	WritePort<Instr> *wp_datapath = nullptr;
	ReadPort<Instr> *rp_datapath = nullptr;

	ReadPort<InstructionOutput> *rp_bypass = nullptr;

	ReadPort<bool> *rp_bypassing_unit_flush_notify = nullptr;

	BranchTestingEnvironment( Module *parent);
};

/* Provides communication between Branch and Environment */
struct BranchTester : public Root {
	Branch<BranchTestInstr> branch;
	BranchTestingEnvironment env;

	BranchTester() : Root( "branch_tester"), branch( this), env( this)
		{ init_portmap(); }
};

BranchTestingEnvironment::BranchTestingEnvironment(Module *parent) :
	Module( parent, "branch_testing_environment")
{
	rp_flush = make_read_port<bool>( "BRANCH_2_ALL_FLUSH", Port::LATENCY);
	wp_trap = make_write_port<bool>( "WRITEBACK_2_ALL_FLUSH", Port::BW);

	rp_flush_target = make_read_port<Target>( "BRANCH_2_FETCH_TARGET", Port::LATENCY);
	rp_bp_update = make_read_port<BPInterface>( "BRANCH_2_FETCH", Port::LATENCY);

	wp_datapath = make_write_port<Instr>( "EXECUTE_2_BRANCH", Port::BW);
	rp_datapath = make_read_port<Instr>( "BRANCH_2_WRITEBACK", Port::LATENCY);

	rp_bypass = make_read_port<InstructionOutput>( "BRANCH_2_EXECUTE_BYPASS", Port::LATENCY);
	rp_bypassing_unit_flush_notify = make_read_port<bool>( "BRANCH_2_BYPASSING_UNIT_FLUSH_NOTIFY", Port::LATENCY);
}

auto create_branch( Addr pc, Addr new_pc, Addr target, bool is_taken)
{
	BranchTestInstr func_instr( pc, new_pc, target);
	BPInterface bpi( pc, is_taken, target, false); // is_hit is not used in tests

	return PerfInstr<BranchTestInstr>( func_instr, bpi);
}


const Addr pc = 100; // PC of a branch
const Addr new_pc = pc + 4;
const uint32 shift = 40; // shift, if branch is taken

const auto cl_arrange = 0_cl;	// setting environment
const auto cl_act     = 1_cl;	// running branch module
const auto cl_assert  = 2_cl;	// checking results

const auto taken_forward_branch      = create_branch(pc, new_pc, pc + shift, true);
const auto taken_backward_branch     = create_branch(pc, new_pc, pc - shift, true);
const auto not_taken_forward_branch  = create_branch(pc, new_pc, pc + shift, false);
const auto not_taken_backward_branch = create_branch(pc, new_pc, pc - shift, false);

using instr_ptr_t = const PerfInstr<BranchTestInstr> *;

TEST_CASE( "branches", "[branch_module_test]")
{
	BranchTester t;
	instr_ptr_t instr = nullptr;

	SECTION( "taken_forward_branch")      { instr = &taken_forward_branch; }
	SECTION( "taken_backward_branch")     { instr = &taken_backward_branch; }
	SECTION( "not_taken_forward_branch")  { instr = &not_taken_forward_branch; }
	SECTION( "not_taken_backward_branch") { instr = &not_taken_backward_branch; }

	t.env.wp_datapath->write( *instr, cl_arrange);

	t.branch.clock( cl_act);

	CHECK( t.env.rp_bp_update->is_ready( cl_assert));
	CHECK( t.env.rp_bypass->is_ready( cl_assert));
}

TEST_CASE( "trap", "[branch_module_test]")
{
	BranchTester t;
	auto instr = taken_forward_branch;

	t.env.wp_datapath->write( instr, cl_arrange);
	t.env.wp_trap->write(true, cl_arrange);

	t.branch.clock( cl_act);

	CHECK( !t.env.rp_bp_update->is_ready( cl_assert));
	CHECK( !t.env.rp_bypass->is_ready( cl_assert));
}

} // end of Test namespace
