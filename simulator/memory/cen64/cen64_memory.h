/*
 * cen64_memory.h - memory interface between MIPT-MIPS and CEN64
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#ifndef CEN64_MEMORY_H
#define CEN64_MEMORY_H
 
#include "../memory.h"

struct bus_controller;
 
std::shared_ptr<FuncMemory> create_cen64_memory( bus_controller * bus_ptr);

#endif
