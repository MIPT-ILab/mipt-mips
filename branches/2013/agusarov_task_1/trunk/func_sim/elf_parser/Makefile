# 
# Building the elf_parser for MIPS ElF binaries
# @author Alexander Titov <alexander.igorevich.titov@gmail.com>
# Copyright 2012 uArchSim iLab Project
#

# specifying relative path to the TRUNK
TRUNK= ../../

# pathes to loop for headers
vpath %.h $(TRUNK)/common

# option for C++ compiler specifying directories 
# to search for headers
INCL= -I ./ -I $(TRUNK)common/

#options for static linking of boost Unit Test library
INCL_GTEST= -I $(TRUNK)/libs/gtest-1.6.0/include
GTEST_LIB= $(TRUNK)/libs/gtest-1.6.0/libgtest.a

#
# Enter for building elf_parser stand alone program
#
elf_parser: elf_parser.o main.o
	@# don't forget to link ELF library using "-l elf"
	$(CXX) $^ -o $@ -l elf
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

elf_parser.o: elf_parser.cpp elf_parser.h types.h
	$(CXX) -c $< $(INCL)

main.o: main.cpp elf_parser.o
	$(CXX) -c $< $(INCL)

#
# Enter for building elf_parser unit test
#
test: unit_test
	@echo ""
	@echo "Running ./$<\n"
	@./$<
	@echo "Unit testing for the moduler ELF parser passed SUCCESSFULLY!"

unit_test: unit_test.o elf_parser.o
	@# don't forget to link ELF library using "-l elf"
	@# and use "-lpthread" options for Google Test
	$(CXX) $^ -lpthread $(GTEST_LIB) -o $@ -l elf  $(GTEST_LIB)
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

unit_test.o: unit_test.cpp elf_parser.o
	$(CXX) -c $< $(INCL_GTEST) $(INCL) 

clean:
	@-rm *.o
	@-rm elf_parser unit_test
