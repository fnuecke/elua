#include "platform.h"
#include "platform_conf.h"
#include "elua_int.h"
#include "common.h"

#include <sbi/riscv_asm.h>
#include <sbi/riscv_encoding.h>

#define INTERRUPT_FLAG _UL(0x80000000)

#include <stdio.h>

void platform_int_init()
{
  csr_write( CSR_MSTATUS, MSTATUS_MIE );
}

void timer_irq_handler()
{
   cmn_virtual_timer_cb();
}

typedef struct
{
  u32 reg[32];
  u32 mstatus;
  u32 mepc;
  u32 mbadaddr;
  u32 mcause;
  u32 insn;
} state;

state* trap_handler( state *s )
{
  if ( s->mcause & INTERRUPT_FLAG ) {
    switch ( s->mcause & ~INTERRUPT_FLAG ) {
      case IRQ_M_TIMER:
        timer_irq_handler();
        break;
      case IRQ_M_EXT:
        // ext_irq_handler();
        printf( "Unexpected external interrupt cause [%"PRIX"].\n", s->mcause );
        break;

      default:
        printf( "Unexpected interrupt cause [%"PRIX"].\n", s->mcause );
    }
  } else {
    // Only non-interrupt exceptions should be ebreak and ecall, we ignore both.
    s->mepc += 4;
  }

  return s;
}
