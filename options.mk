# C++ compile
CXX = g++
CXXFLAGS= -std=c++14 -Wall -Wextra -Werror -Wpedantic -Wold-style-cast -Weffc++
ifeq ($(DEBUG), 1)
	CXXFLAGS+= -O0 -g
else
	CXXFLAGS+= -O3
	LDFLAGS = -flto
endif

# Paths to look for headers
vpath %.h $(TRUNK)/common
vpath %.h $(TRUNK)/func_sim/
vpath %.h $(TRUNK)/func_sim/elf_parser/
vpath %.h $(TRUNK)/func_sim/func_instr/
vpath %.h $(TRUNK)/func_sim/func_memory/
vpath %.h $(TRUNK)/perf_sim/mem/
vpath %.h $(TRUNK)/perf_sim/bpu/
vpath %.cpp $(TRUNK)/common
vpath %.cpp $(TRUNK)/common/ports
vpath %.cpp $(TRUNK)/func_sim/
vpath %.cpp $(TRUNK)/func_sim/elf_parser/
vpath %.cpp $(TRUNK)/func_sim/func_instr/
vpath %.cpp $(TRUNK)/func_sim/func_memory/
vpath %.cpp $(TRUNK)/perf_sim/mem/
vpath %.cpp $(TRUNK)/perf_sim/bpu/


# GoogleTest directories
GTEST_DIR= $(TRUNK)/libs/googletest
GTEST_INCL= -I $(GTEST_DIR)/lib/include
GTEST_LIB= $(GTEST_DIR)/lib/libgtest.a

# option for C++ compiler specifying directories
# to search for headers
INCL= -I $(TRUNK)/
BOOST_INCL= -I /usr/local/include/boost/
