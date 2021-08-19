// "CPU" Description for Bonfire SoC Version 1.0 on ARTY Board

#ifndef __CPU_RISCV_H__
#define __CPU_RISCV_H__

#define RISCV

#include "type.h"
#include "stacks.h"
#include "platform_ints.h"
#include "sedna.h"

#define PLATFORM_HAS_SYSTIMER

// Number of resources (0 if not available/not implemented)
#define NUM_PIO               0
#define NUM_SPI               1
#define NUM_UART              2
#define NUM_TIMER             1
#define NUM_PWM               0
#define NUM_ADC               0
#define NUM_CAN               0

// CPU frequency (needed by the CPU module and MMCFS code, 0 if not used)
#define CPU_FREQUENCY         SYSCLK

// PIO prefix ('0' for P0, P1, ... or 'A' for PA, PB, ...)
#define PIO_PREFIX            'A'
// Pins per port configuration:
// #define PIO_PINS_PER_PORT (n) if each port has the same number of pins, or
// #define PIO_PIN_ARRAY { n1, n2, ... } to define pins per port in an array
// Use #define PIO_PINS_PER_PORT 0 if this isn't needed
#define PIO_PINS_PER_PORT     0
// #define PIO_PIN_ARRAY {14,6}
// #define PORT_SHIFT {0,14}

//// Allocator data: define your free memory zones here in two arrays
//// (start address and end address)
#define INTERNAL_RAM1_FIRST_FREE ( end )
#define INTERNAL_RAM1_LAST_FREE  ( DRAM_TOP - STACK_SIZE_TOTAL )

#endif // #ifndef __CPU_RISCV_H__

