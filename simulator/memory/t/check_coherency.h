/**
 * Simple test to check whether two memory instances are equal
 * @author Pavel I. Kryukov kryukov@frtk.ru
 * Copyright 2012-2018 MIPT-MIPS iLab project
 */

#ifndef CHECK_COHERENCY_H
#define CHECK_COHERENCY_H
 
#include <infra/types.h>

class FuncMemory;
void check_coherency(FuncMemory* mem1, FuncMemory* mem2, Addr address);

#endif // CHECK_COHERENCY_H
