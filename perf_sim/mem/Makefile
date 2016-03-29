#
# Makefile
# Building a model of a cache tag array with performance tests.
# MIPT-MIPS Assignment 5.
# Ladin Oleg.
#

# C++ compiler flags.
CXXFLAGS= -std=c++0x -Dnullptr=0

# Specifying relative path to the trunk.
TRUNK= ../..

# Paths to look for files.
vpath %.h $(TRUNK)/common/

# Options for compiler specifuing paths to look for headers.
INCL= -I ./ -I $(TRUNK)/common/

#
# Enter for build "miss_rate_sim" program.
#
miss_rate_sim: cache_tag_array.o miss_rate_sim.o
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
	@-rm miss_rate_sim
