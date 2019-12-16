/*
 * cen64_wrapper.cpp - CEN64-specific CPU interfaces
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2019 MIPT-MIPS
 */

#include "cen64_rcp_interrupt_mask.h"
#include "cen64_wrapper.h"

static const constexpr uint64 MI_REGS_BASE_ADDRESS = 0x04300000;

int vr4300::init( std::shared_ptr<FuncMemory> m)
{
    set_memory( std::move( m));
    set_pc( 0x1fc00000ULL);
    mi_regs[MI_VERSION_REG] = 0x01010101U;
    mi_regs[MI_INIT_MODE_REG] = 0x80U;
    return 0;
}

void vr4300::apply_mask_to_cause( uint64 mask)
{
    uint64 cause = read_cause_register();
    write_cause_register( cause | mask);
}

void vr4300::reset_mask_to_cause( uint64 mask)
{
    uint64 cause = read_cause_register();
    write_cause_register( cause & ~mask);
}

// Checks for interrupts, possibly sets the cause bit.
void vr4300::check_for_interrupts()
{
    if ((mi_regs[MI_INTR_REG] & mi_regs[MI_INTR_MASK_REG]) != 0)
        apply_mask_to_cause( 0x400);
    else
        reset_mask_to_cause( 0x400);
}

void vr4300::clear_rcp_interrupt(uint32 mask)
{
    mi_regs[MI_INTR_REG] &= ~mask;
    check_for_interrupts();
}

void vr4300::signal_rcp_interrupt(uint32 mask)
{
    mi_regs[MI_INTR_REG] |= mask;
    check_for_interrupts();
}

int vr4300::read_mi_regs( uint32_t address, uint32_t* word) const
{
    auto reg = MiRegister{ ( address - MI_REGS_BASE_ADDRESS) / 4};
    *word = mi_regs.at( reg);
    return 0;
}

int vr4300::write_mi_regs( uint32 address, uint32 word, uint32 dqm)
{
    auto reg = MiRegister{ ( address - MI_REGS_BASE_ADDRESS) / 4};

    switch (reg) {
    case MI_INIT_MODE_REG: write_mi_init_mode_reg( word); break;
    case MI_INTR_MASK_REG: write_mi_intr_mask_reg( word); break;
    default: mi_regs.at( reg) = ( mi_regs.at( reg) & ~dqm) | word; break;
    }

    return 0;
}

void vr4300::write_mi_init_mode_reg( uint32 word)
{
    static const constexpr uint64 MI_EBUS_TEST_MODE = 0x0080;
    static const constexpr uint64 MI_INIT_MODE = 0x0100;
    static const constexpr uint64 MI_RDRAM_REG_MODE = 0x0200;
    uint32 result = word & 0x3FFU;

    if ((word & 0x0080U) != 0)
        result &= ~MI_INIT_MODE;
    else if ((word & 0x0100U) != 0)
        result |= MI_INIT_MODE;

    if ((word & 0x0200U) != 0)
        result &= ~MI_EBUS_TEST_MODE;
    else if ((word & 0x0400U) != 0)
        result |= MI_EBUS_TEST_MODE;

    if ((word & 0x0800U) != 0) {
        mi_regs[MI_INTR_REG] &= ~MI_INTR_DP;
        check_for_interrupts();
    }

    if ((word & 0x1000U) != 0)
        result &= ~MI_RDRAM_REG_MODE;
    else if ((word & 0x2000U) != 0)
        result |= MI_RDRAM_REG_MODE;

    mi_regs[MI_INIT_MODE_REG] = result;
}

void vr4300::write_mi_intr_mask_reg( uint32 word)
{
    if ((word & 0x0001U) != 0)
        mi_regs[MI_INTR_MASK_REG] &= ~MI_INTR_SP;
    else if ((word & 0x0002U) != 0)
        mi_regs[MI_INTR_MASK_REG] |= MI_INTR_SP;

    if ((word & 0x0004U) != 0)
        mi_regs[MI_INTR_MASK_REG] &= ~MI_INTR_SI;
    else if ((word & 0x0008U) != 0)
        mi_regs[MI_INTR_MASK_REG] |= MI_INTR_SI;

    if ((word & 0x0010U) != 0)
        mi_regs[MI_INTR_MASK_REG] &= ~MI_INTR_AI;
    else if ((word & 0x0020U) != 0)
        mi_regs[MI_INTR_MASK_REG] |= MI_INTR_AI;

    if ((word & 0x0040U) != 0)
        mi_regs[MI_INTR_MASK_REG] &= ~MI_INTR_VI;
    else if ((word & 0x0080U) != 0)
        mi_regs[MI_INTR_MASK_REG] |= MI_INTR_VI;

    if ((word & 0x0100U) != 0)
        mi_regs[MI_INTR_MASK_REG] &= ~MI_INTR_PI;
    else if ((word & 0x0200U) != 0)
        mi_regs[MI_INTR_MASK_REG] |= MI_INTR_PI;

    if ((word & 0x0400U) != 0)
        mi_regs[MI_INTR_MASK_REG] &= ~MI_INTR_DP;
    else if ((word & 0x0800U) != 0)
        mi_regs[MI_INTR_MASK_REG] |= MI_INTR_DP;

    check_for_interrupts();
}
