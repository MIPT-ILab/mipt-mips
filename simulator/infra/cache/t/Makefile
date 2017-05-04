#
# Makefile
# Building a model of a cache tag array with performance tests.
# MIPT-MIPS Assignment 5.
# Ladin Oleg.
#

# Specifying relative path to the trunk.
TRUNK= ../../..

# Including options and compiler flags
include $(TRUNK)/options.mk

# Clang
CXXFLAGS += -Wno-unused-private-field

OBJS= cache_tag_array.o miss_rate_sim.o
DEPS= $(OBJS:.o=.d)

#
# Enter for build "miss_rate_sim" program.
#
miss_rate_sim: $(OBJS)
	$(CXX) -o $@ $^
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"


%.o: %.cpp
	@echo "[$(CXX)] $@"
	@$(CXX) $(CXXFLAGS) -c $< $(INCL)
	@$(CXX) $(CXXFLAGS) -MM -o $(patsubst %.o, %.d, $@) $< $(INCL)

#
# Enter to remove all created files.
#
clean:
	@-rm -f *.o *.d
	@-rm -f miss_rate_sim
