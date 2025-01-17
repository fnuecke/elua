/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ifndef __SERIAL_UART8250_H__
#define __SERIAL_UART8250_H__

#include "type.h"

void uart8250_putc( unsigned long base, char ch );

int uart8250_getc( unsigned long base );

int uart8250_init( unsigned long base, u32 in_freq, u32 baudrate, u32 reg_shift, u32 reg_width );

#endif
