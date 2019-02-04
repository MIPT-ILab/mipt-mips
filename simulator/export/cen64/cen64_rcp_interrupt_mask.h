/*
 * cen64_rcp_interrupt_mask.h
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#ifndef CEN64_RCP_INTERRUPT_MASK
#define CEN64_RCP_INTERRUPT_MASK
 
#include <infra/types.h>

enum rcp_interrupt_mask : uint64 {
  MI_INTR_SP = 0x01,
  MI_INTR_SI = 0x02,
  MI_INTR_AI = 0x04,
  MI_INTR_VI = 0x08,
  MI_INTR_PI = 0x10,
  MI_INTR_DP = 0x20
};

#endif
