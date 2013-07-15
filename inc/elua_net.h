// Network services provided by eLua

#ifndef __ELUA_NET_H__
#define __ELUA_NET_H__

#include "type.h"
#include "lauxlib.h"
#include "platform.h"

#include "matrixssl/matrixsslApi.h"

// eLua network typedefs
typedef s16 elua_net_size;

// eLua net error codes
enum
{
  ELUA_NET_ERR_OK = 0,
  ELUA_NET_ERR_TIMEDOUT,
  ELUA_NET_ERR_CLOSED,
  ELUA_NET_ERR_ABORTED,
  ELUA_NET_ERR_OVERFLOW
};

// eLua IP address type
typedef union
{
  u32     ipaddr;
  u8      ipbytes[ 4 ];
  u16     ipwords[ 2 ];
} elua_net_ip;

// eLua services ports
#define ELUA_NET_TELNET_PORT          23

// Different constants
#define ELUA_NET_SOCK_STREAM          0
#define ELUA_NET_SOCK_DGRAM           1

// 'no lastchar' for read to char (recv)
#define ELUA_NET_NO_LASTCHAR          ( -1 )

// eLua TCP/IP functions
int elua_net_socket( int type );
int elua_net_close( int s );
elua_net_size elua_net_recvbuf( int s, luaL_Buffer *buf, elua_net_size maxsize, s16 readto, unsigned timer_id, timer_data_type to_us );
elua_net_size elua_net_recv( int s, void *buf, elua_net_size maxsize, s16 readto, unsigned timer_id, timer_data_type to_us );
elua_net_size elua_net_send( int s, const void* buf, elua_net_size len );
int elua_accept( u16 port, unsigned timer_id, timer_data_type to_us, elua_net_ip* pfrom );
int elua_net_connect( int s, elua_net_ip addr, u16 port );
elua_net_ip elua_net_lookup( const char* hostname );

int elua_net_get_last_err( int s );
int elua_net_get_telnet_socket();


//#ifdef BUILD_DAEMON_SUPPORT_FOR_UIP
struct elua_uip_state;

struct ssl_state {
  u16 sock;
  unsigned char *buf;
  u32 buf_len;
  ssl_t *ssl;
  s32 matrix_state;
};

//typedef void ( *daemon_callback_fn )( volatile struct elua_uip_state *pstate );
//typedef void ( *daemon_callback_fn )( ssl_t *ssl );
typedef void ( *daemon_callback_fn )( struct ssl_state **ssl_state );
// eLua Daemon callbacks
struct daemon
{
  s16 port;
  daemon_callback_fn connected;
  daemon_callback_fn recv;
  daemon_callback_fn sent;
  daemon_callback_fn poll;
  elua_net_ip remote_client_ip;
};

struct daemon *elua_get_daemon_struct( u16 port );
//#endif // BUILD_DAEMON_SUPPORT_FOR_UIP


#endif
