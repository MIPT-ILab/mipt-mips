#include <catch.hpp>
#include <modules/branch/branch.h>
#include <risc_v/riscv_instr.h>

template <typename FuncInstr>
class BranchTestingEnvironment : public Root {
public:
    using Instr = PerfInstr<FuncInstr>;
    using RegisterUInt = typename FuncInstr::RegisterUInt;
    using InstructionOutput = std::array< RegisterUInt, MAX_DST_NUM>;

	ReadPort<bool> *rp_flush_all = nullptr;
	WritePort<bool> *wp_trap = nullptr;

	ReadPort<Target> *rp_flush_target = nullptr;
	ReadPort<BPInterface> *rp_bp_update = nullptr;

	WritePort<Instr> *wp_datapath = nullptr;
	ReadPort<Instr> *rp_datapath = nullptr;

	ReadPort<InstructionOutput> *rp_bypass = nullptr;

	ReadPort<bool> *rp_bypassing_unit_flush_notify = nullptr;

	BranchTestingEnvironment();
	void config() { init_portmap(); }
};

template <typename FuncInstr>
BranchTestingEnvironment<FuncInstr>::BranchTestingEnvironment() :
	Root( "branch_testing_environment")
{
	rp_flush_all = make_read_port<bool>( "BRANCH_2_ALL_FLUSH", Port::LATENCY);
	wp_trap = make_write_port<bool>( "WRITEBACK_2_ALL_FLUSH", Port::BW);

	rp_flush_target = make_read_port<Target>( "BRANCH_2_FETCH_TARGET", Port::LATENCY);
	rp_bp_update = make_read_port<BPInterface>( "BRANCH_2_FETCH", Port::LATENCY);

	wp_datapath = make_write_port<Instr>( "EXECUTE_2_BRANCH", Port::BW);
	rp_datapath = make_read_port<Instr>( "BRANCH_2_WRITEBACK", Port::LATENCY);

	rp_bypass = make_read_port<InstructionOutput>( "BRANCH_2_EXECUTE_BYPASS", Port::LATENCY);
	rp_bypassing_unit_flush_notify = make_read_port<bool>( "BRANCH_2_BYPASSING_UNIT_FLUSH_NOTIFY", Port::LATENCY);
}

/* Creates initialized riscv PerfInstr for tests */
template <typename T>
auto riscv_branch( std::string_view name, uint32 immediate, Addr PC, bool is_taken, bool is_hit)
{
	RISCVInstr<T> func_instr( name, immediate, PC);
	BPInterface bpi( PC, is_taken, func_instr.get_decoded_target(), is_hit);
	
	return PerfInstr<RISCVInstr<T>>( func_instr, bpi);
}

template <typename T>
auto riscv_taken_branch( std::string_view name, uint32 immediate, Addr PC)
{
	return riscv_branch<T>( name, immediate, PC, true, false); // is_hit is not used in tests
}

template <typename T>
auto riscv_not_taken_branch( std::string_view name, uint32 immediate, Addr PC)
{
	return riscv_branch<T>( name, immediate, PC, false, false); // is_hit is not used in tests
}


template <typename FuncInstr>
auto get_configured_branch_and_env()
{
	using Env = BranchTestingEnvironment<FuncInstr>;
	using Branch = Branch<FuncInstr>;

	/*  Cannot return Environment and Branch without pointer,
	 * because both copy and move constructor are deleted */
	auto env = std::make_unique<Env>();
	auto branch = std::make_unique<Branch>(env.get());
	env->config();

	return std::make_pair<std::unique_ptr<Branch>, std::unique_ptr<Env>>(std::move(branch), std::move(env));
}

const Addr PC = 100; // PC of a branch
const uint32 imm = 40; // imm, stored in branch instruction

const auto cl_arrange = 0_cl;	// setting environment
const auto cl_act     = 1_cl;	// running branch module
const auto cl_assert  = 2_cl;	// checking results

TEMPLATE_TEST_CASE( "riscv beq", "[branch_module_test]", uint32, uint64, uint128)
{
	auto [branch, env] = get_configured_branch_and_env<RISCVInstr<TestType>>();

	auto instr = riscv_taken_branch<TestType>( "beq", PC, imm);
	env->wp_datapath->write( instr, cl_arrange);

	branch->clock( cl_act);

	CHECK( env->rp_bp_update->is_ready( cl_assert));
	CHECK( env->rp_bypass->is_ready( cl_assert));
}