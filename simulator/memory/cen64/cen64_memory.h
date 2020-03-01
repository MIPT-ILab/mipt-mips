/*
 * cen64_memory.h - memory interface between MIPT-MIPS and CEN64
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#ifndef CEN64_MEMORY_H
#define CEN64_MEMORY_H
 
#include <infra/exception.h>
#include <memory/memory.h>

struct bus_controller;

struct CEN64MemoryUnsupportedInterface : Exception
{
    explicit CEN64MemoryUnsupportedInterface( const std::string& msg)
        : Exception( "CEN64 interface is not supported", msg)
    { }
};

std::shared_ptr<FuncMemory> create_cen64_memory( bus_controller * bus_ptr);

#endif
