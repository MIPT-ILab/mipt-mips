/**
 * sim_wrapper.h - Functional simulator wrappers for GDB
 * @author Vyacheslav Kompan
 * Copyright 2018 MIPT-MIPS
 */


#ifndef SIM_WRAPPER_H
#define SIM_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

void mips_sim_create (const char **argv, const char *filename);
void mips_sim_destroy ();
void mips_sim_load ();
void mips_sim_prepare ();
void mips_sim_resume (int steps);

int mips_sim_mem_read (unsigned int mem, unsigned char *buf, int length);
int mips_sim_mem_write (unsigned int mem, const unsigned char *buf, int length);

#ifdef __cplusplus
}
#endif

#endif // SIM_WRAPPER_H
