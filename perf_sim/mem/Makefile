#
# Makefile
# Building the scalar MIPS CPU simulator.
# MIPT-MIPS Assignment 4.
# Ladin Oleg.
#

# C++ compiler flags
CXXFLAGS= -std=c++11 -O3 -Ofast

# Specifying relative path to the trunk.
TRUNK= ../../

# Paths to look for files.
vpath %.h $(TRUNK)/common/
vpath %.h $(TRUNK)/perf_sim/mem/
vpath %.cpp $(TRUNK)/perf_sim/mem/

# Options for compiler specifying paths to look for headers.
INCL= -I ./ -I $(TRUNK)/common/ -I $(TRUNK)/perf_sim/mem/

#
# Enter for build "perf_sim" programm.
#
miss_rate_sim:  cache_tag_array.o miss_rate_sim.o
	$(CXX) $(CXXFLAGS) -o $@ $^ 
	@echo "--------------------------------"
	@echo "$@ is built successfully."
cache_tag_array.o: cache_tag_array.cpp
	$(CXX) $(CXXFLAGS) -c $< $(INCL)
miss_rate_sim.o: miss_rate_sim.cpp
	$(CXX) $(CXXFLAGS) -c $< $(INCL)

#
# Enter to remove all created files.
#
clean:
	@-rm *.o
	@-rm mem

