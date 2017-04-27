#
# Building the elf_parser for MIPS ElF binaries
# @author Alexander Titov <alexander.igorevich.titov@gmail.com>
# Copyright 2012 uArchSim iLab Project
#

.PHONY: all gtest clean

# specifying relative path to the TRUNK
TRUNK= ../../..

# Including options and compiler flags
include $(TRUNK)/options.mk

CXXFLAGS+=-Wno-effc++

#
# Enter for building elf_parser stand alone program
#
OBJS= elf_parser.o main.o
DEPS= $(OBJS:.o=.d)

elf_parser: $(OBJS)
	@# don't forget to link ELF library using "-l elf"
	$(CXX) -o $@ $^ -l elf
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

-include $(DEPS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< $(INCL)
	$(CXX) $(CXXFLAGS) -MM -o $(patsubst %.o, %.d, $@) $< $(INCL)

#
# Enter for building elf_parser unit test
#
test: unit_test
	@echo ""
	@echo "Running ./$<\n"
	@./$<
	@echo "Unit testing for the moduler ELF parser passed SUCCESSFULLY!"

OBJS= unit_test.o elf_parser.o
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
	@-rm -rf elf_parser unit_test
