 /*
 * mips_version.cpp - tests of MIPS versions
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */
 
#include "mips_version.h"

// MIPS_I must support only MIPS I instructions
static_assert( is_supported( MIPS_I_Instr,    MIPS_I));
static_assert( !is_supported( MIPS_II_Instr,  MIPS_I));
static_assert( !is_supported( MIPS_III_Instr, MIPS_I));
static_assert( !is_supported( MIPS_IV_Instr,  MIPS_I));
static_assert( !is_supported( MIPS_V_Instr,   MIPS_I));
static_assert( !is_supported( MIPS_64_Instr,  MIPS_I));

// MIPS_II must support only MIPS I and MIPS II instructions
static_assert( is_supported( MIPS_I_Instr,    MIPS_II));
static_assert( is_supported( MIPS_II_Instr,   MIPS_II));
static_assert( !is_supported( MIPS_III_Instr, MIPS_II));
static_assert( !is_supported( MIPS_IV_Instr,  MIPS_II));
static_assert( !is_supported( MIPS_V_Instr,   MIPS_II));
static_assert( !is_supported( MIPS_64_Instr,  MIPS_II));

// etc
static_assert( is_supported( MIPS_I_Instr,    MIPS_III));
static_assert( is_supported( MIPS_II_Instr,   MIPS_III));
static_assert( is_supported( MIPS_III_Instr,  MIPS_III));
static_assert( !is_supported( MIPS_IV_Instr,  MIPS_III));
static_assert( !is_supported( MIPS_V_Instr,   MIPS_III));
static_assert( !is_supported( MIPS_64_Instr,  MIPS_III));

static_assert( is_supported( MIPS_I_Instr,    MIPS_IV));
static_assert( is_supported( MIPS_II_Instr,   MIPS_IV));
static_assert( is_supported( MIPS_III_Instr,  MIPS_IV));
static_assert( is_supported( MIPS_IV_Instr,   MIPS_IV));
static_assert( !is_supported( MIPS_V_Instr,   MIPS_IV));
static_assert( !is_supported( MIPS_64_Instr,  MIPS_IV));

static_assert( is_supported( MIPS_I_Instr,    MIPS_V));
static_assert( is_supported( MIPS_II_Instr,   MIPS_V));
static_assert( is_supported( MIPS_III_Instr,  MIPS_V));
static_assert( is_supported( MIPS_IV_Instr,   MIPS_V));
static_assert( is_supported( MIPS_V_Instr,    MIPS_V));
static_assert( !is_supported( MIPS_64_Instr,  MIPS_V));

static_assert( is_supported( MIPS_I_Instr,   MIPS_64));
static_assert( is_supported( MIPS_II_Instr,  MIPS_64));
static_assert( is_supported( MIPS_III_Instr, MIPS_64));
static_assert( is_supported( MIPS_IV_Instr,  MIPS_64));
static_assert( is_supported( MIPS_V_Instr,   MIPS_64));
static_assert( is_supported( MIPS_64_Instr,  MIPS_64));
