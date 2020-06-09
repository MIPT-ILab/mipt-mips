/*
 * cen64_wrapper.h - CEN64-specific CPU interfaces
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2019 MIPT-MIPS
 */

#ifndef CEN64_WRAPPER
#define CEN64_WRAPPER
 
#include <mips/mips.h>
#include <modules/core/perf_sim.h>

struct vr4300 : private PerfSim<MIPS64>
{
    enum MiRegister : uint64 {
        MI_INIT_MODE_REG,
        MI_VERSION_REG,
        MI_INTR_REG,
        MI_INTR_MASK_REG,
        NUM_MI_REGISTERS,
    };
    std::array<uint32, NUM_MI_REGISTERS> mi_regs = {};
    static const constexpr auto cause_index = MIPSRegister::cause().to_rf_index();

    auto read_cause_register() const { return read_cpu_register( cause_index); }
    void write_cause_register( uint64 value) { return write_cpu_register( cause_index, value); }

public:
    vr4300() : PerfSim<MIPS64>( std::endian::big, "mips64be")
    {
         enable_logging( "cpu");
    }

    int init( std::shared_ptr<FuncMemory> mem);
    void apply_mask_to_cause( uint64 mask);
    void reset_mask_to_cause( uint64 mask);

    // Checks for interrupts, possibly sets the cause bit.
    void check_for_interrupts();

    int read_mi_regs( uint32_t address, uint32_t* word) const;
    int write_mi_regs( uint32 address, uint32 word, uint32 dqm);

    void write_mi_init_mode_reg( uint32 word);
    void write_mi_intr_mask_reg( uint32 word);
    
    void clear_rcp_interrupt(uint32 mask);
    void signal_rcp_interrupt(uint32 mask);

    using PerfSim<MIPS64>::clock;
    using PerfSim<MIPS64>::read_cpu_register;
    using PerfSim<MIPS64>::get_pc;
};

#endif
