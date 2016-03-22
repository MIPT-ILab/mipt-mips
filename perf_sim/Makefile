<<<<<<< HEAD
# 
# Building the single-cycle MIPS simulator
# @author Pavel Kryukov <pavel.kryukov@phystech.com>
# Copyright 2015 MIPT-MIPS iLab Project
#

# specifying relative path to the TRUNK
TRUNK= ../

# paths to look for headers
vpath %.h $(TRUNK)/common
vpath %.h $(TRUNK)/perf_sim/
vpath %.h $(TRUNK)/func_sim/
=======
#
# Makefile
# Building the scalar MIPS CPU simulator.
# MIPT-MIPS Assignment 4.
# Ladin Oleg.
#

# C++ compiler flags
CXXFLAGS= -std=c++11

# Specifying relative path to the trunk.
TRUNK= ..

# Paths to look for files.
vpath %.h $(TRUNK)/common/
vpath %.h $(TRUNK)/perf_sim/
>>>>>>> 7aaf4fb0e927dee4480b2b519fb69df42435a456
vpath %.h $(TRUNK)/func_sim/elf_parser/
vpath %.h $(TRUNK)/func_sim/func_instr/
vpath %.h $(TRUNK)/func_sim/func_memory/
vpath %.cpp $(TRUNK)/perf_sim/
<<<<<<< HEAD
vpath %.cpp $(TRUNK)/func_sim/
=======
>>>>>>> 7aaf4fb0e927dee4480b2b519fb69df42435a456
vpath %.cpp $(TRUNK)/func_sim/elf_parser/
vpath %.cpp $(TRUNK)/func_sim/func_instr/
vpath %.cpp $(TRUNK)/func_sim/func_memory/

<<<<<<< HEAD
# option for C++ compiler specifying directories 
# to search for headers
INCL= -I ./ -I $(TRUNK)/common/ -I $(TRUNK)/func_sim/elf_parser/ -I $(TRUNK)/func_sim/func_memory/ -I $(TRUNK)/func_sim/func_instr -I $(TRUNK)/func_sim/ -I $(TRUNK)/perf_sim/

#options for static linking of boost Unit Test library
INCL_GTEST= -I $(TRUNK)/libs/gtest-1.6.0/include
GTEST_LIB= $(TRUNK)/libs/gtest-1.6.0/libgtest.a

#
# Enter for building func_memory stand alone program
#
perf_sim: func_memory.o elf_parser.o func_instr.o perf_sim.o main.o log.o
	@# don't forget to link ELF library using "-l elf"
	$(CXX) -o $@ $^ -l elf
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

main.o: main.cpp perf_sim.h
	$(CXX) -c $< $(INCL)

perf_sim.o: perf_sim.cpp perf_sim.h types.h func_instr.h func_memory.h rf.h ports.h log.h
	$(CXX) -c $< $(INCL)

func_instr.o: func_instr.cpp func_instr.h types.h
	$(CXX) -c $< $(INCL)
    
func_memory.o: func_memory.cpp func_memory.h types.h
	$(CXX) -c $< $(INCL)

elf_parser.o: elf_parser.cpp elf_parser.h types.h
	$(CXX) -c $< $(INCL)

log.o: log.cpp log.h
	$(CXX) -c $< $(INCL)
clean:
	@-rm *.o
	@-rm func_sim
=======
# Options for compiler specifying paths to look for headers.
INCL= -I ./ -I $(TRUNK)/common/ -I $(TRUNK)/func_sim/elf_parser/ \
  -I $(TRUNK)/func_sim/func_memory/  -I $(TRUNK)/func_sim/func_instr/

#
# Enter for build "perf_sim" programm.
#
perf_sim: elf_parser.o func_memory.o func_instr.o log.o perf_sim.o main.o
	$(CXX) $(CXXFLAGS) -o $@ $^ -l elf
	@echo "--------------------------------"
	@echo "$@ is built successfully."
elf_parser.o: elf_parser.cpp
	$(CXX) $(CXXFLAGS) -c $< $(INCL)
func_memory.o: func_memory.cpp
	$(CXX) $(CXXFLAGS) -c $< $(INCL)
func_instr.o: func_instr.cpp
	$(CXX) $(CXXFLAGS) -c $< $(INCL)
log.o: log.cpp
	$(CXX) $(CXXFLAGS) -c $< $(INCL)
perf_sim.o: perf_sim.cpp
	$(CXX) $(CXXFLAGS) -c $< $(INCL)
main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $< $(INCL)

#
# Enter to remove all created files.
#
clean:
	@-rm *.o
	@-rm perf_sim
>>>>>>> 7aaf4fb0e927dee4480b2b519fb69df42435a456
