# 
# Building the single-cycle MIPS simulator
# @author Pavel Kryukov <pavel.kryukov@phystech.com>
# Copyright 2015 MIPT-MIPS iLab Project
#

# specifying relative path to the TRUNK
TRUNK= ../../

# paths to look for headers
vpath %.h $(TRUNK)/common
vpath %.h $(TRUNK)/perf_sim/mem/
vpath %.cpp $(TRUNK)/perf_sim/mem
vpath %.h $(TRUNK)/func_sim/
vpath %.h $(TRUNK)/func_sim/elf_parser/
vpath %.h $(TRUNK)/func_sim/func_instr/
vpath %.h $(TRUNK)/func_sim/func_memory/
vpath %.cpp $(TRUNK)/func_sim/
vpath %.cpp $(TRUNK)/func_sim/elf_parser/
vpath %.cpp $(TRUNK)/func_sim/func_instr/
vpath %.cpp $(TRUNK)/func_sim/func_memory/

# option for C++ compiler specifying directories 
# to search for headers
INCL= -I ./ -I $(TRUNK)/common/ -I $(TRUNK)/perf_sim/mem/ 

#options for static linking of boost Unit Test library
INCL_GTEST= -I $(TRUNK)/libs/gtest-1.6.0/include
GTEST_LIB= $(TRUNK)/libs/gtest-1.6.0/libgtest.a

#
# Enter for building func_memory stand alone program
#
miss_rate_sim: cache_tag_array.o miss_rate_sim.o
	@# don't forget to link ELF library using "-l elf"
	$(CXX) -o $@ $^ -l elf
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

miss_rate_sim.o: miss_rate_sim.cpp cache_tag_array.h
	$(CXX) -c $< $(INCL)

cache_tag_array.o: cache_tag_array.cpp types.h cache_tag_array.h 
	$(CXX) -c $< $(INCL)

clean:
	@-rm *.o
	@-rm func_sim
