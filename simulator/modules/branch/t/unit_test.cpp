/**
 * Unit tests for Branch module
 * @author Graudt Vladimir
 * Copyright 2020 MIPT-MIPS
 */

#include <catch.hpp>
#include <modules/branch/branch.h>
#include <modules/t/test_instr.h>
#include <numeric>

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

    explicit BranchTestingEnvironment( Module *parent);
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

auto create_branch( Addr pc, Addr new_pc, Addr branch_target,
    bool should_be_taken, bool predicted_as_taken) noexcept
{
    BranchTestInstr func_instr( pc, (should_be_taken) ? branch_target : new_pc, branch_target, should_be_taken);
    BPInterface bpi( pc, predicted_as_taken, (predicted_as_taken) ? branch_target : new_pc, true);

    return PerfInstr<BranchTestInstr>( func_instr, bpi);
}


const Addr pc = 100; // PC of a branch
const Addr new_pc = pc + 4;
const Addr target = pc + 400; // target, if branch is taken

const auto cl_arrange = 0_cl;   // setting environment
const auto cl_act     = 1_cl;   // running branch module
const auto cl_assert  = 2_cl;   // checking results

const auto taken_and_predicted_branch    = create_branch(pc, new_pc, target, true, true);
const auto taken_and_npredicted_branch   = create_branch(pc, new_pc, target, true, false);
const auto ntaken_and_predicted_branch   = create_branch(pc, new_pc, target, false, true);
const auto ntaken_and_npredicted_branch  = create_branch(pc, new_pc, target, false, false);

using InstrPtr = const PerfInstr<BranchTestInstr> *;

TEST_CASE( "Branch::is_misprediction()", "[branch_module]")
{
    BranchTester t;
    CHECK( !t.branch.is_misprediction( taken_and_predicted_branch, taken_and_predicted_branch.get_bp_data()));
    CHECK( t.branch.is_misprediction( taken_and_npredicted_branch, taken_and_npredicted_branch.get_bp_data()));
    CHECK( t.branch.is_misprediction( ntaken_and_predicted_branch, ntaken_and_predicted_branch.get_bp_data()));
    CHECK( !t.branch.is_misprediction( ntaken_and_npredicted_branch, ntaken_and_npredicted_branch.get_bp_data()));
}

#define CHECK_PORT_READY( port) CHECK( ( port)->is_ready( cl_assert))
#define CHECK_PORT_NOT_READY( port) CHECK( !( port)->is_ready( cl_assert))
#define CHECK_PORT_READY_AND_TRUE( port) CHECK_PORT_READY( port); CHECK( ( port)->read( cl_assert) == true)
#define CHECK_PORT_NOT_READY_OR_FALSE( port) CHECK( (!(port)->is_ready( cl_assert) || ( port)->read( cl_assert) == false))

TEST_CASE( "correctly predicted branch", "[branch_module]")
{
    BranchTester t;
    InstrPtr instr = &taken_and_predicted_branch;

    SECTION( "taken_and_predicted_branch") { instr = &taken_and_predicted_branch; }
    SECTION( "ntaken_and_npredicted_branch") { instr = &ntaken_and_npredicted_branch; }

    t.env.wp_datapath->write( *instr, cl_arrange);

    t.branch.clock( cl_act);

    CHECK_PORT_READY( t.env.rp_datapath);
    CHECK_PORT_READY( t.env.rp_bp_update);
    CHECK_PORT_READY( t.env.rp_bypass);
    CHECK_PORT_NOT_READY( t.env.rp_flush_target);
    CHECK_PORT_NOT_READY_OR_FALSE( t.env.rp_flush);
    CHECK_PORT_NOT_READY_OR_FALSE( t.env.rp_bypassing_unit_flush_notify);
    CHECK( t.branch.get_mispredictions_num() == 0);
    CHECK( t.branch.get_jumps_num() == 0);
}

TEST_CASE( "mispredicted branch", "[branch_module]")
{
    BranchTester t;
    InstrPtr instr = nullptr;
    Addr expected_target = NO_VAL<Addr>;

    SECTION( "taken_and_npredicted_branch") { instr = &taken_and_npredicted_branch; expected_target = target; }
    SECTION( "ntaken_and_predicted_branch") { instr = &ntaken_and_predicted_branch; expected_target = new_pc; }

    t.env.wp_datapath->write( *instr, cl_arrange);

    t.branch.clock( cl_act);

    CHECK_PORT_READY( t.env.rp_datapath);
    CHECK_PORT_READY( t.env.rp_bp_update);
    CHECK_PORT_READY( t.env.rp_bypass);
    CHECK_PORT_READY( t.env.rp_flush);
    CHECK_PORT_READY( t.env.rp_flush_target);
    CHECK_PORT_READY_AND_TRUE( t.env.rp_bypassing_unit_flush_notify);
    CHECK( t.branch.get_mispredictions_num() == 1);
    CHECK( t.branch.get_jumps_num() == 0);

    auto actual_target = t.env.rp_flush_target->read( cl_assert);
    CHECK( actual_target.valid);
    CHECK( actual_target.address == expected_target);
    CHECK( actual_target.sequence_id == instr->get_sequence_id() + 1);
}

TEST_CASE( "bypass check", "[branch_module]")
{
    BranchTester t;
    auto instr = taken_and_predicted_branch;
    std::array<uint32, MAX_DST_NUM> dsts = {};

    std::iota(dsts.begin(), dsts.end(), 228);
    for ( int i = 0; i < MAX_DST_NUM; ++i)
        instr.set_v_dst(dsts.at(i), i);

    t.env.wp_datapath->write( instr, cl_arrange);

    t.branch.clock( cl_act);

    CHECK_PORT_READY( t.env.rp_bypass);
    auto bypass_data = t.env.rp_bypass->read( cl_assert);
    for ( int i = 0; i < MAX_DST_NUM; ++i) {
        INFO( "i = " << i);
        CHECK( bypass_data.at( i) == dsts.at( i));
    }
}

TEST_CASE( "trap received", "[branch_module]")
{
    BranchTester t;
    InstrPtr instr = nullptr;
   
    SECTION( "taken_and_predicted_branch") { instr = &taken_and_predicted_branch; }
    SECTION( "taken_and_npredicted_branch") { instr = &taken_and_npredicted_branch; }
    SECTION( "ntaken_and_predicted_branch") { instr = &ntaken_and_predicted_branch; }
    SECTION( "ntaken_and_npredicted_branch") { instr = &ntaken_and_npredicted_branch; }

    t.env.wp_datapath->write( *instr, cl_arrange);
    t.env.wp_trap->write(true, cl_arrange);

    t.branch.clock( cl_act);

    CHECK_PORT_NOT_READY( t.env.rp_datapath);
    CHECK_PORT_NOT_READY( t.env.rp_bp_update);
    CHECK_PORT_NOT_READY( t.env.rp_bypass);
}

TEST_CASE( "instruction dump", "[branch_module]")
{
    std::ostringstream oss;
    oss << BranchTestInstr( 0, 0, 0, false);
    CHECK( oss.str() == "BranchTestInstr");
}

} // namespace Test
