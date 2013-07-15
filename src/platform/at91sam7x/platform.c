// platform-dependent functions

#include "platform.h"
#include "type.h"
#include "pio.h"
#include "usart.h"
#include "devman.h"
#include "pmc.h"
#include "genstd.h"
#include "tc.h"
#include "stacks.h"
#include "pwmc.h"
#include "board.h"
#include <reent.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "utils.h"
#include "common.h"
#include "aic.h"
#include "platform_conf.h"
#include "buf.h"
#include "pit.h"

#include "dbgu.h"
#include "trace.h"

#if  defined BUILD_UIP || defined BUILD_LWIP
#define BUILD_ETHERNET
#endif

#ifdef BUILD_ETHERNET
#include "ethernet/emac.h"
#include "ethernet/dm9161/dm9161.h"

static u8 eth_timer_fired;

#endif

#ifdef  BUILD_UIP
#include "uip_arp.h"
#include "elua_uip.h"
#include "uip-conf.h"
#endif

#ifdef  BUILD_LWIP
#include "elua_lwIP.h"
#endif


u32 platform_ethernet_setup();

// "Stubs" used for our interrupt handlers
// Just a trick to avoid interworking and some other complications

#define INT_STUB( func )\
  asm volatile(\
  "push {lr}\n\t"\
  "bl   " #func "\n\t"\
  "pop  {r0}\n\t"\
  "bx   r0\n\t"\
 )\

// ****************************************************************************
// AT91SAM7X system timer
// We implement this using the PIT, as it has a 20-bit counter (the timers only
// have 16-bit counters) and is not used by eLua in any other way. It is clocked
// at 3MHz (MCLK/16) which means we have 3 ticks per microsecond. To keep things
// as precise as possible, we choose the counter limit to be a multiple of 3.
// This translates to limit = 1048575 (kept as high as possible to minimize system
// impact), which means PIV = 1048574 (since the period is PIV + 1) which in turn
// means 349525us/interrupt

//#define SYSTIMER_US_PER_INTERRUPT 349525
//#define SYSTIMER_LIMIT            1048574
//#define SYSTIMER_MASK             ( ( 1 << 20 ) - 1 )

#define PIT_PERIOD          1000


void __isr_pit_helper()
{
  PIT_GetPIVR();
  cmn_systimer_periodic();
  AT91C_BASE_AIC->AIC_ICCR = 1 << AT91C_ID_SYS;

#ifdef BUILD_ETHERNET
  // Indicate that a SysTick interrupt has occurred.
  eth_timer_fired = 1;

  // Generate a fake Ethernet interrupt.  This will perform the actual work
  // of incrementing the timers and taking the appropriate actions.
  platform_eth_force_interrupt();
#endif //BUILD_ETHERNET
}

static void __attribute__((naked)) ISR_Pit()
{
  INT_STUB( __isr_pit_helper );
}

static void platform_systimer_init()
{
  //PIT_SetPIV( SYSTIMER_LIMIT );
  PIT_Init(PIT_PERIOD, BOARD_MCK / 1000000);
  AIC_ConfigureIT( AT91C_ID_SYS, 0, ISR_Pit );
  PIT_EnableIT();
  AIC_EnableIT( AT91C_ID_SYS );
  PIT_Enable();
}

// ****************************************************************************
// Platform initialization

static const Pin platform_uart_pins[ 2 ][ 2 ] = 
{
  { PIN_USART0_RXD, PIN_USART0_TXD },
  { PIN_USART1_RXD, PIN_USART1_TXD }
};
static const AT91S_TC* timer_base[] = { AT91C_BASE_TC0, AT91C_BASE_TC1, AT91C_BASE_TC2 };

#if VTMR_NUM_TIMERS > 0
void __isr_tc2_helper()
{
  cmn_virtual_timer_cb();
  AT91C_BASE_TC2->TC_SR;
}

static void __attribute__((naked)) ISR_Tc2()
{
  INT_STUB( __isr_tc2_helper );
}
#endif

int platform_init()
{
  int i;
   
  // Enable the peripherals we use in the PMC
  PMC_EnablePeripheral( AT91C_ID_US0 );  
  PMC_EnablePeripheral( AT91C_ID_US1 );
  PMC_EnablePeripheral( AT91C_ID_PIOA );
  PMC_EnablePeripheral( AT91C_ID_PIOB );
  PMC_EnablePeripheral( AT91C_ID_TC0 );
  PMC_EnablePeripheral( AT91C_ID_TC1 );
  PMC_EnablePeripheral( AT91C_ID_TC2 );
  PMC_EnablePeripheral( AT91C_ID_PWMC );  
  
//#if  !defined(NOASSERT) && !defined(NOTRACE)
  TRACE_CONFIGURE(DBGU_STANDARD, 115200, BOARD_MCK);
//#endif

  //printf("XXXXX\r\n");
  // Configure the timers
  AT91C_BASE_TCB->TCB_BMR = 0x15;
  for( i = 0; i < 3; i ++ )
    TC_Configure( ( AT91S_TC* )timer_base[ i ], AT91C_TC_CLKS_TIMER_DIV5_CLOCK | AT91C_TC_WAVE );
        
  // here is also registration of std_send_func
  cmn_platform_init();

  // Initialize device manager
  dm_init();
  //std_set_send_func(fputs);
  fprintf( stderr, "!!!");

  // PWM setup (only the clocks are set at this point)
  PWMC_ConfigureClocks( BOARD_MCK, BOARD_MCK, BOARD_MCK );
  PWMC_ConfigureChannel( 0, AT91C_PWMC_CPRE_MCKA, 0, 0 );
  PWMC_ConfigureChannel( 1, AT91C_PWMC_CPRE_MCKA, 0, 0 );  
  PWMC_ConfigureChannel( 2, AT91C_PWMC_CPRE_MCKB, 0, 0 );
  PWMC_ConfigureChannel( 3, AT91C_PWMC_CPRE_MCKB, 0, 0 );    
  for( i = 0; i < 4; i ++ )
  {
    PWMC_EnableChannel( i );
    PWMC_EnableChannelIt( i );
  }

  

#if VTMR_NUM_TIMERS > 0
  // Virtual timer initialization
  TC_Configure( AT91C_BASE_TC2, AT91C_TC_CLKS_TIMER_DIV5_CLOCK | AT91C_TC_WAVE | AT91C_TC_WAVESEL_UP_AUTO );
  AT91C_BASE_TC2->TC_RC = ( BOARD_MCK / 1024 ) / VTMR_FREQ_HZ;
  AIC_DisableIT( AT91C_ID_TC2 );
  AIC_ConfigureIT( AT91C_ID_TC2, 0, ISR_Tc2 );
  AT91C_BASE_TC2->TC_IER = AT91C_TC_CPCS;
  AIC_EnableIT( AT91C_ID_TC2 );  
  TC_Start( AT91C_BASE_TC2 );
#endif  

  // Initialize the system timer
//  cmn_systimer_set_base_freq( BOARD_MCK / 16 );
//  cmn_systimer_set_interrupt_period_us( SYSTIMER_US_PER_INTERRUPT );
  cmn_systimer_set_base_freq( BOARD_MCK );
  cmn_systimer_set_interrupt_period_us( PIT_PERIOD );
  platform_systimer_init();

#ifdef BUILD_ETHERNET
  // Initialize Ethernet
  platform_ethernet_setup();
#endif // BUILD_ETHERNET

  return PLATFORM_OK;
} 

// ****************************************************************************
// PIO functions

static Pin pio_port_desc[] = 
{
  { 0, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEFAULT },
  { 0, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_INPUT, PIO_DEFAULT }
};

pio_type platform_pio_op( unsigned port, pio_type pinmask, int op )
{
  Pin* pin;
  pio_type retval = 1;
  
  pin = pio_port_desc + port;
  pin->mask = pinmask;
  switch( op )
  {
    case PLATFORM_IO_PORT_SET_VALUE:    
    case PLATFORM_IO_PIN_SET:
      PIO_Set( pin );
      break;
      
    case PLATFORM_IO_PIN_CLEAR:
      PIO_Clear( pin );
      break;
      
    case PLATFORM_IO_PORT_DIR_INPUT:
      pin->mask = 0x7FFFFFFF;      
    case PLATFORM_IO_PIN_DIR_INPUT:
      pin->type = PIO_INPUT;
      PIO_Configure( pin, 1 );
      break;
      
    case PLATFORM_IO_PORT_DIR_OUTPUT:      
      pin->mask = 0x7FFFFFFF;      
    case PLATFORM_IO_PIN_DIR_OUTPUT:
      pin->type = PIO_OUTPUT_0;
      PIO_Configure( pin, 1 );
      break;      
            
    case PLATFORM_IO_PORT_GET_VALUE:
      pin->mask = 0x7FFFFFFF;
      pin->type = PIO_INPUT;
      retval = PIO_Get( pin );
      break;
      
    case PLATFORM_IO_PIN_GET:
      retval = PIO_Get( pin ) & pinmask ? 1 : 0;
      break;
      
    case PLATFORM_IO_PIN_PULLUP:
      pin->pio->PIO_PPUER = pinmask;
      break;
      
    case PLATFORM_IO_PIN_NOPULL:
      pin->pio->PIO_PPUDR = pinmask;
      break;
      
    default:
      retval = 0;
      break;
  }
  return retval;
}

// ****************************************************************************
// UART functions

u32 platform_uart_setup( unsigned id, u32 baud, int databits, int parity, int stopbits )
{
  unsigned int mode;
  AT91S_USART* base = id == 0 ? AT91C_BASE_US0 : AT91C_BASE_US1;

  // Setup mode
  mode = AT91C_US_USMODE_NORMAL | AT91C_US_CHMODE_NORMAL | AT91C_US_CLKS_CLOCK;  
  switch( databits )
  {
    case 5:
      mode |= AT91C_US_CHRL_5_BITS;
      break;      
    case 6:
      mode |= AT91C_US_CHRL_6_BITS;
      break;      
    case 7:
      mode |= AT91C_US_CHRL_7_BITS;
      break;      
    case 8:
      mode |= AT91C_US_CHRL_8_BITS;
      break;
  }
  if( parity == PLATFORM_UART_PARITY_EVEN )
    mode |= AT91C_US_PAR_EVEN;
  else if( parity == PLATFORM_UART_PARITY_ODD )
    mode |= AT91C_US_PAR_ODD;
  else
    mode |= AT91C_US_PAR_NONE;
  if( stopbits == PLATFORM_UART_STOPBITS_1 )
    mode |= AT91C_US_NBSTOP_1_BIT;
  else if( stopbits == PLATFORM_UART_STOPBITS_1_5 )
    mode |= AT91C_US_NBSTOP_15_BIT;
  else
    mode |= AT91C_US_NBSTOP_2_BIT;
      
  // Configure pins
  PIO_Configure(platform_uart_pins[ id ], PIO_LISTSIZE(platform_uart_pins[ id ]));
    
  // Configure the USART in the desired mode @115200 bauds
  baud = USART_Configure( base, mode, baud, BOARD_MCK );
  
  // Enable receiver & transmitter
  USART_SetTransmitterEnabled(base, 1);
  USART_SetReceiverEnabled(base, 1);
  
  // Return actual baud    
  return baud;
}

void platform_s_uart_send( unsigned id, u8 data )
{
  AT91S_USART* base = id == 0 ? AT91C_BASE_US0 : AT91C_BASE_US1;  
  
  USART_Write( base, data, 0 );
}

int platform_s_uart_recv( unsigned id, timer_data_type timeout )
{
  AT91S_USART* base = id == 0 ? AT91C_BASE_US0 : AT91C_BASE_US1;  
    
  if( timeout == 0 )
  {
    // Return data only if already available
    if( USART_IsDataAvailable( base ) )
      return USART_Read( base, 0 );
    else
      return -1;
  }
  return USART_Read( base, 0 );
}

int platform_s_uart_set_flow_control( unsigned id, int type )
{
  return PLATFORM_ERR;
}

// to have stderr on DBGU console
void platform_stderr_send( u8 data )
{
  DBGU_PutChar(data);
}

// ****************************************************************************
// Timer functions

static const unsigned clkdivs[] = { 2, 8, 32, 128, 1024 };

// Helper: get timer clock
static u32 platform_timer_get_clock( unsigned id )
{
  u32 div;
  
  div = timer_base[ id ]->TC_CMR & 7;
  if( div > 4 )
    div = 4;
  return BOARD_MCK / clkdivs[ div ];
}

// Helper: set timer clock
static u32 platform_timer_set_clock( unsigned id, u32 clock )
{
  unsigned i, mini = 0;
  
  for( i = 0; i < 5; i ++ )
    if( ABSDIFF( clock, BOARD_MCK / clkdivs[ i ] ) < ABSDIFF( clock, BOARD_MCK / clkdivs[ mini ] ) )
      mini = i;
  TC_Configure( ( AT91S_TC* )timer_base[ id ], mini | AT91C_TC_WAVE );  
  return BOARD_MCK / clkdivs[ mini ];
}

void platform_s_timer_delay( unsigned id, timer_data_type delay_us )
{
  AT91S_TC* base = ( AT91S_TC* )timer_base[ id ];  
  u32 freq;
  timer_data_type final;
  volatile int i;
    
  freq = platform_timer_get_clock( id );
  final = ( ( u64 )delay_us * freq ) / 1000000;
  if( final > 0xFFFF )
    final = 0xFFFF;
  TC_Start( base );  
  for( i = 0; i < 200; i ++ );
  while( base->TC_CV < final );  
}

timer_data_type platform_s_timer_op( unsigned id, int op, timer_data_type data )
{
  u32 res = 0;
  AT91S_TC* base = ( AT91S_TC* )timer_base[ id ];
  volatile int i;
  
  switch( op )
  {
    case PLATFORM_TIMER_OP_START:
      res = 0;
      TC_Start( base );
      for( i = 0; i < 200; i ++ );      
      break;
      
    case PLATFORM_TIMER_OP_READ:
      res = base->TC_CV;
      break;
      
    case PLATFORM_TIMER_OP_SET_CLOCK:
      res = platform_timer_set_clock( id, data );
      break;
      
    case PLATFORM_TIMER_OP_GET_CLOCK:
      res = platform_timer_get_clock( id );
      break;

    case PLATFORM_TIMER_OP_GET_MAX_CNT:
      res = 0xFFFF;
      break;
  }
  return res;
}

u64 platform_timer_sys_raw_read()
{
  return (PIT_GetPIIR() >> 20);
}

void platform_timer_sys_disable_int()
{
  PIT_DisableIT();
}

void platform_timer_sys_enable_int()
{
  PIT_EnableIT();
}

timer_data_type platform_timer_read_sys()
{
  return cmn_systimer_get();
}

// ****************************************************************************
// PWMs

// PWM0, PWM1 -> they can modify CLKA and are statically assigned to CLKA
// PWM2, PWM3 -> they can modify CLKB and are statically assigned to CLKB

// PWM pins
static const Pin pwm_pins[] = { PIN_PWMC_PWM0, PIN_PWMC_PWM1, PIN_PWMC_PWM2, PIN_PWMC_PWM3 };

// Return the PWM clock
u32 platform_pwm_get_clock( unsigned id )
{
  u32 cfg = AT91C_BASE_PWMC->PWMC_CH[ id ].PWMC_CMR;
  u16 clkdata;
  
  clkdata = cfg & 0x0F;
  if( clkdata < 11 )
    return BOARD_MCK / ( 1 << clkdata );
  else
  {
    // clka / clkb
    cfg = AT91C_BASE_PWMC->PWMC_MR;
    if( clkdata == 12 ) // clkb
      cfg >>= 16;
    clkdata = cfg & 0x0FFF;
    return BOARD_MCK / ( ( clkdata & 0xFF ) * ( 1 << ( clkdata >> 8 ) ) );
  }
}

// Set the PWM clock
u32 platform_pwm_set_clock( unsigned id, u32 clock )
{
  if( id < 2 )
    PWMC_ConfigureClocks( clock, 0, BOARD_MCK );
  else
    PWMC_ConfigureClocks( 0, clock, BOARD_MCK );
  return platform_pwm_get_clock( id );
}

u32 platform_pwm_setup( unsigned id, u32 frequency, unsigned duty )
{
  u32 pwmclk = platform_pwm_get_clock( id );
  u32 period;  
  volatile u32 dummy;
  
  // Compute period
  period = pwmclk / frequency;
  // Set the period
  dummy = AT91C_BASE_PWMC->PWMC_ISR;
  PWMC_SetPeriod( id, period );
  while( ( AT91C_BASE_PWMC->PWMC_ISR & ( 1 << id ) ) == 0 );
  // Set duty cycle
  PWMC_SetDutyCycle( id, ( period * duty ) / 100 );
  // Return actual frequency
  return pwmclk / period;
}

void platform_pwm_start( unsigned id )
{
  volatile u32 dummy;
  PIO_Configure( pwm_pins + id, 1 );    
  dummy = AT91C_BASE_PWMC->PWMC_ISR;
}

void platform_pwm_stop( unsigned id )
{
  platform_pio_op( 1, 1 << ( 19 + id ), PLATFORM_IO_PIN_DIR_INPUT );
}

// ****************************************************************************
// Network support

#ifdef BUILD_ETHERNET
/// EMAC power control pin
#if !defined(BOARD_EMAC_POWER_ALWAYS_ON)
static const Pin emacPwrDn[] = {BOARD_EMAC_PIN_PWRDN};
#endif

/// The PINs' on PHY reset
static const Pin emacRstPins[] = {BOARD_EMAC_RST_PINS};

/// The PINs for EMAC
static const Pin emacPins[] = {BOARD_EMAC_RUN_PINS};

/// PHY address
#define EMAC_PHY_ADDR             31


/// The DM9161 driver instance
static Dm9161 gDm9161; //TODO: LDuck: check,if there is any reason, why this variable is global

//-----------------------------------------------------------------------------
/// Emac interrupt handler
//-----------------------------------------------------------------------------
static void ISR_Emac(void)
{
    EMAC_Handler();
}

u32 platform_ethernet_setup()
{
    Dm9161       *pDm = &gDm9161;
    unsigned int errCount = 0;


//    printf("-- Basic EMAC uIP Project %s --\n\r");
//    printf("-- %s\n\r", BOARD_NAME);
//    TRACE_INFO("-- Compiled: %s %s --\n\r", __DATE__, __TIME__);

    // Display MAC & IP settings
//    printf(" - MAC %x:%x:%x:%x:%x:%x\n\r",
//           MacAddress.addr[0], MacAddress.addr[1], MacAddress.addr[2],
//           MacAddress.addr[3], MacAddress.addr[4], MacAddress.addr[5]);
//
//#if !defined(UIP_DHCP_on)
//    printf(" - Host IP  %d.%d.%d.%d\n\r",
//           HostIpAddress[0], HostIpAddress[1], HostIpAddress[2], HostIpAddress[3]);
//    printf(" - Router IP  %d.%d.%d.%d\n\r",
//           RoutIpAddress[0], RoutIpAddress[1], RoutIpAddress[2], RoutIpAddress[3]);
//    printf(" - Net Mask  %d.%d.%d.%d\n\r",
//           NetMask[0], NetMask[1], NetMask[2], NetMask[3]);
//#endif

#if !defined(BOARD_EMAC_POWER_ALWAYS_ON)
    // clear PHY power down mode
    PIO_Configure(emacPwrDn, 1);
#endif

    // Init EMAC driver structure
    //bolo pre uIP: EMAC_Init(AT91C_ID_EMAC, MacAddress.addr, EMAC_CAF_ENABLE, EMAC_NBC_DISABLE);
    EMAC_Init(AT91C_ID_EMAC, MacAddress, EMAC_CAF_ENABLE, EMAC_NBC_DISABLE);

    // Setup EMAC buffers and interrupts
    AIC_ConfigureIT(AT91C_ID_EMAC, AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL, ISR_Emac);
    AIC_EnableIT(AT91C_ID_EMAC);

    // Init DM9161 driver
    DM9161_Init(pDm, EMAC_PHY_ADDR);

    // PHY initialize
    if (!DM9161_InitPhy(pDm, BOARD_MCK,
                        emacRstPins, PIO_LISTSIZE(emacRstPins),
                        emacPins, PIO_LISTSIZE(emacPins))) {

        printf("P: PHY Initialize ERROR!\n\r");
        return PLATFORM_ERR;
    }

    // Auto Negotiate
    if (!DM9161_AutoNegotiate(pDm)) {

        printf("P: Auto Negotiate ERROR!\n\r");
        return PLATFORM_ERR;
    }

    while( DM9161_GetLinkSpeed(pDm, 1) == 0 ) {

        errCount++;
    }
    //printf("P: Link detected\n\r");

#ifdef BUILD_UIP
  // Initialize the eLua uIP layer
  //NOTE: ale toto bola trochu ina MacAddress. islo o strukturu
  elua_uip_init( MacAddress );
#elif defined BUILD_LWIP
  //TODO: elua_lwip_int(...) alebo nieco ine
  elua_lwip_init( MacAddress );
#endif
  return PLATFORM_OK;
}

s8 platform_eth_send_packet( const void* src, u32 size )
{
  u8 emac_rc;

  //TODO LDuck: prepisar EMAC_Send, lebo nikdy nebudem pouzivat tento callback, a taktiez tam o ifdefovat statistiky
  emac_rc = EMAC_Send( ( void* )src, size, ( EMAC_TxCallback )0 );
  if( emac_rc == EMAC_TX_OK ) {
    return ERR_OK;
  } else {
    //TRACE_ERROR("E: Send, rc 0x%x\n\r", emac_rc);
    printf("E: Send, rc 0x%x\n\r", emac_rc);
    return ERR_BUF;
  }
}

u32 platform_eth_get_packet_nb( void* buf, u32 maxlen )
{
  // TODO LDuck: check, if maxlen is number smaller than "maxsizeof(unsigned int))",
  // because input parameter maxbuflen to EMAC_Poll (in that function it is called "frameSize")
  // is type "unsigned int" and not "u32" what is "unsigned long"
  u8 emac_ret;
  u32 pkt_len = 0;

  emac_ret = EMAC_Poll( ( unsigned char* )buf, maxlen, ( unsigned int * )&pkt_len );

  if( emac_ret == EMAC_RX_OK)
  {
    return pkt_len;
  }
  else if( emac_ret == EMAC_RX_FRAME_SIZE_TOO_SMALL )
  {
    //TODO: check what eLua do, with that frame. end of that frame is lost for ever
    fprintf( stderr, "!!! WARNING; part received packet is lost. code: L01 !!!!\n" );
    fflush( stderr );
    return 0;
  }
  else
    return 0;
}

void platform_eth_force_interrupt()
{
#ifdef BUILD_UIP
  elua_uip_mainloop();
#elif defined BUILD_LWIP
  emacif_poll();
#endif
}


#ifdef BUILD_UIP
//------------------------------------------------------------------------------
/// Get the elapsed time (in ms) since the last invocation of the uIP main loop
/// (elua_uip_mainloop, from which this function is called).
/// \return 0 if the uIP loop was called because of Ethernet activity, not
///     because a timer expired. Otherwise the Ethernet timer period in ms
///     (which indicates timer activity)
//------------------------------------------------------------------------------
u32 platform_eth_get_elapsed_time()
{
  if( eth_timer_fired )
  {
    eth_timer_fired = 0;
    return SYSTIMER_US_PER_INTERRUPT / 1000;
  }
  else
    return 0;
}
#endif // BUILD_UIP

#endif // #ifdef BUILD_ETHERNET

// ****************************************************************************
// MMC SD Card, SPI basic
#ifdef BUILD_MMCFS


#define PPIO_BASE_SPI         AT91C_BASE_PIOA
#define PSPI_BASE             AT91C_BASE_SPI0
#define PPIO_BASE_CS          AT91C_BASE_PIOA //MMCFS_CS_PORT
#define CARD_SELECT_PIN       AT91C_PA13_SPI0_NPCS1     //AT91C_PA11_NPCS0
#define SPI_SCBR_MIN          2 /* 1 failed with my cards */
#define SPI_CSR_NUM           0
#define PMC_ID_CS             AT91C_ID_PIOA


/* general AT91 SPI send/receive */
static inline BYTE AT91_spi_write_read( BYTE outgoing )
{
        BYTE incoming;

        while( !( PSPI_BASE->SPI_SR & AT91C_SPI_TDRE ) ); // transfer complete wait
        PSPI_BASE->SPI_TDR = (WORD)( outgoing );
        while( !( PSPI_BASE->SPI_SR & AT91C_SPI_RDRF ) ); // wait for char
        incoming = (BYTE)( PSPI_BASE->SPI_RDR );

        return incoming;
}


spi_data_type platform_spi_send_recv( unsigned id, spi_data_type data )
{
  return AT91_spi_write_read( data );
}

//void platform_spi_select( unsigned id, int is_select );


static void AT91_spiSetSpeed(BYTE speed)
{
        DWORD reg;

        if ( speed < SPI_SCBR_MIN ) speed = SPI_SCBR_MIN;
        if ( speed > 1 ) speed &= 0xFE;

        reg = PSPI_BASE->SPI_CSR[SPI_CSR_NUM];
        reg = ( reg & ~(AT91C_SPI_SCBR) ) | ( (DWORD)speed << 8 );
        PSPI_BASE->SPI_CSR[SPI_CSR_NUM] = reg;
}

u32 platform_spi_setup( unsigned id, int mode, u32 clock, unsigned cpol, unsigned cpha, unsigned databits )
{

  // set SPI pins to the peripheral function
  PPIO_BASE_SPI->PIO_ASR = AT91C_PA16_SPI0_MISO | AT91C_PA17_SPI0_MOSI | AT91C_PA18_SPI0_SPCK;
  // disable PIO from controlling the pins (so they are used for peripheral)
  PPIO_BASE_SPI->PIO_PDR = AT91C_PA16_SPI0_MISO | AT91C_PA17_SPI0_MOSI | AT91C_PA18_SPI0_SPCK;

  /* set chip-select as output high (unselect card) */
  /* manual control for CS !                        */
  AT91C_BASE_PMC->PMC_PCER = ( 1 << PMC_ID_CS ); // enable needed PIO in PMC
  PPIO_BASE_CS->PIO_PER  = CARD_SELECT_PIN;  // enable GPIO of CS-pin (disable peripheral functions)
  PPIO_BASE_CS->PIO_SODR = CARD_SELECT_PIN;  // set high
  PPIO_BASE_CS->PIO_OER  = CARD_SELECT_PIN;  // output enable

  // enable peripheral clock for SPI ( PID Bit 5 )
  AT91C_BASE_PMC->PMC_PCER = ( 1 << AT91C_ID_SPI0 ); // n.b. IDs are just bit-numbers

  // SPI disable
  PSPI_BASE->SPI_CR = AT91C_SPI_SPIDIS;

#if USE_DMA
  // init the SPI's PDC-controller:
  // disable PDC TX and RX
  PSPI_BASE->SPI_PTCR = AT91C_PDC_TXTDIS | AT91C_PDC_RXTDIS;
  // init counters and buffer-pointers to 0
  // "next" TX
  PSPI_BASE->SPI_TNPR = 0;
  PSPI_BASE->SPI_TNCR = 0;
  // "next" RX
  PSPI_BASE->SPI_RNPR = 0;
  PSPI_BASE->SPI_RNCR = 0;
  // TX
  PSPI_BASE->SPI_TPR = 0;
  PSPI_BASE->SPI_TCR = 0;
  // RX
  PSPI_BASE->SPI_RPR = 0;
  PSPI_BASE->SPI_RCR = 0;
#endif /* USE_DMA */

  // SPI enable and reset
  // "It seems that the state machine for revB version needs to have 2 SPI
  // software reset to properly reset the state machine."
  PSPI_BASE->SPI_CR = AT91C_SPI_SWRST;
  PSPI_BASE->SPI_CR = AT91C_SPI_SWRST;
  PSPI_BASE->SPI_CR = AT91C_SPI_SPIEN;

  // SPI mode: master, FDIV=0, fault detection disabled
  PSPI_BASE->SPI_MR  = AT91C_SPI_MSTR | AT91C_SPI_MODFDIS;

  // set chip-select-register
  // 8 bits per transfer, CPOL=1, ClockPhase=0, DLYBCT = 0
  PSPI_BASE->SPI_CSR[SPI_CSR_NUM] = AT91C_SPI_CPOL | AT91C_SPI_BITS_8;

  // slow during init
  AT91_spiSetSpeed(0xFE);

  // enable SPI
  PSPI_BASE->SPI_CR = AT91C_SPI_SPIEN;

}





#endif /* ifdef BUILD_MMCFS */


