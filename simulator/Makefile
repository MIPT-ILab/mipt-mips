# Makefile
# Building the scalar MIPS CPU simulator.
# MIPT-MIPS Assignment 4.
# Ladin Oleg.
#

.SUFFIXES:
.PHONY: clean test
.PRECIOUS: %/t/test.o %/t/test.out

# Including options and compiler flags
include ./options.mk

# Clang
CXXFLAGS += -Wno-unused-private-field

#
# Enter for build "perf_sim" program
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


OBJS= $(addprefix $(OBJ_DIR)/, $(notdir $(CPPS:%.cpp=%.o)))
DEPS= $(OBJS:.o=.d)
LIBS= elf boost_program_options boost_timer

vpath %.cpp $(dir $(CPPS))

mipt-mips: $(OBJS) $(OBJ_DIR)/main.o
	@$(CXX) $(LDFLAGS) $(LPATH) -o $@ $^ $(addprefix -l,$(LIBS))
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

tidy: $(CPPS) main.cpp
	@clang-tidy $^ $(TIDYFLAGS) -- -std=c++14 $(INCL)

###### Rules to get GTest #######

GTEST_LIB= $(GTEST_LPATH)/libgtest.a
$(GTEST_LIB):
	@$(MAKE) -C $(GTEST_DIR)

####### TESTS #######

TESTS:= \
    infra/elf_parser \
    infra/memory \
    mips \
    bpu \
    $(shell pwd) \


test: $(addsuffix /t/test.run, $(TESTS))

clean-tests: $(addsuffix /t/test.clean, $(TESTS))

-include $(addsuffix /t/test.d, $(TESTS))

%/t/test.o: %/t/unit_test.cpp
	@echo "[$(CXX)] $@"
	@$(CXX) $(CXXFLAGS) -Wno-effc++ -o $@ -c $< $(INCL) -I $(GTEST_INCL)
	@$(CXX) $(CXXFLAGS) -Wno-effc++ -o $(patsubst %.o, %.d, $@) -MM -MT $@ $< $(INCL) -I $(GTEST_INCL)

# TODO: remove dependencies from ALL objs
%/t/test.out: $(OBJS) %/t/test.o $(GTEST_LIB)
	@echo "[ld] $@"
	@$(CXX) $(LDFLAGS) $(LPATH) -pthread -o $@ $^ $(addprefix -l,$(LIBS)) -l pthread

%/t/test.run: %/t/test.out
	@echo "[$<]"
	@(cd $(dir $^) && exec ./test.out)

%/t/test.clean:
	rm -f $(dir $@)test.*

####### DISASM #######

DISASM_CPPS= \
    infra/memory/memory.cpp \
    infra/elf_parser/elf_parser.cpp \
    func_sim/func_sim.cpp \
    mips/mips_instr.cpp \
    disasm.cpp

disasm: $(addprefix $(OBJ_DIR)/, $(notdir $(DISASM_CPPS:%.cpp=%.o)))
	@$(CXX) $(LDFLAGS) -o $@ $^ -l elf
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

####### COMMON #######

-include $(DEPS)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(OBJ_DIR)
	@echo "[$(CXX)] $@"
	@$(CXX) $(CXXFLAGS) -o $@ -c $< $(INCL)
	@$(CXX) $(CXXFLAGS) -o $(patsubst %.o, %.d, $@) -MM -MT $@ $< $(INCL)

clean: clean-tests
	rm -rf obj
	rm -f mipt-mips disasm

