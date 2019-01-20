/*
 * cen64_stub.cpp - memory interface between MIPT-MIPS and CEN64
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#ifndef CEN64_STUB_H
#define CEN64_STUB_H

#include <memory>

struct bus_controller;
std::unique_ptr<bus_controller> get_bus_controller_stub();

#endif
