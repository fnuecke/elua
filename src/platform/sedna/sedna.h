#ifndef __SEDNA_H__
#define __SEDNA_H__

#define SYSCLK 25000000

extern u32 sedna_dram_top;
#define DRAM_TOP  (sedna_dram_top)

#define UART_TERM 0 /* NB: TERM must come first because other UARTs depend
                       on its address having been set in initialization. */
#define UART_RPC  1

#define SPI_SDMMC 0

#endif // #ifndef __SEDNA_H__

