This is based on qemu-0.10.6 and qemu-0.14.0.

YM2608 sound generator 'fmgen' is developed by cisc.

These patches for target-i386 are included in qemu-0.10.6-pc98:

2009-04-02 Immediate versions of ro[lr]
2009-06-29 CPUID Fn8000_0001.EAX is family/model/stepping, not features
2009-09-26 target-i386: use subfi instead of sub with a non-freed constant
2009-09-26 target-i386: kill a tmp register
2009-10-04 target-i386: add lock mov cr0 = cr8
2009-10-04 target-i386: Fix exceptions for fxsave/fxrstor
2009-10-06 target-i386: fix ARPL
2009-12-18 target-i386: Fix evaluation of DR7 register
2009-12-24 Intel CPUs starting from pentium have apic
2010-01-13 x86: translate.c: remove dead assignment
2010-02-05 remove two dead assignments in target-i386/translate.c
2010-03-06 target-i386: fix lddqu SSE instruction
2010-03-06 target-i386: Fix long jumps/calls in long mode with REX.W set
2010-03-10 target-i386: fix SIB decoding with index = 4
2010-03-13 target-i386: fix commit c22549204a6edc431e8e4358e61bd56386ff6957
2010-03-13 x86/cpuid: fix CPUID levels
2010-04-25 x86: remove dead assignments, spotted by clang analyzer
2010-06-04 target-i386: fix decoding of negative 4-byte displacements
2010-07-01 target-i386: fix xchg rax,r8
2010-07-01 Add more boundary checking to sse3/4 parsing
2010-07-25 target-i386: use gen_update_cc_op()
2010-08-22 target-i386: svm: Fix MSRPM check
