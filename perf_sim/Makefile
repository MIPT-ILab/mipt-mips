#
# Makefile
# Building the scalar MIPS CPU simulator.
# MIPT-MIPS Assignment 4.
# Ladin Oleg.
#

# C++ compiler flags
CXXFLAGS= -std=c++0x -Dnullptr=0

# Specifying relative path to the trunk.
TRUNK= ..

# Paths to look for files.
vpath %.h $(TRUNK)/common/
vpath %.h $(TRUNK)/perf_sim/
vpath %.h $(TRUNK)/func_sim/elf_parser/
vpath %.h $(TRUNK)/func_sim/func_instr/
vpath %.h $(TRUNK)/func_sim/func_memory/
vpath %.cpp $(TRUNK)/perf_sim/
vpath %.cpp $(TRUNK)/func_sim/elf_parser/
vpath %.cpp $(TRUNK)/func_sim/func_instr/
vpath %.cpp $(TRUNK)/func_sim/func_memory/

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
