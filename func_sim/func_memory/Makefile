# 
# Building the elf_parser for MIPS ElF binaries
# @author Alexander Titov <alexander.igorevich.titov@gmail.com>
# Copyright 2012 uArchSim iLab Project
#

# specifying relative path to the TRUNK
TRUNK= ../..

# C++ compiler flags
CXXFLAGS= -std=c++11

# paths to look for headers
vpath %.h $(TRUNK)/common
vpath %.h $(TRUNK)/func_sim/elf_parser/
vpath %.cpp $(TRUNK)/func_sim/elf_parser/

# option for C++ compiler specifying directories 
# to search for headers
INCL= -I ./ -I $(TRUNK)/common/ -I $(TRUNK)/func_sim/elf_parser/

# googletest directories
GTEST_DIR= $(TRUNK)/libs/googletest
GTEST_INCL= -I $(GTEST_DIR)/lib/include
GTEST_LIB= $(GTEST_DIR)/lib/libgtest.a

#
# Enter for building func_memory stand alone program
#
func_memory: func_memory.o elf_parser.o main.o
	@# don't forget to link ELF library using "-l elf"
	$(CXX) -o $@ $^ -l elf
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

func_memory.o: func_memory.cpp func_memory.h types.h
	$(CXX) $(CXXFLAGS) -c $< $(INCL)

elf_parser.o: elf_parser.cpp elf_parser.h types.h
	$(CXX) $(CXXFLAGS) -c $< $(INCL)

main.o: main.cpp func_memory.h types.h
	$(CXX) $(CXXFLAGS) -c $< $(INCL)

#
# Enter for building func_memory unit test
#
test: unit_test
	@echo ""
	@echo "Running ./$<\n"
	@./$<
	@echo "Unit testing for the moduler functional memory passed SUCCESSFULLY!"

unit_test: unit_test.o func_memory.o elf_parser.o $(GTEST_LIB)
	@# don't forget to link ELF library using "-l elf"
	@# and use "-lpthread" options for Google Test
	$(CXX) $(GTEST_INCL) -pthread -lpthread $^ -o $@ -l elf
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

unit_test.o: unit_test.cpp func_memory.o elf_parser.o $(GTEST_LIB)
	$(CXX) $(CXXFLAGS) $(GTEST_INCL) $(INCL) -c $<

$(GTEST_LIB):
	$(MAKE) -C $(GTEST_DIR)

clean:
	@-rm *.o
	@-rm -rf func_memory unit_test
