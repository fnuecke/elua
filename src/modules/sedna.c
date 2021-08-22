// Sedna proprietary module with some utility stuff.

#include "platform_conf.h"

#include "lauxlib.h"
#include "lualib.h"
#include "auxmods.h"
#include "lrotable.h"

#include <stdio.h>

extern void* fdt_address;

static int get_fdt_address( lua_State *L )
{
  lua_pushinteger( L, ( int )fdt_address );
  return 1;
}

#if _USE_MKFS && !_FS_READONLY
#include "ff.h"
static int format_disk( lua_State *L )
{
  int size = luaL_checkinteger( L , 1 );
  lua_pushinteger( L, f_mkfs( 0, 0, size ) );
  return 1;
}
#endif

#define MIN_OPT_LEVEL 2
#include "lrodefs.h"
const LUA_REG_TYPE sedna_map[] =
{
  { LSTRKEY( "fdt_address" ), LFUNCVAL( get_fdt_address ) },
#if _USE_MKFS && !_FS_READONLY
  { LSTRKEY( "format_disk" ), LFUNCVAL( format_disk ) },
#endif
  { LNILKEY, LNILVAL }
};

LUALIB_API int luaopen_sedna( lua_State* L )
{
  LREGISTER( L, AUXLIB_SEDNA, sedna_map );
}
