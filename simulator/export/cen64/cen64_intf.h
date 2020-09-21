//
// vr4300/interface.h: CPU interface.
//
// CEN64: Cycle-Accurate Nintendo 64 Emulator.
// Copyright (c) 2015, Tyler J. Stachecki
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the Tyler J. Stachecki nor the names of its
//       contributors may be used to endorse or promote products derived from
//       this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL TYLER J. STACHECKI BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef VR4300_CPU_H
#define VR4300_CPU_H

#include "cen64_rcp_interrupt_mask.h"
#include <cstddef>

struct vr4300;
struct vr4300_stats;

extern "C" {

struct vr4300* vr4300_alloc();
void vr4300_free(struct vr4300* ptr);

struct vr4300_stats* vr4300_stats_alloc();
void vr4300_stats_free(struct vr4300_stats* ptr);

int vr4300_init(struct vr4300 *vr4300, struct bus_controller *bus);
void vr4300_cp1_init(struct vr4300 *vr4300);

void vr4300_cycle(struct vr4300 *vr4300);
void vr4300_cycle_extra(struct vr4300 *vr4300, struct vr4300_stats *stats);

uint64 vr4300_get_register(struct vr4300 *vr4300, std::size_t i);
uint64 vr4300_get_pc(struct vr4300 *vr4300);

int read_mi_regs(struct vr4300 *vr4300, uint32 address, uint32 *word);
int write_mi_regs(struct vr4300 *vr4300, uint32 address, uint32 word, uint32 dqm);

void clear_rcp_interrupt(struct vr4300 *vr4300, enum rcp_interrupt_mask mask);
void signal_rcp_interrupt(struct vr4300 *vr4300, enum rcp_interrupt_mask mask);

void clear_dd_interrupt(struct vr4300 *vr4300);
void signal_dd_interrupt(struct vr4300 *vr4300);

uint64_t get_profile_sample(struct vr4300 const *vr4300, size_t i);
int has_profile_samples(struct vr4300 const *vr4300);

}

#endif // VR4300_CPU_H
