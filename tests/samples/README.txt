MIPS binutils are installed at iLab servers and have the following shortcuts:

    mips-as          is an assembler for MIPS ISA
    mips-ld          is a linker for MIPS object files
    mips-objdumpt    dumps content of MIPS binary files (also disassebly instructions)
    mips-gcc         gcc compiler configured for MIPS

In order to create MIPS binary file do the following steps:

   1) create a file with assember code using and text editor
   2) save it as <test name>.s
   3) generate an object file:
          mips-as <test name>.s -o <test name>.o

   4) convert the object file into the binary file:
          mips-ld  <test name>.o -o  <test name>.out

   5*) look the content of <test name>.out using (pay attention only to .text section):
          mips-objdump -D <test name>.out

Or you can use the makefile in this directory.
