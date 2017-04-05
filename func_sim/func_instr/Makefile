# 
# Building the disasm for MIPS binaries
# @author Pavel Kryukov <pavel.kryukov@phystech.com>
# Copyright 2014 MIPT-MIPS iLab Project
#

# specifying relative path to the TRUNK
TRUNK= ../..

# Including options and compiler flags
include $(TRUNK)/options.mk

# Enter for building func_memory standalone program
#
disasm: func_memory.o elf_parser.o disasm.o func_instr.o
	@# don't forget to link ELF library using "-l elf"
	$(CXX) $(CXXFLAGS) -o $@ $^ -l elf
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

func_instr.o: func_instr.cpp func_instr.h types.h
	$(CXX) $(CXXFLAGS) -c $< $(INCL)
    
func_memory.o: func_memory.cpp func_memory.h types.h
	$(CXX) $(CXXFLAGS) -c $< $(INCL)

elf_parser.o: elf_parser.cpp elf_parser.h types.h
	$(CXX) $(CXXFLAGS) -c $< $(INCL)

disasm.o: disasm.cpp func_memory.h types.h func_instr.h
	$(CXX) $(CXXFLAGS) -c $< $(INCL)

#
# Enter for building func_memory unit test
#
test: unit_test
	@echo ""
	@echo "Running ./$<\n"
	@./$<
	@echo "Unit testing for the moduler functional memory passed SUCCESSFULLY!"

unit_test: unit_test.o func_memory.o elf_parser.o func_instr.o $(GTEST_LIB)
	@# don't forget to link ELF library using "-l elf"
	@# and use "-lpthread" options for Google Test
	$(CXX) $(GTEST_INCL) -pthread -lpthread $^ -o $@ -l elf
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

unit_test.o: unit_test.cpp func_instr.h $(GTEST_LIB)
	$(CXX) $(CXXFLAGS) $(GTEST_INCL) $(INCL) -c $<

$(GTEST_LIB):
	$(MAKE) -C $(GTEST_DIR)

clean:
	@-rm *.o
	@-rm -rf disasm unit_test
