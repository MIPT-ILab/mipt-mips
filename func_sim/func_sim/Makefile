
#
# Building the single-cycle implementation for MIPS
# @author Aleksandr Shashev <aleksandr.shashev.phystech.edu>
# Copyright 2016 MIPT-MIPS iLab Project
#

# specifying relative path to the TRUNK
TRUNK= ../../

# specifying compiler flags
CXXFLAGS=-std=c++11 -g

# paths to look for headers
vpath %.h $(TRUNK)/common
vpath %.h $(TRUNK)/func_sim/elf_parser/
vpath %.h $(TRUNK)/func_sim/func_instr/
vpath %.h $(TRUNK)/func_sim/func_memory/
vpath %.h $(TRUNK)/func_sim/func_sim/
vpath %.cpp $(TRUNK)/func_sim/elf_parser/
vpath %.cpp $(TRUNK)/func_sim/func_instr/
vpath %.cpp $(TRUNK)/func_sim/func_memory/
vpath %.cpp $(TRUNK)/func_sim/func_sim/

# option for compiler specifying directories to search
# for headers
INCL= -I ./ -I $(TRUNK)/common/ -I $(TRUNK)/func_sim/elf_parser/ -I $(TRUNK)/func_sim/func_memory -I $(TRUNK)/func_sim/func_instr/

# options for static linking of boost Unit Test library
INCL_GTEST= -I $(TRUNK)/libs/gtest-1.6.0/include
GTEST_LIB= $(TRUNK)/libs/gtest-1.6.0/libgtest.a

#
# Enter for building func_sim stad alone program
#
funcsim: func_sim.o func_memory.o elf_parser.o func_instr.o main.o
	@# don't forget to link ELF library using "-l elf"
	$(CXX) $(CXXFLAGS) -o $@ $^ -l elf
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

func_sim.o: func_sim.cpp func_sim.h types.h
	$(CXX) $(CXXFLAGS) -c $< $(INCL)

func_memory.o: func_memory.cpp func_memory.h types.h
	$(CXX) $(CXXFLAGS) -c $< $(INCL)

elf_parser.o: elf_parser.cpp elf_parser.h types.h
	$(CXX) $(CXXFLAGS) -c $< $(INCL)

func_instr.o: func_instr.cpp func_instr.h types.h
	$(CXX) $(CXXFLAGS) -c $< $(INCL)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $< $(INCL)

clean:
	@-rm *.o
	@-rm funcsim
