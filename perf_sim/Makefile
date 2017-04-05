#
# Makefile
# Building the scalar MIPS CPU simulator.
# MIPT-MIPS Assignment 4.
# Ladin Oleg.
#


# Specifying relative path to the trunk
TRUNK= ..

# Including options and compiler flags
include $(TRUNK)/options.mk

#
# Enter for build "perf_sim" program
#
perf_sim: elf_parser.o func_memory.o func_instr.o perf_sim.o main.o
	$(CXX) $(CXXFLAGS) -o $@ $^ -l elf
	@echo "--------------------------------"
	@echo "$@ is built successfully."
elf_parser.o: elf_parser.cpp
	$(CXX) $(CXXFLAGS) -c $< $(INCL)
func_memory.o: func_memory.cpp
	$(CXX) $(CXXFLAGS) -c $< $(INCL)
func_instr.o: func_instr.cpp
	$(CXX) $(CXXFLAGS) -c $< $(INCL)
#log.o: log.cpp
#	$(CXX) $(CXXFLAGS) -c $< $(INCL)
perf_sim.o: perf_sim.cpp
	$(CXX) $(CXXFLAGS) -c $< $(INCL)
main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $< $(INCL)

#
# Enter to remove all created files
#
clean:
	@-rm *.o
	@-rm perf_sim
