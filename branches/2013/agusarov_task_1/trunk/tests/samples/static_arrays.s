    .data # It says to the assembler that it is the beginning of the data section
          # All statical variables (and arrays) must be declared here.

dec_digits: .byte 0,1,2,3,4,5,6,7,8,9 # Statically allocate 10 bytes and initialize them
                                      # by given values.
                                      #
                                      # "decdigits" is a lable using which
                                      # you can refer to the begining of this array

best_nums: .word 7,11,13 # You also can declare arrays of words and other types
                         # Thus, here you allocate space for 3 variable each of 4 bytes,
                         # i.e. 12 bytes in sum.

just_space: .space 160  # It is not required to initialize the variables.
                        # You just need to say how many bytes you need.
                        # E.g. here 160 bytes are allocated.
                        # The lable "just_space" refers to the address of
                        # the beginning of this space.

    .text # It says to the assembler that it is the beginning of the text section.
          # All you code must be written here.

    .global __start # It is just makes a label "__start" visible outside this file.
                    # In fact, it is not needed if all you code is in one file.
 __start:
    la  $t3, best_nums # it says write the start address of the array "best_nums"
                       # into register $t3
    lw  $t2, 4($t3) # load the 2nd element of "best_nums" into $t2 
