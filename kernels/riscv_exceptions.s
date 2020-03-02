# RISC-V exception handler
# Author Eric Konks
# Copyright 2020 MIPT-V

	.data
__m1_:	.string "  Exception "
__m2_:	.string "  Interrupt "
__m3_:	.string "  occurred\n"

__i0_:	.string "  [User software interrupt] "
__i1_:	.string	"  [Supervisor software interrupt]"
__i2_:	.string	"  [Reserved for future standard use]"
__i3_:	.string	"  [Machine software interrupt]"
__i4_:	.string	"  [User timer interrupt]"
__i5_:	.string	"  [Supervisor timer interrupt]"
__i6_:	.string	"  [Reserved for future standard use]"
__i7_:	.string	"  [Machine timer interrupt]"
__i8_:	.string	"  [User external interrupt]"
__i9_:	.string	"  [Supervisor external interrupt]"
__i10_:	.string	"  [Reserved for future standard use] "
__i11_:	.string	"  [Machine external interrupt]"

__e0_:	.string	"  [Instruction address misaligned]"
__e1_:	.string	"  [Instruction access fault]"
__e2_:	.string	"  [Illegal instruction]"
__e3_:	.string	"  [Breakpoint]"
__e4_:	.string	"  [Load address misaligned]"
__e5_:	.string	"  [Load access fault]"
__e6_:	.string	"  [Store/AMO address misaligned]"
__e7_:	.string	"  [Store/AMO access fault]"
__e8_:	.string	"  [Environment call from U-mode]"
__e9_:	.string	"  [Environment call from S-mode]"
__e10_:	.string	"  [Reserved]"
__e11_:	.string	"  [Environment call from M-mode]"
__e12_:	.string	"  [Instruction page fault]"
__e13_:	.string	"  [Load page fault]"
__e14_:	.string	"  [Reserved for future standard use]"
__e15_:	.string	"  [Store/AMO page fault]"

__excp:	.word __e0_, __e1_, __e2_, __e3_, __e4_, __e5_, __e6_, __e7_, __e8_, __e9_
	.word __e10_, __e11_, __e12_, __e13_, __e14_

__intr:	.word __i0_, __i1_, __i2_, __i3_, __i4_, __i5_, __i6_, __i7_, __i8_, __i9_
	.word __i10_, __i10_, __i11_

	.section .text
	.globl _start
_start:
	csrr t0, scause		# Save cause register
	not t1, x0
	srli t1, t1, 0x1
	and t0, t0, t1		# Extract exception code
	not t1, t1		# Extract interrupt bit
	beqz t1, _not_intr		# Branch if not interrupt

	# Interrupt-specific code

	# Print information about interrupt
	li x2, 4		# Syscall 4 (print string)
	la x4, __m2_
	ecall
	li x2, 1		# Syscall 1 (print int)
	mv x4, t0
	ecall
	li x2, 4		# Syscall 4 (print string)
	lw x4, %lo(__intr)(t0)
	ecall
	li x2, 4		# Syscall 4 (print string)
	la x4, __m3_
	ecall

_not_intr:

	# Exception-specific code

	# Print information about exception
	li x2, 4		# Syscall 4 (print str)
	la x4, __m1_
	ecall
	li x2, 1		# Syscall 1 (print int)
	mv x4, t0
	ecall
	li x2, 4		# Syscall 4 (print string)
	lw x4, %lo(__excp)(t0)
	ecall
	li x2, 4		# Syscall 4 (print string)
	la x4, __m3_
	ecall

	# 0x2 Illegal insruction
	li t1, 2
	beq t0, t1, _terminate

	# 0x0 Instruction Address misaligned
	bnez t0, _ok_pc		# Branch if exception code != 0
	csrr t0, sepc
	andi t0, t0, 3		# Word-aligning check
	beqz t0, _ok_pc		# Branch if EPC word-aligned
	j _terminate

_terminate:
	li x2, 0xA		# Syscall 10 (exit)
	ecall

_ok_pc:

_return:
	csrwi scause, 0		# Clear cause register
	csrr t0, sepc
	addi t0, t0, 4		# Skip instruction to avoid infinite loop
	csrw sepc, t0 
	sret		# Return from exception
