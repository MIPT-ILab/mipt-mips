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
OBJS= elf_parser.o func_memory.o func_instr.o perf_sim.o config.o main.o
DEPS= $(OBJS:.o=.d)

perf_sim: $(OBJS)
	@# don't forget to link ELF library using "-l elf"
	$(CXX) -o $@ $^ -l elf -lboost_program_options
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

-include $(DEPS)

%.o: %.cpp
	@echo "[$(CXX)] $@"
	@$(CXX) $(CXXFLAGS) -c $< $(INCL)
	@$(CXX) $(CXXFLAGS) -MM -o $(patsubst %.o, %.d, $@) $< $(INCL)


#
# Enter to remove all created files
#
clean:
	@-rm -f *.o *.d
	@-rm -f perf_sim
