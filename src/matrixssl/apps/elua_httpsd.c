/*
 * elua_httpsd.c
 *
 *  Created on: Apr 25, 2012
 *      Author: lduck
 */

#include "platform.h"
#include "trace.h"
#include "elua_net.h"
//#include "elua_uip.h"

#include "app.h"

#define USE_HEADER_KEYS

#ifdef USE_HEADER_KEYS
#include "sampleCerts/certSrv.h"
#include "sampleCerts/privkeySrv.h"
#else
static char certSrvFile[] = "certSrv.pem";
static char privkeySrvFile[] = "privkeySrv.pem";
#endif /* USE_HEADER_KEYS */

static sslKeys_t *keys;

/* certCb - The function callback that will be invoked
 * during a client-authentication SSL handshake
 * to see the internal authentication status
 * of the certificate chain and perform custom validations as needed.
 */
#define certCb NULL
#define HTTPSD_PORT 443


static s32 init_matrixSsl(void ) {
    //FIXME v tejto funkcii prerobit rc na "hs->ssl_state"
    s32 rc;
    keys = NULL;

    if ((rc = matrixSslOpen()) < 0) {
        _psTrace("MatrixSSL library init failure.  Exiting\n");
        return rc;
    }
    if (matrixSslNewKeys(&keys) < 0) {
        _psTrace("MatrixSSL library key init failure.  Exiting\n");
        return -1;
    }

#ifdef USE_HEADER_KEYS
/*
    In-memory based keys
*/
    if ((rc = matrixSslLoadRsaKeysMem(keys, certSrvBuf, sizeof(certSrvBuf),
                   privkeySrvBuf, sizeof(privkeySrvBuf), NULL, 0)) < 0) {
        _psTrace("No certificate material loaded.  Exiting\n");
        matrixSslDeleteKeys(keys);
        matrixSslClose();
        return rc;
    }
#else /* USE_HEADER_KEYS */
/*
    File based keys
*/
    if ((rc = matrixSslLoadRsaKeys(keys, certSrvFile, privkeySrvFile, NULL,
            NULL)) < 0) {
        _psTrace("No certificate material loaded.  Exiting\n");
        matrixSslDeleteKeys(keys);
        matrixSslClose();
        return rc;
    }
#endif /* USE_HEADER_KEYS */

    return rc;
}


void http_close_conn(u16 sock, s16 reason ) {
  TRACE_INFO("closing connection\r\n");
  elua_net_close( sock );
  return;
}

enum
{
  CONNECTION_CLOSED = 0, // can finish
  READ,                 // call recv_s
  WRITE,                // call send_s
  PROCESS               // call process_s
};

int send_s(struct ssl_state *ssl_state)// ssl_t *ssl_state->ssl)// ssl_state ssl_state->matrix_state )
{
  s32 send_len;
  //v matrixssl priklade bolo pred dalsimi riadkami WRITE_MORE:
  printf("02 (recv_len = matrixSslGetOutdata(cp->ssl_state->ssl, &buf)) > 0\n");
  if( ( send_len = matrixSslGetOutdata( ssl_state->ssl, &ssl_state->buf ) ) > 0) {
    elua_net_send( ssl_state->sock, ssl_state->buf, ssl_state->buf_len );
  //    can_sent = tcp_sndbuf(pcb);
  //    printf("buf_len: %d, can_sent: %d\n", buf_len, can_sent);
  //    if (can_sent > buf_len) {
  //      //mozem poslat vsetko
  //      err = tcp_write(pcb, hs->buf, hs->buf_len, 0);
  //    } else {
  //      err = tcp_write(pcb, hs->buf, can_sent, 0);
  //      // ostatne poslem neskor
  //    }
    if(elua_net_get_last_err( ssl_state->sock ) != ELUA_NET_ERR_OK) {
      http_close_conn(ssl_state->sock, PS_PLATFORM_FAIL);
      return CONNECTION_CLOSED;
    } else {
      /* Indicate that we've written > 0 bytes of data */
      printf("03 ((rc = matrixSslSentData(ssl_state->ssl, transferred)) < 0)\n");
      if( ( ssl_state->matrix_state = matrixSslSentData( ssl_state->ssl, send_len ) ) < 0 ) {
        printf("MAL BY SOM SKONCIT\n");
        http_close_conn(ssl_state->sock, PS_ARG_FAIL);
        return CONNECTION_CLOSED; //continue;   /* Next connection */
      }
      if( ssl_state->matrix_state == MATRIXSSL_REQUEST_CLOSE ) {
        printf("MATRIXSSL_REQUEST_CLOSE\n");
        http_close_conn(ssl_state->sock, MATRIXSSL_REQUEST_CLOSE);
        return CONNECTION_CLOSED; //continue;   /* Next connection */
      }
      else if( ssl_state->matrix_state == MATRIXSSL_HANDSHAKE_COMPLETE) {
        printf("MATRIXSSL_HANDSHAKE_COMPLETE\n");
        /* If the protocol is server initiated, send data here */
      }
      /* Update activity time */
      //FIXME:psGetTime(&cp->time);
    }
  } else if( ssl_state->buf_len < 0 ) {
      printf("buf_len: %d, \n", ssl_state->buf_len);
      http_close_conn(ssl_state->sock, PS_ARG_FAIL);
      return CONNECTION_CLOSED; //continue;   /* Next connection */
  }
  return READ;
}

int recv_s( struct ssl_state *ssl_state )
{
  //elua_net_size buf_len;
  //s16 lastchar = ELUA_NET_NO_LASTCHAR;
  //unsigned timer_id = PLATFORM_TIMER_SYS_ID;
  //timer_data_type timeout = PLATFORM_TIMER_INF_TIMEOUT;
  //char buffer[LUAL_BUFFERSIZE];
  //char *buf;
  //u8 continue_recv = 1;

  if( ( ssl_state->buf_len = matrixSslGetReadbuf(ssl_state->ssl, &ssl_state->buf ) ) <= 0 ) {
      TRACE_INFO("did not get ssl buffer\r\n");
      http_close_conn( ssl_state->sock, PS_ARG_FAIL );
      return CONNECTION_CLOSED;
  }

  u16 recv_len = elua_net_recv( ssl_state->sock, ssl_state->buf,
                                ssl_state->buf_len, ELUA_NET_NO_LASTCHAR,
                                PLATFORM_TIMER_SYS_ID, PLATFORM_TIMER_INF_TIMEOUT
                              );
  //TRACE_INFO("buffer: %s\n", net_recv_buff.buffer );
  //TRACE_INFO("len: %d\n", len );

  if(elua_net_get_last_err( ssl_state->sock ) != ELUA_NET_ERR_OK) {
    http_close_conn( ssl_state->sock, PS_PLATFORM_FAIL );
    return CONNECTION_CLOSED;
  }
  if( recv_len < 0 ) {
    TRACE_ERROR("elua_net_recv() < 0.");
    TRACE_ERROR("This should never happen according documentation of net.recv()\r\n");
  } else if( recv_len == 0 ) {
    /* If EOF, remote socket closed. This is semi-normal closure.
       Officially, we should close on closure alert. */
    TRACE_INFO("Closing connection %d on EOF\r\n", ssl_state->sock );
    http_close_conn( ssl_state->sock, 0 );
    return CONNECTION_CLOSED;
  }

  /* Notify SSL state machine that we've received more data into the
     ssl buffer retreived with matrixSslGetReadbuf. */
  TRACE_DEBUG("05 ((rc = matrixSslReceivedData(cp->ssl, (int32)transferred:%d, &buf, (uint32*)&len:%d)) < 0)\n", recv_len, ssl_state->buf_len);
  //TRACE_DEBUG("ssl_state->buf:%s", ssl_state->buf);
  ssl_state->matrix_state = matrixSslReceivedData( ssl_state->ssl, recv_len,
                                                   &ssl_state->buf,
                                                   &ssl_state->buf_len
                                                 );
  if ( ssl_state->matrix_state < 0 ) {
    TRACE_INFO("MAL BY SOM SKONCIT, buf_len: %d, ssl_state: %d\r\n", ssl_state->buf_len, ssl_state->matrix_state);
    http_close_conn( ssl_state->sock, 0 );
    return CONNECTION_CLOSED;
  }
  /* Update activity time */
  //FIXME: psGetTime(&cp->time);
  return PROCESS;
}

int process_s( struct ssl_state *ssl_state )
{
  switch( ssl_state->matrix_state ) {
    case MATRIXSSL_HANDSHAKE_COMPLETE:
      /* If the protocol is server initiated, send data here */
      TRACE_DEBUG("MATRIXSSL_HANDSHAKE_COMPLETE\n");
      return READ; //goto READ_MORE;

    case MATRIXSSL_APP_DATA:
      TRACE_DEBUG("MATRIXSSL_APP_DATA\n");
      //FIXME: zatial poslem to co som prijal. je to us odsifrovane
      TRACE_DEBUG("plaintex recieved data: %s\r\n", *ssl_state->buf);
      return WRITE;
      /* Remember, must handle if len == 0! */
      //TODO tu musim rozparsovat request a podla toho odpovedat
      // mozno sa chcem nechat inspirovat prikladom z lwIP pre POST
//      if ((rc = httpBasicParse(cp, buf, len)) < 0) {
//              _psTrace("Couldn't parse HTTP data.  Closing conn.\n");
//              closeConn(cp, PS_PROTOCOL_FAIL);
//              continue; /* Next connection */
//      }
//      if (rc == HTTPS_COMPLETE) {
//        if (httpWriteResponse(ssl) < 0) {
//          http_close_conn(sock);
//        }
//        /* For HTTP, we assume no pipelined requests, so we
//           close after parsing a single HTTP request */
//        /* Ignore return of closure alert, it's optional */
//        printf("matrixSslEncodeClosureAlert(cp->ssl)\n");
//        matrixSslEncodeClosureAlert( ssl_state->ssl );
//        printf("matrixSslProcessedData(cp->ssl, &buf, (uint32*)&len)\n");
//        ssl_state->matrix_state = matrixSslProcessedData(ssl_state->ssl, &ssl_state->buf, (u32 *)&ssl_state->buf_len);
//        if (ssl_state->matrix_state > 0) {
//            /* Additional data is available, but we ignore it */
//            //FIXME: tak toto asi chcem podporovat
//            _psTrace("HTTP data parsing not supported, ignoring.\n");
//            http_close_conn(sock);
//            //continue; /* Next connection */
//        } else if( ssl_state->matrix_state < 0) {
//            http_close_conn(sock);
//            //continue; /* Next connection */
//        }
//        /* rc == 0, write out our response and closure alert */
//        //goto WRITE_MORE;
//      }
//      /* We processed a partial HTTP message */
//      printf("(rc = matrixSslProcessedData(cp->ssl, &buf, (uint32*)&len)) == 0)\n");
//      if ((rc = matrixSslProcessedData(cp->ssl, &buf, (uint32*)&len)) == 0) {
//              goto READ_MORE;
//      }
//      goto PROCESS_MORE;
//      break;

    case MATRIXSSL_REQUEST_SEND:
      TRACE_DEBUG("MATRIXSSL_REQUEST_SEND\n");
      /* teraz by som mal zapisat do siete. v matrix priklade este
       * mazu fileDescriptor na citanie, aby po skonceni zapisu nesli znova citat.
       *
       * aby sa to tu nestalo, tak vynimocne zavolam funkciu priamo a nebudem
       * vracat kod, pre zapis, lebo send_s() vrati CONECTION_CLOSED alebo READ,
       * ale kedze ja uz nechcem citat, tak vratim CONECTION_CLOSED, a to bez toho
       * aby som sa tu snazil zavriet spojenie. takze bud sa zavrie vo vnutry
       * send_s(), alebo s nim chcem este v buducnosti pracovat.
       */
      send_s( ssl_state ); //goto WRITE_MORE;
      return WRITE;//send_s( ssl_state ); //goto WRITE_MORE;

    case MATRIXSSL_REQUEST_RECV:
        TRACE_DEBUG("MATRIXSSL_REQUEST_RECV\n");
        return READ;

    case MATRIXSSL_RECEIVED_ALERT:
        TRACE_DEBUG("MATRIXSSL_RECEIVED_ALERT\n");
        /* The first byte of the buffer is the level */
        /* The second byte is the description */
        if( *ssl_state->buf == SSL_ALERT_LEVEL_FATAL ) {
          TRACE_INFO("Fatal alert: %d, closing connection.\n", *(ssl_state->buf + 1) );
          http_close_conn(ssl_state->sock, PS_PROTOCOL_FAIL);
          return CONNECTION_CLOSED; //continue; /* Next connection */
        }
        /* Closure alert is normal (and best) way to close */
        if( *(ssl_state->buf + 1) == SSL_ALERT_CLOSE_NOTIFY ) {
                http_close_conn(ssl_state->sock, PS_SUCCESS);
                return CONNECTION_CLOSED; //continue; /* Next connection */
        }
        TRACE_INFO("Warning alert: %d\n", *(ssl_state->buf + 1));

        printf("((rc = matrixSslProcessedData(cp->ssl, &buf, (uint32*)&len)) == 0)\n");
        ssl_state->matrix_state = matrixSslProcessedData( ssl_state->ssl,
                                                          &ssl_state->buf,
                                                          (u32*)&ssl_state->buf_len
                                                        );
        if ( ssl_state->matrix_state == 0 ) {
          /* No more data in buffer. Might as well read for more. */
          return READ; //goto READ_MORE;
        }
        return PROCESS; //goto PROCESS_MORE;

    default:
        printf("default\n");
        /* If rc <= 0 we fall here */
        http_close_conn(ssl_state->sock, PS_PROTOCOL_FAIL);
        return CONNECTION_CLOSED; /* Next connection */

  } //end of switch
}

//TODO 1. make return codes on failure and handle them in elua_uip_appcall() in elua_uip.c
//TODO 2. zabezpecit, aby pri obdrzani dat z nezatvoreneho spojenia sa tieto data dostali
// precitali funkciou recv_s s prislusnym ssl_state parametrom
void elua_httpsd_accept(struct ssl_state *ssl_state)
{
//  *ssl_state = (struct ssl_state *)malloc( sizeof( struct ssl_state ) );
//  if( ssl_state == NULL )  {
//      TRACE_WARNING("Not enough memory for allocation ssl_state\r\n");
//  }
//  (*ssl_state)->ssl = (ssl_t *)malloc( sizeof ( ssl_t ) );
//  if( (*ssl_state)->ssl == NULL )  {
//      TRACE_WARNING("Not enough memory for allocation ssl_state->ssl_t\r\n");
//  }

  matrixSslNewServerSession( &ssl_state->ssl, keys, certCb);

  int ret = READ;
  while( ret != CONNECTION_CLOSED ) {
    switch( ret ) {
      case READ:
        ret = recv_s( ssl_state );
        break;
      case PROCESS:
        ret = process_s( ssl_state );
        break;
      case WRITE:
        ret = send_s( ssl_state );
        break;
    }
  }
  TRACE_WARNING("odchadzam z elua_httpsd_accept()");

  // bud som zavrel spojenie vo vnutry, alebo som ho nechal otvorene
  // a budem s nim nieco este robit
  return;
}

struct ssl_state *ssl_state_alloc()
{
  struct ssl_state *ssl_state;
  ssl_state = (struct ssl_state *)malloc( sizeof( struct ssl_state ) );
  if( ssl_state == NULL )  {
      printf("Not enough memory for allocation ssl_state\r\n");
      return ssl_state;
  }
  ssl_state->ssl = (ssl_t *)malloc( sizeof ( ssl_t ) );
  if( ssl_state->ssl == NULL )  {
      printf("Not enough memory for allocation ssl_state->ssl_t\r\n");
  }
  return ssl_state;
}

void elua_httpsd_init()
{
  struct ssl_state *ssl_state;
  ssl_state = ssl_state_alloc();
  // find first free struct for this daemon
  struct daemon *d;
  d = (struct daemon *)elua_get_daemon_struct(0); // zero indicates, that the struct is not used
  if( d == NULL ) {
    TRACE_WARNING("https daemon can not be start. no free struct\r\n");
    return;
  }
  d->port = HTTPSD_PORT;
  d->connected = elua_httpsd_accept;

  //TODO odtestovat navratovy kod, alebo ho posielat dalej a vysporiadat sa s nim tam
  init_matrixSsl();
  matrixSslNewServerSession( &ssl_state->ssl, keys, certCb);

  elua_net_ip remip;

  while(1) {
      ssl_state->sock = elua_accept( 443, PLATFORM_TIMER_SYS_ID, PLATFORM_TIMER_INF_TIMEOUT, &remip );
      elua_httpsd_accept(ssl_state);
  }

//  int old_status;
//  old_status = platform_cpu_set_global_interrupts( PLATFORM_CPU_DISABLE );
//  uip_unlisten( htons( HTTPSD_PORT ) );
//  uip_listen( htons( HTTPSD_PORT ) );
//  platform_cpu_set_global_interrupts( old_status );
  elua_daemon_accept(d);
  TRACE_INFO("https daemon started\r\n");
  return;

//  timer_data_type tmrstart = 0;
//  int old_status;
//
////    if( !elua_uip_configured )
////      return -1;
//  old_status = platform_cpu_set_global_interrupts( PLATFORM_CPU_DISABLE );
//  uip_unlisten( htons( HTTPSD_PORT ) );
//  uip_listen( htons( HTTPSD_PORT ) );
//  platform_cpu_set_global_interrupts( old_status );
//  //elua_uip_accept_sock = -1;
//  //elua_uip_accept_request = 1;

}



void elua_httpsd_run()
{
  u16 port = 80;
  unsigned timer_id = PLATFORM_TIMER_SYS_ID;
  timer_data_type timeout = PLATFORM_TIMER_INF_TIMEOUT;
  elua_net_ip remip;

  init_matrixSsl();

  int sock;

  sock = elua_accept( port, timer_id, timeout, &remip );
  //err = elua_net_get_last_err( sock );
  TRACE_INFO("Got connection on socket: %d\n", sock);

  TRACE_INFO("Remote ip: %d.%d.%d.%d", ( int )remip.ipbytes[ 0 ], ( int )remip.ipbytes[ 1 ],
                       ( int )remip.ipbytes[ 2 ], ( int )remip.ipbytes[ 3 ] );

  //u32 ssl_state;
  ssl_t *ssl;
  matrixSslNewServerSession(&ssl, keys, certCb);

  elua_net_size maxsize = 1024 ;
  s16 lastchar = ELUA_NET_NO_LASTCHAR;
  timer_id = PLATFORM_TIMER_SYS_ID;
  timeout = PLATFORM_TIMER_INF_TIMEOUT;
  char buffer[LUAL_BUFFERSIZE];

  //iluaL_buffinit( L, &net_recv_buff );
  //u16 len = elua_net_recv( sock, &net_recv_buff.buffer, maxsize, lastchar, timer_id, timeout );
  u16 len = elua_net_recv( sock, &buffer, maxsize, lastchar, timer_id, timeout );
  //TRACE_INFO("buffer: %s\n", net_recv_buff.buffer );
  //TRACE_INFO("len: %d\n", len );
    //return 1;
  TRACE_INFO("buffer: %s\n", buffer );
    TRACE_INFO("len: %d, LUAL_BUFFERSIZE: %d\n", len, LUAL_BUFFERSIZE );
    elua_net_send( sock, buffer, len );
    return;
}

