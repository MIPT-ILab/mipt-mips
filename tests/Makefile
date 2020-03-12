# Building RISC-V ElF test binaries
# @author Eric Konks
# Copyright 2020 MIPT-V

RISCV_AS?=riscv64-unknown-elf-as
AS_FLAGS?=-march=rv32i -mabi=ilp32
RISCV_LD?=riscv64-unknown-elf-ld
LD_FLAGS?=-melf32lriscv --no-relax

# assemble all the object files 
build_all: rv32-scall.bin

rv32-scall.bin: rv32-scall.o
	@$(RISCV_LD) $(LD_FLAGS) $< -o $@
	@echo $@ is built

rv32-scall.o: rv32-scall.s
	@$(RISCV_AS) $(AS_FLAGS) $< -o $@

# it is needed to preven make from 
# deleting .o files automatically
.PRECIOUS: %.o

.PHONY: clean
clean:
	-rm *.o *.out -rf
