// This header lists all interrupts defined for this platform

#ifndef __PLATFORM_INTS_H__
#define __PLATFORM_INTS_H__

#include "elua_int.h"

#define INT_UART_TERM        ( ELUA_INT_FIRST_ID )
#define INT_UART_RPC         ( ELUA_INT_FIRST_ID + 1 )
#define INT_TMR_MATCH        ( ELUA_INT_FIRST_ID + 2 )
#define INT_ELUA_LAST          INT_TMR_MATCH

#endif // #ifndef __PLATFORM_INTS_H__

