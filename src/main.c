#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "type.h"
#include "devman.h"
#include "platform.h"
#include "romfs.h"
#include "xmodem.h"
#include "shell.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "term.h"
#include "platform_conf.h"
#include "elua_rfs.h"
#ifdef ELUA_SIMULATOR
#include "hostif.h"
#endif

// Validate eLua configuratin options
#include "validate.h"

#include "mmcfs.h"
#include "romfs.h"
#include "semifs.h"

// Define here your autorun/boot files, 
// in the order you want eLua to search for them
char *boot_order[] = {
#if defined(BUILD_MMCFS)
  "/mmc/autorun.lua",
  "/mmc/autorun.lc",
#endif
#if defined(BUILD_ROMFS)
  "/rom/autorun.lua",
  "/rom/autorun.lc",
#endif
};

extern char etext[];


#ifdef ELUA_BOOT_RPC

#ifndef RPC_UART_ID
  #define RPC_UART_ID     CON_UART_ID
#endif

#ifndef RPC_TIMER_ID
  #define RPC_TIMER_ID    PLATFORM_TIMER_SYS_ID
#endif

#ifndef RPC_UART_SPEED
  #define RPC_UART_SPEED  CON_UART_SPEED
#endif

void boot_rpc( void )
{
  lua_State *L = lua_open();
  luaL_openlibs(L);  /* open libraries */
  
  // Set up UART for 8N1 w/ adjustable baud rate
  platform_uart_setup( RPC_UART_ID, RPC_UART_SPEED, 8, PLATFORM_UART_PARITY_NONE, PLATFORM_UART_STOPBITS_1 );
  
  // Start RPC Server
  lua_getglobal( L, "rpc" );
  lua_getfield( L, -1, "server" );
  lua_pushnumber( L, RPC_UART_ID );
  lua_pushnumber( L, RPC_TIMER_ID );
  lua_pcall( L, 2, 0, 0 );
}
#endif

// ****************************************************************************
//  Program entry point

int main( void )
{
  int i;
  FILE* fp;

  // Initialize platform first
  if( platform_init() != PLATFORM_OK )
  {
    // This should never happen
    while( 1 );
  }

  // Initialize device manager
//som to dal do platform.c  dm_init();

  dm_chdir_dev_id = DM_ERR_NO_DEVICE;

  // Register the ROM filesystem
  dm_register( romfs_init() );

  // Register the MMC filesystem
  dm_mmc_index = dm_register( mmcfs_init() );

  // Register the Semihosting filesystem
  dm_register( semifs_init() );

  // Register the remote filesystem
  dm_register( remotefs_init() );

  // Search for autorun files in the defined order and execute the 1st if found
  for( i = 0; i < sizeof( boot_order ) / sizeof( *boot_order ); i++ )
  {
    if( ( fp = fopen( boot_order[ i ], "r" ) ) != NULL )
    {
      fclose( fp );
      char* lua_argv[] = { "lua", boot_order[i], NULL };
      lua_main( 2, lua_argv );
      break; // autoruns only the first found
    }
  }

#ifdef ELUA_BOOT_RPC
  boot_rpc();
#else

  //elua_httpsd_run();
  //matrix_ssl_test();
  //elua_httpsd_init();

  // Run the shell
  if( shell_init() != 0 )
  {
    // Start Lua directly
    char* lua_argv[] = { "lua", NULL };
    lua_main( 1, lua_argv );
  }
  else
    shell_start();
#endif // #ifdef ELUA_BOOT_RPC

#ifdef ELUA_SIMULATOR
  hostif_exit(0);
  return 0;
#else
  while( 1 );
#endif
}
