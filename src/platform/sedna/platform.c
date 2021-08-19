// Platform-dependent functions

#include <common.h>
#include <platform.h>
#include <platform_conf.h>

#include <libfdt/libfdt.h>
#include <sbi/riscv_asm.h>
#include <sbi/riscv_encoding.h>
#include <sbi_utils/serial/uart8250.h>

// ****************************************************************************
// FDT utility methods

#define INVALID_ADDRESS -1

void* fdt_address = ( void* ) INVALID_ADDRESS;

static int fdt_getaddrrange( const void *fdt, int nodeoffset, uint64_t *addr, uint64_t *size )
{
  int parent, cell_count, cell_size, len, i;
  const fdt32_t *reg;

  parent = fdt_parent_offset( fdt, nodeoffset );
  if ( parent < 0 )
    return parent;

  cell_count = fdt_address_cells( fdt, parent );
  if ( cell_count < 0 )
    return cell_count;
  cell_size = fdt_size_cells( fdt, parent );
  if ( cell_size < 0 )
    return cell_size;

  reg = ( const fdt32_t* )fdt_getprop( fdt, nodeoffset, "reg", &len );
  if ( !reg )
    return -FDT_ERR_NOTFOUND;

  if ( addr )
  {
    *addr = 0;
    for ( i = 0; i < cell_count; ++i )
      *addr = ( *addr << 32 ) | fdt32_to_cpu( *reg++ );
  }

  if ( size )
  {
    *size = 0;
    for ( i = 0; i < cell_size; ++i )
      *size = ( *size << 32 ) | fdt32_to_cpu( *reg++ );
  }

  return 0;
}

static unsigned long get_uart_base( unsigned id );

static int fdt_uart_node( unsigned id )
{
  switch ( id )
  {
    case UART_TERM: {
      // Use UART assigned as /chosen/stdout-path for term.
      int offset, len, result;
      u64 addr;
      const char *name;

      offset = fdt_path_offset( fdt_address, "/chosen" );
      if ( offset < 0 )
        return INVALID_ADDRESS;

      name = fdt_stringlist_get( fdt_address, offset, "stdout-path", 0, &len );
      if ( !name )
        return INVALID_ADDRESS;

      offset = fdt_path_offset( fdt_address, name );
      if ( offset < 0 )
        return INVALID_ADDRESS;

      result = fdt_getaddrrange( fdt_address, offset, &addr, NULL );
      if ( result < 0 || !addr )
        return INVALID_ADDRESS;

      return offset;
    }
    case UART_RPC: {
      // Use first non-term UART for RPC.
      int offset, result;
      u64 term_addr, addr;

      term_addr = get_uart_base( UART_TERM );

      offset = fdt_node_offset_by_compatible( fdt_address, -1, "ns16550a" );
      while ( offset >= 0 )
      {
        result = fdt_getaddrrange( fdt_address, offset, &addr, NULL );
        if ( result >= 0 && addr && addr != term_addr )
          return offset;

        offset = fdt_node_offset_by_compatible( fdt_address, offset, "ns16550a" );
      }

      return INVALID_ADDRESS;
    }
    default:
      return INVALID_ADDRESS;
  }
}

static unsigned long fdt_uart_base( unsigned id )
{
  int offset, result;
  u64 addr;

  offset = fdt_uart_node( id );
  if ( offset == INVALID_ADDRESS )
    return 0;

  result = fdt_getaddrrange( fdt_address, offset, &addr, NULL );
  if ( result < 0 || !addr )
    return 0;

  return addr;
}

// ****************************************************************************
// Platform initialization

u32 sedna_dram_top;

u32 platform_early_init( void* fdt )
{
  fdt_address = fdt;

  // Since the bootstrapper puts the FDT as far back in memory as possible, and we
  // want to keep the FDT around anyway, we can use that as our top of memory.
  sedna_dram_top = ( ( u32 )fdt ) & 0xFFFFFFF0;

  // TODO: Parse stuff we need from the FDT once, then throw it away.

  return sedna_dram_top;
}

int platform_init()
{
  cmn_systimer_set_base_freq( SYSCLK );
  cmn_platform_init();
  return PLATFORM_OK;
}

// ****************************************************************************
// CPU functions

int platform_cpu_set_global_interrupts( int status )
{
  unsigned long prev_status;
  if ( status == PLATFORM_CPU_ENABLE )
    prev_status = csr_read_set( CSR_MSTATUS, MSTATUS_MIE );
  else
    prev_status = csr_read_clear( CSR_MSTATUS, MSTATUS_MIE );
  if ( prev_status )
    return PLATFORM_CPU_ENABLE;
  else
    return PLATFORM_CPU_DISABLE;
}

// ****************************************************************************
// SPI functions

u32 platform_spi_setup( unsigned id, int mode, u32 clock, unsigned cpol, unsigned cpha, unsigned databits )
{
  switch ( id )
  {
    case SPI_SDMMC: {
      if ( mode != PLATFORM_SPI_MASTER )
        return 0;

      return clock;
    }
  }

  return 0;
}

spi_data_type platform_spi_send_recv( unsigned id, spi_data_type data )
{
  return data;
}

// ****************************************************************************
// PIO functions

pio_type platform_pio_op( unsigned port, pio_type pinmask, int op )
{
  // Need to implement this for the MMCFS module, don't have actual PIO.
  return 0;
}

// ****************************************************************************
// UART functions

static unsigned long uart_addresses[ NUM_UART ] = { INVALID_ADDRESS };

static unsigned long get_uart_base( unsigned id )
{
  if ( id < 0 || id >= NUM_UART )
    return 0;
  if ( uart_addresses[ id ] == INVALID_ADDRESS )
    uart_addresses[ id ] = fdt_uart_base( id );
  return uart_addresses[ id ];
}

static void fdt_uart_config( int nodeoffset, u32 *in_freq, u32 *baudrate, u32 *reg_shift, u32 *reg_width )
{
  int len;
  const fdt32_t *prop;

  if ( in_freq )
  {
    *in_freq = 3686400;

    prop = ( const fdt32_t* )fdt_getprop( fdt_address, nodeoffset, "clock-frequency", &len );
    if ( prop && len > 0 )
      *in_freq = fdt32_to_cpu( *prop );
  }

  if ( baudrate )
  {
    *baudrate = 115200;

    prop = ( const fdt32_t* )fdt_getprop( fdt_address, nodeoffset, "current-speed", &len );
    if ( prop && len > 0 )
      *in_freq = fdt32_to_cpu( *prop );
  }

  if ( reg_shift )
  {
    *reg_shift = 0;

    prop = ( const fdt32_t* )fdt_getprop( fdt_address, nodeoffset, "reg-shift", &len );
    if ( prop && len > 0 )
      *in_freq = fdt32_to_cpu( *prop );
  }

  if ( reg_width )
  {
    *reg_width = 1;

    prop = ( const fdt32_t* )fdt_getprop( fdt_address, nodeoffset, "reg-io-width", &len );
    if ( prop && len > 0 )
      *in_freq = fdt32_to_cpu( *prop );
  }
}

u32 platform_uart_setup( unsigned id, u32 baud, int databits, int parity, int stopbits )
{
  if ( id > NUM_UART - 1 )
    return 0;

  int offset;
  u32 in_freq, baudrate, reg_shift, reg_width;
  unsigned long base;

  offset = fdt_uart_node( id );
  if ( offset == INVALID_ADDRESS )
    return 0;

  base = get_uart_base( id );
  if ( base == 0 )
    return 0;

  fdt_uart_config( offset, &in_freq, &baudrate, &reg_shift, &reg_width );

  uart8250_init( base, in_freq, baudrate, reg_shift, reg_width );

  return baudrate;
}

void platform_s_uart_send( unsigned id, u8 data )
{
  unsigned long base = get_uart_base( id );
  if ( base == 0 )
    return;

  uart8250_putc( base, ( char )data );
}

int platform_s_uart_recv( unsigned id, timer_data_type timeout )
{
  unsigned long base = get_uart_base( id );
  if ( base == 0 )
    return -1;

  if ( timeout == 0 )
    return uart8250_getc( base );
  for ( ; ; )
  {
    int result = uart8250_getc( base );
    if ( result != -1 )
      return result;
  }
}

int platform_s_uart_set_flow_control( unsigned id, int type )
{
  return PLATFORM_ERR;
}

// ****************************************************************************
// System timer functions

timer_data_type platform_timer_read_sys( void )
{
  return cmn_systimer_get();
}

u64 platform_timer_sys_raw_read()
{
  for ( ; ; ) {
    u32 hi = ( u32 )csr_read( CSR_MCYCLEH );
    u32 lo = ( u32 )csr_read( CSR_MCYCLE );
    if ( hi == csr_read( CSR_MCYCLEH ) )
      return ( ( u64 )hi << 32 ) | lo;
  }
}

void platform_timer_sys_enable_int()
{
}

void platform_timer_sys_disable_int()
{
}

timer_data_type platform_s_timer_op( unsigned id, int op, timer_data_type data )
{
  return 0;
}

// ****************************************************************************
// Timer module functions

void platform_s_timer_delay( unsigned id, timer_data_type delay_us )
{
}
