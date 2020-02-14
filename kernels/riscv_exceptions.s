	.data
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
	csrr t0, mcause		# Save cause register
	not t1, x0
	srli t1, t1, 0x1
	and t0, t0, t1		# Extract exception code
	not t1, t1		# Extract interrupt bit
	beqz t1, _not_intr		# Branch if not interrupt

	# Interrupt-specific code

_not_intr:

	# Exception-specific code

	bnez t1, _ok_pc		# EPC check

_bad_pc:
	csrr t0, mepc
	andi t0, t0, 0x3	# Word-aligning check
	beqz t0, _ok_pc

	# Specific code for misaligned instruction address

_ok_pc:

_return:
	csrwi mcause, 0		# Clear cause register
	csrr t0, mepc
	addi t0, t0, 4		# Skip instruction to avoid infinite loop
	csrw mepc, t0 
	mret		# Return from exception
