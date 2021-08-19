.section .init
.global _start

.extern memset

.extern platform_early_init
.extern main
.extern trap_handler

// Proprietary Sedna CSR used to switch 64 bit machine
// into 32 bit mode from within a pure 32 bit program.
#define CSR_SETXLEN32 0xBC0

// Size of register values in bytes.
#define REGBYTES 4

// Early stack top; actual stack top will be set after early platform init,
// which will return actual ram size read from FDT.
#define EARLY_STACK_TOP ( 0x80000000 + 512 * 1024 )

_start:
  // Enter 32 bit mode.
  csrwi  CSR_SETXLEN32, 1

  // Save a1 on stack, it's the address to the FDT.
  li     sp, EARLY_STACK_TOP
  addi   sp, sp,-4
  sw     a1, 4(sp)

  li     x1, 0
  li     x3, 0
  li     x4, 0
  li     x5, 0
  li     x6, 0
  li     x7, 0
  li     x8, 0
  li     x9, 0
  li     x10, 0
  li     x11, 0
  li     x12, 0
  li     x13, 0
  li     x14, 0
  li     x15, 0
  li     x16, 0
  li     x17, 0
  li     x18, 0
  li     x19, 0
  li     x20, 0
  li     x21, 0
  li     x22, 0
  li     x23, 0
  li     x24, 0
  li     x25, 0
  li     x26, 0
  li     x27, 0
  li     x28, 0
  li     x29, 0
  li     x30, 0
  li     x31, 0

  la     gp, _gp

  la     a1, __trap
  csrw   mtvec, a1

  // https://github.com/riscv/riscv-newlib/blob/f289cef6be67da67b2d97a47d6576fa7e6b4c858/libgloss/riscv/crt0.S#L29-L34
  // Clear the bss segment
  la     a0, _edata
  li     a1, 0
  la     a2, _end
  sub    a2, a2, a0
  call   memset

  // Pass FDT address to early init, return value is top of usable ram.
  lw     a0, 4(sp)
  call   platform_early_init
  mv     sp, a0

  call   main

endless:
  j      endless


__trap:
  csrw   mscratch, sp           // Save original stack pointer in scratch CSR.
  addi   sp, sp, -(37*REGBYTES) // We use the stack for generated state struct, reserve space.

  // Initializes state struct from platform_int.c
  // state.reg
  sw     x1, 1*REGBYTES(sp)    // ra
                               // sp (See below)
  sw     x3, 3*REGBYTES(sp)    // gp
  sw     x4, 4*REGBYTES(sp)    // tp
  sw     x5, 5*REGBYTES(sp)    // t0
  sw     x6, 6*REGBYTES(sp)    // t1
  sw     x7, 7*REGBYTES(sp)    // t2
  sw     x8, 8*REGBYTES(sp)    // s0
  sw     x9, 9*REGBYTES(sp)    // s1
  sw     x10, 10*REGBYTES(sp)  // a0
  sw     x11, 11*REGBYTES(sp)  // a1
  sw     x12, 12*REGBYTES(sp)  // a2
  sw     x13, 13*REGBYTES(sp)  // a3
  sw     x14, 14*REGBYTES(sp)  // a4
  sw     x15, 15*REGBYTES(sp)  // a5
  sw     x16, 16*REGBYTES(sp)  // a6
  sw     x17, 17*REGBYTES(sp)  // a7
  sw     x18, 18*REGBYTES(sp)  // s2
  sw     x19, 19*REGBYTES(sp)  // s3
  sw     x20, 20*REGBYTES(sp)  // s4
  sw     x21, 21*REGBYTES(sp)  // s5
  sw     x22, 22*REGBYTES(sp)  // s6
  sw     x23, 23*REGBYTES(sp)  // s7
  sw     x24, 24*REGBYTES(sp)  // s8
  sw     x25, 25*REGBYTES(sp)  // s9
  sw     x26, 26*REGBYTES(sp)  // s10
  sw     x27, 27*REGBYTES(sp)  // s11
  sw     x28, 28*REGBYTES(sp)  // t3
  sw     x29, 29*REGBYTES(sp)  // t4
  sw     x30, 30*REGBYTES(sp)  // t5
  sw     x31, 31*REGBYTES(sp)  // t6

  csrr   t0, mscratch
  sw     t0, 2*REGBYTES(sp)    // sp

  csrr   s0, mstatus
  sw     s0, 32*REGBYTES(sp)   // state.mstatus

  csrr   t1, mepc
  sw     t1, 33*REGBYTES(sp)   // state.mepc

  csrr   t2, mbadaddr
  sw     t2, 34*REGBYTES(sp)   // state.mbadaddr

  csrr   t3, mcause
  sw     t3, 35*REGBYTES(sp)   // state.mcause

  lw     t4, (t1)
  sw     t4, 36*REGBYTES(x2)   // state.insn

  mv     a0, sp                // First and only argument to method is pointer to state.
  jal    trap_handler          // Returns adjusted state in a0.

  lw     t1, 33*REGBYTES(a0)   // state.mepc
  csrw   mepc, t1

  lw     t1, 32*REGBYTES(a0)   // state.mstatus
  csrw   mstatus, t1

  // state.reg
  lw     x1, 1*REGBYTES(a0)    // ra
  lw     x2, 2*REGBYTES(a0)    // sp 
  lw     x3, 3*REGBYTES(a0)    // gp
  lw     x4, 4*REGBYTES(a0)    // tp
  lw     x5, 5*REGBYTES(a0)    // t0
  lw     x6, 6*REGBYTES(a0)    // t1
  lw     x7, 7*REGBYTES(a0)    // t2
  lw     x8, 8*REGBYTES(a0)    // s0
  lw     x9, 9*REGBYTES(a0)    // s1
                               // a0 (See below)
  lw     x11, 11*REGBYTES(a0)  // a1
  lw     x12, 12*REGBYTES(a0)  // a2
  lw     x13, 13*REGBYTES(a0)  // a3
  lw     x14, 14*REGBYTES(a0)  // a4
  lw     x15, 15*REGBYTES(a0)  // a5
  lw     x16, 16*REGBYTES(a0)  // a6
  lw     x17, 17*REGBYTES(a0)  // a7
  lw     x18, 18*REGBYTES(a0)  // s2
  lw     x19, 19*REGBYTES(a0)  // s3
  lw     x20, 20*REGBYTES(a0)  // s4
  lw     x21, 21*REGBYTES(a0)  // s5
  lw     x22, 22*REGBYTES(a0)  // s6
  lw     x23, 23*REGBYTES(a0)  // s7
  lw     x24, 24*REGBYTES(a0)  // s8
  lw     x25, 25*REGBYTES(a0)  // s9
  lw     x26, 26*REGBYTES(a0)  // s10
  lw     x27, 27*REGBYTES(a0)  // s11
  lw     x28, 28*REGBYTES(a0)  // t3
  lw     x29, 29*REGBYTES(a0)  // t4
  lw     x30, 30*REGBYTES(a0)  // t5
  lw     x31, 31*REGBYTES(a0)  // t6

  // Restore a0 last since it holds the pointer to our state data.
  lw     x10, 10*REGBYTES(a0)

  sret
