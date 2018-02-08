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

#
# Enter for build "perf_sim" program
CPPS= \
    infra/macro_test.cpp \
    infra/elf_parser/elf_parser.cpp \
    infra/memory/memory.cpp \
    infra/config/config.cpp \
    infra/ports/ports.cpp \
    infra/cache/cache_tag_array.cpp \
    mips/mips_instr.cpp \
    mips/mips_memory.cpp \
    func_sim/func_sim.cpp \
    core/perf_sim.cpp \
    simulator.cpp


OBJS= $(addprefix $(OBJ_DIR)/, $(notdir $(CPPS:%.cpp=%.o)))
DEPS= $(OBJS:.o=.d)
LIBS= elf boost_program_options$(BOOST_POSTFIX)

vpath %.cpp $(dir $(CPPS))

mipt-mips: $(OBJS) $(OBJ_DIR)/main.o
	@$(CXX) $(LDFLAGS) $(LPATH) -o $@ $^ $(addprefix -l,$(LIBS))
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

tidy-check: $(CPPS) main.cpp
	@$(TIDY) $^ $(TIDYFLAGS) -- -std=c++17 $(INCL)

tidy: $(CPPS) main.cpp
	@$(TIDY) $^ $(TIDYFLAGS) -fix -- -std=c++17 $(INCL)

###### Rules to get GTest #######

GTEST_DIR:=../googletest/googletest
GTEST_LIB:=libgtest.a
GTEST_INCL:=$(GTEST_DIR)/include
GTEST_OBJ:=$(OBJ_DIR)/gtest-all.o

$(GTEST_OBJ): $(GTEST_DIR)/src/gtest-all.cc
	@echo "[$(CXX)] $@"
	@$(CXX) $(CXXFLAGS) -I $(GTEST_INCL) -I $(GTEST_INCL)/.. -c $< -o $@

$(GTEST_LIB): $(GTEST_OBJ)
	@echo "[ar] $@"
	@ar -rv $@ $<

####### TESTS #######

TESTS:= \
    infra/cache \
    infra/elf_parser \
    infra/memory \
    infra/config \
    infra/instrcache \
    infra/ports \
    infra/string \
    mips \
    bpu \
    func_sim \
    core \

TEST_PATH:=$(shell pwd)/../traces/tt.core.out

test: $(addsuffix /t/test.run, $(TESTS))

clean-tests: $(addsuffix /t/test.clean, $(TESTS))

-include $(addsuffix /t/test.d, $(TESTS))

%/t/test.o: %/t/unit_test.cpp 
	@echo "[$(CXX)] $@"
	@$(CXX) $(CXXFLAGS) -o $@ -c $< $(INCL) -isystem $(GTEST_INCL) -DTEST_PATH=\"$(TEST_PATH)\" -DIMAGE_PATH=\"$(IMAGE_PATH)\"
	@$(CXX) $(CXXFLAGS) -o $(patsubst %.o, %.d, $@) -MM -MT $@ $< $(INCL) -isystem $(GTEST_INCL)

# TODO: remove dependencies from ALL objs
%/t/test.out: $(OBJS) %/t/test.o $(GTEST_LIB)
	@echo "[ld] $@"
	@$(CXX) $(LDFLAGS) $(LPATH) -o $@ $^ $(addprefix -l,$(LIBS)) -lpthread

%/t/test.run: %/t/test.out
	@echo "[$<]"
	@(cd $(dir $^) && exec ./test.out)

%/t/test.clean:
	rm -f $(dir $@)test.*

####### COMMON #######

-include $(DEPS)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(OBJ_DIR)
	@echo "[$(CXX)] $@"
	@$(CXX) $(CXXFLAGS) -o $@ -c $< $(INCL)
	@$(CXX) $(CXXFLAGS) -o $(patsubst %.o, %.d, $@) -MM -MT $@ $< $(INCL)

clean: clean-tests
	rm -rf $(OBJ_DIR)
	rm -f mipt-mips disasm $(GTEST_LIB)
