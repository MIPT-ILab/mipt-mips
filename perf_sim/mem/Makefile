#
# Building the cache testing program
# @author Alexander Pronin <alexander.pronin.mipt@gmail.com>
# MIPT-MIPS 2015
#

# specifying relative path to the TRUNK
TRUNK= ../../

# paths to look for headers
vpath %.h $(TRUNK)/common

# option for C++ compiler specifying directories 
# to search for headers
INCL= -I ./ -I $(TRUNK)/common/

#options for static linking of boost Unit Test library
INCL_GTEST= -I $(TRUNK)/libs/gtest-1.6.0/include
GTEST_LIB= $(TRUNK)/libs/gtest-1.6.0/libgtest.a

#
# Enter for building perf_sim stand alone program
#
miss_rate_sim: miss_rate_sim.o cache_tag_array.o
	@# don't forget to link ELF library using "-l elf"
	$(CXX) -o $@ $^ -l elf
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

cache_tag_array.o: cache_tag_array.cpp cache_tag_array.h types.h
	$(CXX) -c $< $(INCL)

miss_rate_sim.o: miss_rate_sim.cpp cache_tag_array.h types.h
	$(CXX) -c $< $(INCL)
    
clean:
	@-rm *.o
