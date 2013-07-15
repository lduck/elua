// UIP "helper" for eLua
// Implements the eLua specific UIP application

#ifndef __ELUA_UIP_H__
#define __ELUA_UIP_H__

#include "type.h"
#include "elua_net.h"

#include "platform_conf.h"
#include "matrixssl/matrixsslApi.h"

// eLua UIP application states
enum
{
  ELUA_UIP_STATE_IDLE = 0,
  ELUA_UIP_STATE_SEND,
  ELUA_UIP_STATE_RECV,
  ELUA_UIP_STATE_RECV_2,
  ELUA_UIP_STATE_CONNECT,
  ELUA_UIP_STATE_CLOSE
};

//#ifdef BUILD_DAEMON_SUPPORT_FOR_UIP
//struct elua_uip_state;
//typedef void ( *daemon_callback_fn )( volatile struct elua_uip_state *pstate );
//// eLua Daemon callbacks
//struct daemon
//{
////  elua_net_size port;
//  u16 port;
//  daemon_callback_fn connected;
//  daemon_callback_fn recv;
//  daemon_callback_fn sent;
//  daemon_callback_fn poll;
//  elua_net_ip remote_client_ip;
//};

//#include "uipopt.h"
//u16 daemon_ports[UIP_CONNS];
//#endif // BUILD_DAEMON_SUPPORT_FOR_UIP


// eLua UIP state
struct elua_uip_state
{
  u8                state, res;
  char*             ptr; 
  elua_net_size     len;
  s16               readto;
//#ifdef BUILD_SSL
  //ssl_t             *ssl;
  struct ssl_state *ssl_state;
//#endif
//#ifdef BUILD_DAEMON_SUPPORT_FOR_UIP
//  struct daemon_callbacks *dcall;
//#endif
};

struct uip_eth_addr;

// Helper functions
void elua_uip_appcall();
void elua_uip_udp_appcall();
void elua_uip_init( const struct uip_eth_addr* paddr );
void elua_uip_mainloop();

#endif
