# 
# Building the elf_parser for MIPS ElF binaries
# @author Alexander Titov <alexander.igorevich.titov@gmail.com>
# Copyright 2012 uArchSim iLab Project
#

.PHONY: all gtest clean

# specifying relative path to the TRUNK
TRUNK= ../..

# Including options and compiler flags
include $(TRUNK)/options.mk

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
	$(CXX) $(GTEST_INCL) -pthread -lpthread $^ -o $@ -l elf
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

unit_test.o: unit_test.cpp elf_parser.o $(GTEST_LIB)
	$(CXX) $(CXXFLAGS) $(GTEST_INCL) $(INCL) -c $< $(GTEST_LIB)

$(GTEST_LIB):
	$(MAKE) -C $(GTEST_DIR)

clean:
	@-rm *.o
	@-rm -rf elf_parser unit_test
