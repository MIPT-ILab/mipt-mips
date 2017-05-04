

# Makefile
# Building the scalar MIPS CPU simulator.
# MIPT-MIPS Assignment 4.
# Ladin Oleg.
#

# Including options and compiler flags
include ./options.mk

# Clang
CXXFLAGS += -Wno-unused-private-field

#
# Enter for build "perf_sim" program
#
OBJ_DIR=obj
CPPS= \
    infra/elf_parser/elf_parser.cpp \
    infra/memory/memory.cpp \
    infra/config/config.cpp \
    infra/ports/ports.cpp \
    infra/cache/cache_tag_array.cpp \
    mips/mips_instr.cpp \
    func_sim/func_sim.cpp \
    perf_sim.cpp \
    main.cpp
OBJS= $(addprefix $(OBJ_DIR)/, $(notdir $(CPPS:%.cpp=%.o)))
DEPS= $(OBJS:.o=.d)
LIBS= elf boost_program_options boost_timer

vpath %.cpp $(dir $(CPPS))

mipt-mips: $(OBJS)
	@$(CXX) $(LDFLAGS) $(LPATH) -o $@ $^ $(addprefix -l,$(LIBS))
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

DISASM_CPPS= \
    infra/memory/memory.cpp \
    infra/elf_parser/elf_parser.cpp \
    func_sim/func_sim.cpp \
    mips/mips_instr.cpp \
    disasm.cpp
DISASM_OBJS= $(DISASM_CPPS:%.cpp=$(OBJ_DIR)/%.o)

disasm: $(DISASM_OBJS)
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
clean:
	rm -rf obj
	rm -f mipt-mips
