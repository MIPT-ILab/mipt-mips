# 
# Building the model of cache imp;lementation
# @author Dmitry Ermilov <ermilov@phystech.edu>
# For MIPT MIPS 2015
#

# specifying relative path to the TRUNK
TRUNK= ~T5/

# paths to look for headers
vpath %.h $(TRUNK)/common
vpath %.h $(TRUNK)/perf_sim/
vpath %.cpp $(TRUNK)/perf_sim/

# option for C++ compiler specifying directories 
# to search for headers
INCL= -I ./ -I $(TRUNK)/common/ -I $(TRUNK)/func_sim/elf_parser/ -I $(TRUNK)/func_sim/func_memory/ -I $(TRUNK)/func_sim/func_instr

#options for static linking of boost Unit Test library
INCL_GTEST= -I $(TRUNK)/libs/gtest-1.6.0/include
GTEST_LIB= $(TRUNK)/libs/gtest-1.6.0/libgtest.a

#
# Enter for building func_memory stand alone program
#
miss_rate: func_memory.o elf_parser.o func_instr.o func_sim.o main.o
	$(CXX) -o $@ $^ 
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

main.o: miss_rate_sim.cpp cache_tag_array.h types.h
	$(CXX) -c $< $(INCL)

cache_tag_array.o: cache_tag_array.cpp cache_tag_array.h types.h
	$(CXX) -c $< $(INCL)

clean:
	@-rm *.o
	@-rm miss_rate
