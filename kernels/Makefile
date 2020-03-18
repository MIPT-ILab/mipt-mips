# Building MIPS and RISC-V ElF binaries
# @author Alexander Titov <alexander.igorevich.titov@gmail.com>
# Copyright 2012-2019 uArchSim iLab Project

MIPS_AS?=mips-linux-gnu-as
MIPS_LD?=mips-linux-gnu-ld

RISCV_AS?=riscv64-unknown-elf-as
RISCV_LD?=riscv64-unknown-elf-ld

AS_FLAGS?=-march=rv32i -mabi=ilp32
LD_FLAGS?=-melf32lriscv --no-relax

# assemble all the object files 
build_all: mars32_le.bin riscv32.bin

mars32_le.bin: mars32_le.o
	@$(MIPS_LD) $< -o $@ -EL -pie
	@chmod -x $@
	@echo $@ is built

mars32_le.o: exceptions.s Makefile
	@$(MIPS_AS) $< -o $@ -O0 -mips64 -no-break -EL -g -KPIC

riscv32.bin: riscv32.o
	@$(RISCV_LD) $(LD_FLAGS) $< -o $@
	@echo $@ is built

riscv32.o: riscv_exceptions.s Makefile
	@$(RISCV_AS) $(AS_FLAGS) $< -o $@

# it is needed to preven make from 
# deleting .o files automatically
.PRECIOUS: %.o

.PHONY: clean
clean:
	-rm *.o *.out -rf

.PHONY: help
help:
	@echo "  This makefile build all MIPS and RISC-V assembly files in the directory."
	@echo "  To do that just type 'make' or 'make build_all'."
	@echo "  Note that assembly files should have '.s' extension."
