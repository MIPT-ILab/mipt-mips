# C++ compile
CXX = g++
CXXFLAGS= -std=c++14 -Wall -Wextra -Werror -O3

# Paths to look for headers
vpath %.h $(TRUNK)/common
vpath %.h $(TRUNK)/func_sim/
vpath %.h $(TRUNK)/func_sim/elf_parser/
vpath %.h $(TRUNK)/func_sim/func_instr/
vpath %.h $(TRUNK)/func_sim/func_memory/
vpath %.cpp $(TRUNK)/func_sim/
vpath %.cpp $(TRUNK)/func_sim/elf_parser/
vpath %.cpp $(TRUNK)/func_sim/func_instr/
vpath %.cpp $(TRUNK)/func_sim/func_memory/

# GoogleTest directories
GTEST_DIR= $(TRUNK)/libs/googletest
GTEST_INCL= -I $(GTEST_DIR)/lib/include
GTEST_LIB= $(GTEST_DIR)/lib/libgtest.a

# option for C++ compiler specifying directories 
# to search for headers
INCL= -I $(TRUNK)/ -I /usr/local/include/boost/

