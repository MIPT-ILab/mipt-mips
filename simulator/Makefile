#
# Makefile
# Building the scalar MIPS CPU simulator.
# MIPT-MIPS Assignment 4.
# Ladin Oleg.
#

TRUNK=.

# Including options and compiler flags
include $(TRUNK)/options.mk

#
# Enter for build "perf_sim" program
#
OBJ_DIR:=obj
CPPS= elf_parser.cpp memory.cpp mips_instr.cpp perf_sim.cpp func_sim.cpp config.cpp ports.cpp cache_tag_array.cpp main.cpp
OBJS= $(CPPS:%.cpp=$(OBJ_DIR)/%.o)
DEPS= $(OBJS:.o=.d)


mipt-mips: $(OBJS)
	@# don't forget to link ELF library using "-l elf"
	@$(CXX) $(LDFLAGS) -o $@ $^ -l elf -l boost_program_options -l boost_timer
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

DISASM_CPPS= memory.cpp elf_parser.cpp disasm.cpp func_instr.cpp
DISASM_OBJS= $(DISASM_CPPS:%.cpp=$(OBJ_DIR)/%.o)

disasm: $(DISASM_OBJS)
	@# don't forget to link ELF library using "-l elf"
	$(CXX) $(LDFLAGS) -o $@ $^ -l elf
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

-include $(DEPS)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(OBJ_DIR)
	@echo "[$(CXX)] $<"
	@$(CXX) $(CXXFLAGS) -o $@ -c $< $(INCL)
	@$(CXX) $(CXXFLAGS) -o $(patsubst %.o, %.d, $@) -MM -MT $@ $< $(INCL)

#
# Enter to remove all created files
#
clean:
	rm -rf obj
	rm -f mipt-mips
