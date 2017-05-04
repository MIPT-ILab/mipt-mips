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
vpath %.cpp $(TRUNK)/infra
vpath %.cpp $(TRUNK)/infra/ports
vpath %.cpp $(TRUNK)/infra/config
vpath %.cpp $(TRUNK)/infra/memory/
vpath %.cpp $(TRUNK)/infra/cache/
vpath %.cpp $(TRUNK)/infra/elf_parser/
vpath %.cpp $(TRUNK)/mips/
vpath %.cpp $(TRUNK)/func_sim/
vpath %.cpp $(TRUNK)/bpu/

# GoogleTest directories
GTEST_DIR= $(TRUNK)/../libs/googletest
GTEST_INCL= -I $(GTEST_DIR)/lib/include
GTEST_LIB= $(GTEST_DIR)/lib/libgtest.a

# option for C++ compiler specifying directories
# to search for headers
INCL= -I $(TRUNK)/
BOOST_INCL= -I /usr/local/include/boost/
