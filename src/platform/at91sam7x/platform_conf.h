// eLua platform configuration

#ifndef __PLATFORM_CONF_H__
#define __PLATFORM_CONF_H__

#include "auxmods.h"
#include "board.h"
#include "stacks.h"
#include "buf.h"
#include "elua_int.h"
#include "sermux.h"
#include "uip.h"

// *****************************************************************************
// Define here what components you want for this platform

// this will never be #define BUILD_DAEMON_SUPPORT_FOR_UIP
#define BUILD_LWIP
//#define BUILD_SSL

//#define BUILD_UIP
//#define BUILD_DHCPC
//#define BUILD_DNS
//#define BUILD_CON_TCP

//#define BUILD_XMODEM
#define BUILD_SHELL
#define BUILD_ROMFS
#define BUILD_TERM
#define BUILD_CON_GENERIC

// support for arrow keys, history and some key sequences in the interpreter.
// requires BUILD_TERM. http://www.eluaproject.net/doc/v0.8/en_linenoise.html
#define BUILD_LINENOISE

//#define BUILD_RPC
//#define BUILD_RFS
//#define BUILD_SERMUX
#define BUILD_C_INT_HANDLERS


#define BUILD_MMCFS
#define MMCFS_CS_PORT 0 //AT91C_BASE_PIOA
#define MMCFS_CS_PIN 13 //AT91C_PA13_SPI0_NPCS1     //AT91C_PA11_NPCS0
#define MMCFS_SPI_NUM 0


// trace and assert are writing to dbgu port
//#define STDERR 1
#define TRACE_LEVEL TRACE_LEVEL_DEBUG


#define PLATFORM_HAS_SYSTIMER

// *****************************************************************************
// UART/Timer IDs configuration data (used in main.c)

//#define CON_UART_ID         ( SERMUX_SERVICE_ID_FIRST + 1 )
#define CON_UART_ID           0
#define CON_UART_SPEED        115200
#define TERM_LINES            25
#define TERM_COLS             80

// *****************************************************************************
// Auxiliary libraries that will be compiled for this platform

#if defined( ELUA_BOOT_RPC ) && !defined( BUILD_RPC )
#define BUILD_RPC
#endif

#if defined BUILD_UIP //|| defined BUILD_LWIP
#define NETLINE  _ROM( AUXLIB_NET, luaopen_net, net_map )
#else
#define NETLINE
#endif

#if defined( BUILD_RPC ) 
#define RPCLINE _ROM( AUXLIB_RPC, luaopen_rpc, rpc_map )
#else
#define RPCLINE
#endif

#define LUA_PLATFORM_LIBS_ROM\
  _ROM( AUXLIB_PIO, luaopen_pio, pio_map )\
  _ROM( AUXLIB_TMR, luaopen_tmr, tmr_map )\
  _ROM( AUXLIB_PD, luaopen_pd, pd_map )\
  _ROM( AUXLIB_UART, luaopen_uart, uart_map )\
  _ROM( AUXLIB_TERM, luaopen_term, term_map )\
  _ROM( AUXLIB_PWM, luaopen_pwm, pwm_map )\
  _ROM( AUXLIB_PACK, luaopen_pack, pack_map )\
  _ROM( AUXLIB_BIT, luaopen_bit, bit_map )\
  NETLINE\
  _ROM( AUXLIB_CPU, luaopen_cpu, cpu_map )\
  _ROM( AUXLIB_ELUA, luaopen_elua, elua_map )\
  RPCLINE\
  _ROM( LUA_MATHLIBNAME, luaopen_math, math_map )\
  _ROM( LUA_FATFSLIBNAME, luaopen_fatfs, fatfs_map )\
  _ROM( LUA_WSAPILIBNAME, luaopen_wsapi, wsapi_map )

// *****************************************************************************
// Configuration data

/// The MAC address used for this project
//static const struct uip_eth_addr MacAddress = {{0x00, 0x45, 0x56, 0x78, 0x9a, 0xbc}};
//TODO: mal by som vymysliet nejaky kusok pamate, kde to bude nakonfigurovane a
//   namiesto pristupu k tejto globalnej premennej sa to z tadial precita
static const u8_t  MacAddress[6] = {0x00, 0x45, 0x56, 0x78, 0x9a, 0xbc};

// Static TCP/IP configuration
#define ELUA_CONF_IPADDR0     192
#define ELUA_CONF_IPADDR1     168
#define ELUA_CONF_IPADDR2     100
#define ELUA_CONF_IPADDR3     90

#define ELUA_CONF_NETMASK0    255
#define ELUA_CONF_NETMASK1    255
#define ELUA_CONF_NETMASK2    255
#define ELUA_CONF_NETMASK3    0

#define ELUA_CONF_DEFGW0      192
#define ELUA_CONF_DEFGW1      168
#define ELUA_CONF_DEFGW2      100
#define ELUA_CONF_DEFGW3      20

#define ELUA_CONF_DNS0        192
#define ELUA_CONF_DNS1        168
#define ELUA_CONF_DNS2        100
#define ELUA_CONF_DNS3        20

// *****************************************************************************
// Configuration data

// Virtual timers (0 if not used)
#define VTMR_NUM_TIMERS       4
#define VTMR_FREQ_HZ          4

// Number of resources (0 if not available/not implemented)
#define NUM_PIO               2
#define NUM_SPI               0
#define NUM_UART              2
#if VTMR_NUM_TIMERS > 0
#define NUM_TIMER             3
#else
#define NUM_TIMER             2
#endif
#define NUM_PWM               4
#define NUM_ADC               0
#define NUM_CAN               0

// RPC boot options
#define RPC_UART_ID           CON_UART_ID
#define RPC_UART_SPEED        CON_UART_SPEED

// Enable RX buffering on UART
#define BUF_ENABLE_UART
#define CON_BUF_SIZE          BUF_SIZE_128

// Linenoise buffer sizes
#define LINENOISE_HISTORY_SIZE_LUA    50
#define LINENOISE_HISTORY_SIZE_SHELL  10

// CPU frequency (needed by the CPU module and MMCFS code, 0 if not used)
#define CPU_FREQUENCY         BOARD_MCK

// PIO prefix ('0' for P0, P1, ... or 'A' for PA, PB, ...)
#define PIO_PREFIX            'A'
// Pins per port configuration:
// #define PIO_PINS_PER_PORT (n) if each port has the same number of pins, or
// #define PIO_PIN_ARRAY { n1, n2, ... } to define pins per port in an array
// Use #define PIO_PINS_PER_PORT 0 if this isn't needed
#define PIO_PINS_PER_PORT     31

// Allocator data: define your free memory zones here in two arrays
// (start address and end address)
#define SRAM_ORIGIN           0x200000
#ifdef at91sam7x256
  #define SRAM_SIZE           0x10000
#else
  #define SRAM_SIZE           0x20000
#endif
#define MEM_START_ADDRESS     { ( void* )end }
#define MEM_END_ADDRESS       { ( void* )( SRAM_ORIGIN + SRAM_SIZE - STACK_SIZE_TOTAL - 1 ) }

/*
#define RFS_BUFFER_SIZE       BUF_SIZE_512
#define RFS_UART_ID           ( SERMUX_SERVICE_ID_FIRST )
#define RFS_TIMEOUT           100000
#define RFS_UART_SPEED        115200

#define SERMUX_PHYS_ID        0
#define SERMUX_PHYS_SPEED     115200
#define SERMUX_NUM_VUART      2
#define SERMUX_BUFFER_SIZES   { RFS_BUFFER_SIZE, CON_BUF_SIZE }
*/

// Interrupt list
#define INT_UART_RX           ELUA_INT_FIRST_ID
#define INT_ELUA_LAST         INT_UART_RX

#define PLATFORM_CPU_CONSTANTS\
 _C( INT_UART_RX )

#endif // #ifndef __PLATFORM_CONF_H__
