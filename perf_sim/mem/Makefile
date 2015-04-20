# 
# Building the single-cycle MIPS simulator
# @author Pavel Kryukov <pavel.kryukov@phystech.com>
# Copyright 2015 MIPT-MIPS iLab Project
#

# specifying relative path to the TRUNK
TRUNK= ../../

# paths to look for headers
vpath %.h $(TRUNK)/common
vpath %.h $(TRUNK)/perf_sim/mem


# option for C++ compiler specifying directories 
# to search for headers
INCL= -I ./ -I $(TRUNK)/common/

#options for static linking of boost Unit Test library
INCL_GTEST= -I $(TRUNK)/libs/gtest-1.6.0/include
GTEST_LIB= $(TRUNK)/libs/gtest-1.6.0/libgtest.a

#
# Enter for building func_memory stand alone program
#
miss_rate_sim: miss_rate_sim.o cache_tag_array.o Cache_Prop_Table.o
	@# don't forget to link ELF library using "-l elf"
	$(CXX) -o $@ $^ -l elf
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

miss_rate_sim.o: miss_rate_sim.cpp cache_tag_array.h Cache_Prop_Table.h
	$(CXX) -c $< $(INCL)

cache_tag_array.o: cache_tag_array.cpp cache_tag_array.h types.h
	$(CXX) -c $< $(INCL)

Cache_Prop_Table.o: Cache_Prop_Table.h types.h
	$(CXX) -c $< $(INCL)


clean:
	@-rm *.o
	@-rm perf_sim
