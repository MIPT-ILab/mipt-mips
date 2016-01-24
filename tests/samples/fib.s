.text

move  $t1, $zero
addi  $t2, $zero, 1 

fib:
add  $t3, $t1, $t2 # c := a + b
move $t1, $t2      # a := b
move $t2, $t3      # b := c
nop
j fib

