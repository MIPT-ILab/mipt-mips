#
# Building the disasm for MIPS binaries
# @author Pavel Kryukov <pavel.kryukov@phystech.com>
# Copyright 2014 MIPT-MIPS iLab Project
#

# specifying relative path to the TRUNK
TRUNK= ../..

# Including options and compiler flags
include $(TRUNK)/options.mk

CXXFLAGS+=-Wno-effc++

#
# Enter for building func_instr standalone program
#
OBJS= func_memory.o elf_parser.o disasm.o func_instr.o
DEPS= $(OBJS:.o=.d)

disasm: $(OBJS)
	@# don't forget to link ELF library using "-l elf"
	$(CXX) -o $@ $^ -l elf
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

-include $(DEPS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< $(INCL)
	$(CXX) $(CXXFLAGS) -MM -o $(patsubst %.o, %.d, $@) $< $(INCL)


#
# Enter for building func_instr unit test
#
test: unit_test
	@echo ""
	@echo "Running ./$<\n"
	@./$<
	@echo "Unit testing for the moduler functional instructions passed SUCCESSFULLY!"

OBJS= unit_test.o func_memory.o elf_parser.o func_instr.o
DEPS= $(OBJS:.o=.d)

unit_test: $(OBJS) $(GTEST_LIB)
	@# don't forget to link ELF library using "-l elf"
	@# and use "-lpthread" options for Google Test
	$(CXX) $(GTEST_INCL) -pthread -lpthread $^ -o $@ -l elf
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"


-include $(DEPS)

unit_test.o: unit_test.cpp $(GTEST_LIB)
	$(CXX) $(CXXFLAGS) $(GTEST_INCL) -c $< $(INCL)
	$(CXX) $(CXXFLAGS) $(GTEST_INCL) -MM -o $(patsubst %.o, %.d, $@) $< $(INCL)

$(GTEST_LIB):
	$(MAKE) -C $(GTEST_DIR)


clean:
	@-rm -rf *.o *.d
	@-rm -rf disasm unit_test
