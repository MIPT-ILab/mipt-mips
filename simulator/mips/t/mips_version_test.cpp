 /*
 * mips_version.cpp - tests of MIPS versions
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */
 
#include <mips/mips_version.h>

// MIPS_I must support only MIPS I instructions
static_assert(  MIPS_I_Instr.is_supported(MIPSVersion::I));
static_assert( !MIPS_II_Instr.is_supported(MIPSVersion::I));
static_assert( !MIPS_III_Instr.is_supported(MIPSVersion::I));
static_assert( !MIPS_IV_Instr.is_supported(MIPSVersion::I));
static_assert( !MIPS_V_Instr.is_supported(MIPSVersion::I));
static_assert( !MIPS_64_Instr.is_supported(MIPSVersion::I));

// MIPS_II must support only MIPS I and MIPS II instructions
static_assert(  MIPS_I_Instr.is_supported(MIPSVersion::II));
static_assert(  MIPS_II_Instr.is_supported(MIPSVersion::II));
static_assert( !MIPS_III_Instr.is_supported(MIPSVersion::II));
static_assert( !MIPS_IV_Instr.is_supported(MIPSVersion::II));
static_assert( !MIPS_V_Instr.is_supported(MIPSVersion::II));
static_assert( !MIPS_64_Instr.is_supported(MIPSVersion::II));

// etc
static_assert(  MIPS_I_Instr.is_supported(MIPSVersion::III));
static_assert(  MIPS_II_Instr.is_supported(MIPSVersion::III));
static_assert(  MIPS_III_Instr.is_supported(MIPSVersion::III));
static_assert( !MIPS_IV_Instr.is_supported(MIPSVersion::III));
static_assert( !MIPS_V_Instr.is_supported(MIPSVersion::III));
static_assert( !MIPS_64_Instr.is_supported(MIPSVersion::III));

static_assert(  MIPS_I_Instr.is_supported(MIPSVersion::IV));
static_assert(  MIPS_II_Instr.is_supported(MIPSVersion::IV));
static_assert(  MIPS_III_Instr.is_supported(MIPSVersion::IV));
static_assert(  MIPS_IV_Instr.is_supported(MIPSVersion::IV));
static_assert( !MIPS_V_Instr.is_supported(MIPSVersion::IV));
static_assert( !MIPS_64_Instr.is_supported(MIPSVersion::IV));

static_assert(  MIPS_I_Instr.is_supported(MIPSVersion::V));
static_assert(  MIPS_II_Instr.is_supported(MIPSVersion::V));
static_assert(  MIPS_III_Instr.is_supported(MIPSVersion::V));
static_assert(  MIPS_IV_Instr.is_supported(MIPSVersion::V));
static_assert(  MIPS_V_Instr.is_supported(MIPSVersion::V));
static_assert( !MIPS_64_Instr.is_supported(MIPSVersion::V));

static_assert(  MIPS_I_Instr.is_supported(MIPSVersion::v64));
static_assert(  MIPS_II_Instr.is_supported(MIPSVersion::v64));
static_assert(  MIPS_III_Instr.is_supported(MIPSVersion::v64));
static_assert(  MIPS_IV_Instr.is_supported(MIPSVersion::v64));
static_assert(  MIPS_V_Instr.is_supported(MIPSVersion::v64));
static_assert(  MIPS_64_Instr.is_supported(MIPSVersion::v64));
