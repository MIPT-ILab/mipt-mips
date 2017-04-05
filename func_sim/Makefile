# 
# Building the single-cycle MIPS simulator
# @author Pavel Kryukov <pavel.kryukov@phystech.com>
# Copyright 2015 MIPT-MIPS iLab Project
#

# specifying relative path to the TRUNK
TRUNK= ../

# Including options and compiler flags
include $(TRUNK)/options.mk

#
# Enter for building func_memory standalone program
#
func_sim: func_memory.o elf_parser.o func_instr.o func_sim.o main.o
	@# don't forget to link ELF library using "-l elf"
	$(CXX) -o $@ $^ -l elf
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

main.o: main.cpp func_sim.h
	$(CXX) $(CXXFLAGS) -c $< $(INCL)

func_sim.o: func_sim.cpp func_sim.h types.h func_instr.h func_memory.h rf.h
	$(CXX) $(CXXFLAGS) -c $< $(INCL)

func_instr.o: func_instr.cpp func_instr.h types.h
	$(CXX) $(CXXFLAGS) -c $< $(INCL)
    
func_memory.o: func_memory.cpp func_memory.h types.h
	$(CXX) $(CXXFLAGS) -c $< $(INCL)

elf_parser.o: elf_parser.cpp elf_parser.h types.h
	$(CXX) $(CXXFLAGS) -c $< $(INCL)

clean:
	@-rm *.o
	@-rm func_sim
