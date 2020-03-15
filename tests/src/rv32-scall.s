# RISC-V syscall test
# Author Eric Konks
# Copyright 2020 MIPT-V

	.section .text
	.globl _start
_start:
	li x2, 5		# Syscall(5) Read integer
	ecall
	mv t1, x2		# Checker and PerfSim should have same t1 value
