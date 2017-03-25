# 
# Building the elf_parser for MIPS ElF binaries
# @author Alexander Titov <alexander.igorevich.titov@gmail.com>
# Copyright 2012 uArchSim iLab Project
#

.PHONY: all gtest clean

# specifying relative path to the TRUNK
TRUNK= ../../

# C++ compiler flags
CXXFLAGS= -std=c++11

# pathes to loop for headers
vpath %.h $(TRUNK)/common

# option for C++ compiler specifying directories 
# to search for headers
INCL= -I ./ -I $(TRUNK)common/

# googletest directories
GTEST_DIR= $(TRUNK)/libs/googletest
GTEST_INCL= -I $(GTEST_DIR)/lib/include
GTEST_LIB= $(GTEST_DIR)/lib/libgtest.a


#
# Enter for building elf_parser stand alone program
#
elf_parser: elf_parser.o main.o
	@# don't forget to link ELF library using "-l elf"
	$(CXX) $(CXXFLAGS) $^ -o $@ -l elf
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

elf_parser.o: elf_parser.cpp elf_parser.h types.h
	$(CXX) $(CXXFLAGS) -c $< $(INCL)

main.o: main.cpp elf_parser.o
	$(CXX) $(CXXFLAGS) -c $< $(INCL)

#
# Enter for building elf_parser unit test
#
test: unit_test
	@echo ""
	@echo "Running ./$<\n"
	@./$<
	@echo "Unit testing for the moduler ELF parser passed SUCCESSFULLY!"

unit_test: unit_test.o elf_parser.o $(GTEST_LIB)
	@# don't forget to link ELF library using "-l elf"
	@# and use "-lpthread" options for Google Test
	$(CXX) $(GTEST_INCL) -pthread -lpthread $^ -o $@ -l elf $(GTEST_LIB)
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

unit_test.o: unit_test.cpp elf_parser.o $(GTEST_LIB)
	$(CXX) $(CXXFLAGS) $(GTEST_INCL) $(INCL) -c $<

$(GTEST_LIB):
	$(MAKE) -C $(GTEST_DIR)

clean:
	@-rm *.o
	@-rm elf_parser unit_test
