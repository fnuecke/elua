#ifndef __TYPE_H__
#define __TYPE_H__

#include <sys/types.h>

#ifndef NULL
#define NULL    ( ( void * )0 )
#endif

#ifndef FALSE
#define FALSE   ( 0 )
#endif
#ifndef false
#define false   ( 0 )
#endif

#ifndef TRUE
#define TRUE    ( 1 )
#endif
#ifndef true
#define true    ( 1 )
#endif

typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef unsigned long       DWORD;
typedef unsigned int        BOOL;

typedef unsigned int        bool;

typedef unsigned char       u8;
typedef unsigned char       uint8_t;
typedef signed char         s8;
typedef unsigned short      u16;
typedef unsigned short      uint16_t;
typedef signed short        s16;
typedef signed short        int16_t;
typedef unsigned int        u32;
typedef signed int          s32;

typedef unsigned long long  u64;
typedef signed long long    s64;
#define PRILX               "08lx"
#define PRIX                "08x"

#endif
