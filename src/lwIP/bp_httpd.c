/* z noveho lwIP */

#include "lwip/err.h"
//#include "lwip/debug.h"
#include "lwip/opt.h"

#include "lwip/stats.h"
#include "lwip/tcp.h"
//#include "fs.h"

#include "bp_httpd.h"

#include <string.h>

/** The poll delay is X*500ms */
#ifndef HTTPD_POLL_INTERVAL
#define HTTPD_POLL_INTERVAL                 4
#endif


/************************************************************************/
/******** kod pre matrixSSSL ********************************************/
/************************************************************************/
#include "apps/app.h"

#include "matrixssl/matrixsslApi.h"

#define USE_HEADER_KEYS

#ifdef USE_HEADER_KEYS
#include "sampleCerts/certSrv.h"
#include "sampleCerts/privkeySrv.h"
//#include "sampleCerts/RSA/1024_RSA.h"
//#include "sampleCerts/RSA/1024_RSA_KEY.h"
#else
static char certSrvFile[] = "/mmc/certSrv.pem";
static char privkeySrvFile[] = "/mmc/privkeySrv.pem";
#endif /* USE_HEADER_KEYS */

static unsigned char    g_httpResponseHdr[] = "HTTP/1.0 200 OK\r\n"
    "Server: PeerSec Networks MatrixSSL/" MATRIXSSL_VERSION "\r\n"
    "Pragma: no-cache\r\n"
    "Cache-Control: no-cache\r\n"
    "Content-type: text/plain\r\n"
    "Content-length: 9\r\n"
    "\r\n"
    "MatrixSSL";

//#include "uhttpd-mimetypes.h"
//
//static unsigned char    status_hdr[] = "HTTP/1.%d %d %s\r\n";
//static unsigned char    cont_type_hdr[] = "Content-type: %s\r\n";
//static unsigned char    cont_len_hdr[] = "Content-length: %d\r\n";
//static unsigned char    connect_hdr[] = "Connection: %s\r\n";





static sslKeys_t *keys;

/* certCb - The function callback that will be invoked
 * during a client-authentication SSL handshake 
 * to see the internal authentication status 
 * of the certificate chain and perform custom validations as needed.
 */
#define certCb NULL

static int process_s( struct tcp_pcb *pcb, struct http_state *hs);
static int send_s( struct tcp_pcb *pcb, struct http_state *hs);
static void http_close_conn(struct tcp_pcb *pcb, struct http_state *hs);

static u16_t init_matrixSsl(void ) {
    //FIXME v tejto funkcii prerobit rc na "hs->ssl_state"
    u16_t rc;
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
//    if ((rc = matrixSslLoadRsaKeysMem(keys, RSA1024, sizeof(RSA1024),
//       RSA1024KEY, sizeof(RSA1024KEY), NULL, 0)) < 0) {
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


/******************************************************************************/
/*
    Create an HTTP response and encode it to the SSL buffer
 */
#define TEST_SIZE   16000
static int32 httpWriteResponse(ssl_t *cp)
{
    unsigned char   *buf;
    uint32          available;
    
    if ((available = matrixSslGetWritebuf(cp, &buf, 
                            strlen((char *)g_httpResponseHdr) + 1)) < 0) {
        return PS_MEM_FAIL;
    }   
    strncpy((char *)buf, (char *)g_httpResponseHdr, available);
    if (matrixSslEncodeWritebuf(cp, strlen((char *)buf)) < 0) {
        return PS_MEM_FAIL;
    }   
    return MATRIXSSL_REQUEST_SEND;
}

static u8 write_response(struct tcp_pcb *pcb, struct http_state *hs) {
  //matrixSslEncodeToOutdata(hs->ssl, (unsigned char *) g_httpResponseHdr,  )
//  u32 copy_len;
//  if ((hs->buf_len = matrixSslGetWritebuf(hs->ssl, &hs->buf, hs->to_send_buf_len) ) < 0 ) {
//    //return PS_MEM_FAIL;
//    http_close_conn(pcb, hs);
//    return CONNECTION_CLOSED;
//  }
//  copy_len = LWIP_MIN(hs->buf_len, hs->to_send_buf_len);
//  memcpy(hs->buf, hs->to_send_buf, copy_len);
//
//  int32 can_copy = hs->to_send_buf_len - hs->to_send_buf_copied_len;
//  if(can_copy >= 2) {
//    memcpy(hs->to_send_buf + hs->to_send_buf_copied_len, "\r\n", 2);
//    hs->to_send_buf_copied_len += 2;
//  }

//  if(hs->http_req->file != NULL) {
//    // 1, write headers
//    // 2,
//    if(hs->server_state == WRITE_HEADERS ) {
//      if(hs->to_send_buf==NULL) {
//        if ((hs->to_send_buf_len = matrixSslGetWritebuf(hs->ssl, &hs->to_send_buf, hs->http_req->file_size) ) < 0 ) {
//          //return PS_MEM_FAIL;
//          while(1);//FIXME chcem vevidet kedy a ci sa stane pri testovani
//          return CLOSE_CONNECTION;
//        }
//      }
//
//    }
//
//    if ((hs->to_send_buf_len = matrixSslGetWritebuf(hs->ssl, &hs->to_send_buf, hs->http_req->file_size) ) < 0 ) {
//      //return PS_MEM_FAIL;
//      while(1);//FIXME chcem vevidet kedy a ci sa stane pri testovani
//      return CLOSE_CONNECTION;
//    }
//    u32 can_read = LWIP_MIN(hs->to_send_buf_len, hs->http_req->file_size);
//    hs->to_send_buf_used_len = fread(hs->to_send_buf, 1, can_read, hs->http_req->file);
//    if(hs->to_send_buf_used_len < can_read ) {
//      //EOF or error
//      hs->http_req->file_size = 0;
//      while(1);//FIXME chcem vevidet kedy a ci sa stane pri testovani
//    } else {
//      hs->http_req->file_size -= hs->to_send_buf_used_len;
//    }
//  }




  /* for static and wsapi page: */
  if (matrixSslEncodeWritebuf(hs->ssl, hs->to_send_buf_used_len ) < 0 ) {
    //return PS_MEM_FAIL;
    return CLOSE_CONNECTION;
//    http_close_conn(pcb, hs);
//    return CONNECTION_CLOSED;
  }
  //hs->encoded = hs->buf_len;

  return MATRIXSSL_REQUEST_SEND;
}

/************************************************************************/
/************************************************************************/
/************************************************************************/



/** Allocate a struct http_state. */
static struct http_state* http_state_alloc(void) {
    struct http_state *ret;
    ret = (struct http_state *)mem_malloc(sizeof(struct http_state));

    if (ret != NULL) {
        /* Initialize the structure. */
        memset(ret, 0, sizeof(struct http_state));
    }

    ret->http_req = (struct http_request *)mem_malloc(sizeof(struct http_request));
    if (ret->http_req != NULL) {
        memset(ret->http_req, 0, sizeof(struct http_request));
    } else {
      free(ret);
      return NULL;
    }

    ret->http_resp = (struct http_response *)mem_malloc(sizeof(struct http_response));
    if (ret->http_resp != NULL) {
        memset(ret->http_resp, 0, sizeof(struct http_response));
    } else {
      free(ret->http_req);
      free(ret);
      return NULL;
    }

    return ret;
}

/* Clear request related values */
static void http_clear_conn(struct http_state *hs ) {
  hs->http_req->method = HTTP_METHOD_UNKNOW; //0 /* not necessary */
  hs->http_req->content_left = 0;  /* not necessary */
  hs->server_state = FISRT_LINE; //0 /* not necessary */

  if(hs->http_resp->file != NULL) {
    fclose(hs->http_resp->file);
    hs->http_resp->file = NULL;
  }
  hs->http_resp->file_size = 0; /* not necessary */

  if (hs->L != NULL) {
    lua_close(hs->L);
    printf("LUA closed\n");
    hs->L = NULL;
  }
  if (hs->req != NULL) {
    while(1); //todo ked sa tu nebudem zasekavat tak mozeme cely if zmazat
    pbuf_free(hs->req);
    hs->req = NULL;
  }
  if( hs->http_req->to_parse_buf != NULL) {
    mem_free(hs->http_req->to_parse_buf);
    hs->http_req->to_parse_buf = NULL;
  }

  hs->http_req->to_parse_buf_len = 0; //
  if( hs->stack != NULL) {
    mem_free(hs->stack);
    hs->stack = NULL;
  }
  hs->stack_size = 0;

  hs->to_send_buf = NULL; /* malloc by matrixSSL, but i test for NULL */
  hs->to_send_buf_used_len = 0; /* probably not necessary */
  hs->to_send_buf_len = 0; /* probably not necessary */

}

/** Free a struct http_state.
 *  * Also frees the file data if dynamic.
 *   */
static void http_state_free(struct http_state *hs) {
  if (hs->L != NULL) {
    lua_close(hs->L);
    printf("LUA closed\n");
    hs->L = NULL;
  }
//  if (hs->to_parse_buf != NULL) {
//    // tak tu to pada psAssert(hs->to_parse_buf_len > 0);
//    mem_free(hs->to_parse_buf);
//    hs->to_parse_buf_len = 0;
//  }
  /* Free matrixSsl data*/
  matrixSslDeleteSession(hs->ssl);
  hs->ssl = NULL;

  //TODO: free: hs->stack, hs->to_parse_buf

  if(hs->to_free != NULL){
      mem_free(hs->to_free);
  }
  else {
      printf("nothing to be freed, probably I served static page\n\r");
  }
  mem_free(hs);
  hs = NULL;
}



/**
 * The connection shall be actively closed.
 * Reset the sent- and recv-callbacks.
 *
 * @param pcb the tcp pcb to reset callbacks
 * @param hs connection state to free
 * @param linger: - 1 when closing because all data is sent (let TCP transmit
 *                  unacked data)
 *                - 0 when closing because of an error (RST is sent when
 *                  unsent data is present)
 */
static void http_close_conn(struct tcp_pcb *pcb, struct http_state *hs) {
    err_t err;
    u32 can_sent;
    printf("Closing connection %x\n\r", (unsigned int ) pcb);

    if (hs->req != NULL) {
      while(1); //todo ked sa tu nebudem zasekavat tak mozeme cely if zmazat
      pbuf_free(hs->req);
      hs->req = NULL;
    }


    /* nepovinny oznam klienktovi ze koncim ssl spojenie */
    /* Quick attempt to send a closure alert, don't worry about failure */
    if (matrixSslEncodeClosureAlert(hs->ssl) >= 0) {
      if ((hs->buf_len = matrixSslGetOutdata(hs->ssl, &hs->buf)) > 0) {
         /* if ((len = send(cp->fd, hs->buf, len, MSG_DONTWAIT)) > 0) {
          *   matrixSslSentData(hs->ssl, len); */
        can_sent = tcp_sndbuf(pcb);
        printf("buf_len: %lu, can_sent: %hu\n", hs->buf_len, can_sent);
        if (can_sent > hs->buf_len) {
          //mozem poslat vsetko
          err = tcp_write(pcb, hs->buf, hs->buf_len, 0);
          //tcp_output(pcb);
        } else {
          err = tcp_write(pcb, hs->buf, can_sent, 0);
          //tcp_output(pcb);
        }
      }
    }

    /* send everythong that we can */
    tcp_output(pcb);

    tcp_arg(pcb, NULL);
    tcp_recv(pcb, NULL);
    tcp_err(pcb, NULL);
    tcp_poll(pcb, NULL, 0);
    tcp_sent(pcb, NULL);

    http_state_free(hs);
    tcp_arg(pcb, NULL);//hs);
    
    
    err = tcp_close(pcb);
    if (err != ERR_OK) {
        printf("Error %d closing %p\n", err, (void*)pcb);
        /* error closing, try again later in poll */
        /*FIXME  
         * povodne tu bulo v pripade neukoncenia spojenia znovu nastavenie tcp_poll(pcb, http_poll, HTTPD_POLL_INTERVAL);
         * treba sa pozriet do lwip vzoru ako to v http_poll funciii potom osetruju*/
    }
}

/** Defines the maximum length of a HTTP request line (up to the first CRLF,
 *     copied from pbuf into this a global buffer when pbuf- or packet-queues
 *         are received - otherwise the input pbuf is used directly) */
#ifndef LWIP_HTTPD_MAX_REQ_LENGTH
#define LWIP_HTTPD_MAX_REQ_LENGTH           1023
#endif

/* Send string. It does not mater, how long it is. 
 * It should send it all, (?? if it fits into tcp window size
 * defined in lwipopts.h ??)*/
static u8_t httpd_send_static(struct tcp_pcb *pcb, struct http_state *hs, 
                              u16_t len) {
    err_t err;
    u16_t q_len;

    //printf("len(hs->file): %d\n\r", strlen(hs->file));
    //printf("q_len: %d\n\r", q_len);
    
    if(hs->file != NULL) {
        q_len = tcp_sndbuf(pcb);
        while(len > q_len) {
            //printf("page > q_len\n\r");
            err = tcp_write(pcb, hs->file, q_len, 0);
            if(err == ERR_MEM){
                printf("5 ERR_MEM??\n\r");
                return 0;
                //TODO: should I return 0 ????
            } 
            else {
                printf("6 ERR_OK\n\r");
                //hs->file =  &webif_css[q_len];
                hs->file =  &hs->file[q_len];

                q_len = tcp_sndbuf(pcb);
                if(q_len <= 1 ){
                    //printf("q_len <= 1, : %d\n\r", q_len);
                    return 0;
                }
            }
        }

        q_len = tcp_sndbuf(pcb);
        if(len <= q_len) {
            printf("page %d< q_len %d\n\r", len, q_len );
            err = tcp_write(pcb, hs->file, len, 0);
            if(err == ERR_MEM){
                printf("7 ERR_MEM\n\r");
                http_close_conn(pcb, hs);
                return 1; // FIXME: this probably never happen
                // ale stalo sa uz, ked som si skopcil do stranky aj
                // byty z adries ku ktorim som uz nemal pristup ovat
            } 
            else {
                printf("8 ERR_OK\n\r");
                hs->file = NULL;
                http_close_conn(pcb, hs);
                return 1;
            }
        }
    } //else {
        printf("!!!!!!!!!!!!ZLE JE!!!!!!!!!\r\n");
        return 0;
    //}
}


//static u8_t 
//httpd_parse_request(struct tcp_pcb *pcb, struct http_state *hs) {
//    u16_t data_len;
//    char *data;
//    char httpd_req_buf[LWIP_HTTPD_MAX_REQ_LENGTH+1];
//
//    data_len = LWIP_MIN(hs->req->tot_len, LWIP_HTTPD_MAX_REQ_LENGTH);
//    pbuf_copy_partial(hs->req, httpd_req_buf, data_len, 0);
//    data = httpd_req_buf;
//
//    printf("dostal som: %s\n\r", data);
//    tcp_write(pcb, "blba odpoved", 12, 0);
//    http_close_conn(pcb, hs);
//    return 1;
//}

static err_t copy_to_matrinxssl_inbuf(struct tcp_pcb *pcb, struct http_state *hs ) {
  u16_t data_len;

  printf("04 ((len = matrixSslGetReadbuf(hs->ssl, &buf)) <= 0)\n");
  if ((hs->buf_len = matrixSslGetReadbuf(hs->ssl, &hs->buf)) <= 0) {
    //FIXME zavriet spojenie closeConn(cp, PS_ARG_FAIL);
    return ERR_MEM;
  }

  data_len = LWIP_MIN(hs->req->tot_len - hs->copied_data_len, hs->buf_len);

  pbuf_copy_partial(hs->req, hs->buf, data_len, hs->copied_data_len);
  hs->copied_data_len += data_len;

  printf("data_len: %d, hs->req->len: %d, hs->req->tot_len: %d\n", data_len, hs->req->len, hs->req->tot_len);

  printf("05 ((rc = matrixSslReceivedData(cp->ssl, (int32)transferred:%d, &buf, (uint32*)&len:%lu)) < 0)\n", data_len, hs->buf_len);
  //printf("data:%s", data);
  if ((hs->ssl_state = matrixSslReceivedData(hs->ssl, data_len,  &hs->buf,
                                  (uint32*) &hs->buf_len)) < 0) {
    printf("MAL BY SOM SKONCIT, buf_len: %lu, \n", hs->buf_len);
    return ERR_MEM;
      //FIXME closeConn(cp, 0);
      //continue;   /* Next connection */
  }
  printf("05 rc:%ld buf_len: %lu, hs->copied_data_len:%d \n", hs->ssl_state, hs->buf_len, hs->copied_data_len);
  return ERR_OK;
}

/* This is the callback function that is called
when a TCP segment has arrived in the connection. */
static err_t
httpd_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *lwip_pbuf, err_t err) {
    //unsigned char *begin, *end;
    struct http_state *hs = (struct http_state *)arg;
    //u32_t reqcount;
    //int i;
    //char httpd_req_buf[LWIP_HTTPD_MAX_REQ_LENGTH+1];
    //char *data, *hs_req;
    //char *data;
    u16_t data_len;//, can_sent;
    err_t parsed = 1;

    hs->retries = 0;

    printf("CB recv, hs:%x, pcb:%x, lwip_pbuf:%x, %x\n\r", (unsigned int) arg, (unsigned int) pcb, (unsigned int) lwip_pbuf, (unsigned int) err);
    if (err != ERR_OK) {
        return err;
    }

    /* If we got a NULL pbuf in lwip_pbuf, the remote end has closed
    the connection. */
    if(lwip_pbuf == NULL) {
        //TODO uvolnit hs?? a zatvorit moju stranu?
        return ERR_OK;
    }
    //printf("Received %u bytes\n\r", p->tot_len);

    /* first check allowed characters in this pbuf? */

    /* enqueue the pbuf */
    if (hs->req == NULL) {
      printf("First pbuf\n\r");

//            printf("04 ((len = matrixSslGetReadbuf(hs->ssl, &buf)) <= 0)\n");
//            if ((hs->buf_len = matrixSslGetReadbuf(hs->ssl, &hs->buf)) <= 0) {
//                //FIXME zavriet spojenie closeConn(cp, PS_ARG_FAIL);
//                http_close_conn(pcb, hs);
//            }
//            printf("04 len: %d\n", hs->buf_len);

      hs->req = lwip_pbuf;
      parsed = 0;
    } else {
        /* FIXME: 15. 10. 2012 ak uvazujem spravne, tak ked zavolam tuto
         * funkciu (httpd_recv) s nejakym lwip_pbuf (moze byt chain) tak
         * jeho data skopirujem do hs->buf (ktory spravuje matrixssl) s pbuf_copy_partial(...)
         */
        printf("!!!! pbuf enqueued. NEMA NASTAT!!!!!!!!!!!!!!!!!!!!!!!\n\r");
        while(1);
        pbuf_cat(hs->req, lwip_pbuf);
        parsed = 0;

        psAssert(FALSE);
        /* FIXME: kontrola, ci to vobec niekedy nastane. NEMALO by.
         * ak by to nastalo, tak musim znova premysliet kedy volat matrixSslGetReadbuf
         */
        LWIP_ERROR("buf by tu mal byt NULL", (hs->buf == NULL), return 0;);
        LWIP_ERROR("buf_len by tu mal byt 0", (hs->buf == 0), return 0;);
    }


    //FIXME matrixSslGetReadbuf vracia signed int. ked vysledok priradim do unsigned, tak sa to pokazi :(
    printf("04 ((len = matrixSslGetReadbuf(hs->ssl, &buf)) <= 0)\n");
    if ((hs->buf_len = matrixSslGetReadbuf(hs->ssl, &hs->buf)) <= 0) {
        //FIXME zavriet spojenie closeConn(cp, PS_ARG_FAIL);
        http_close_conn(pcb, hs);
        return ERR_MEM;
    }
    printf("04 len: %lu\n", hs->buf_len);

//        if (hs->req->next != NULL) {
//            printf("TO MAM ASI SPRACOVAT DATAi\n");
//            data_len = LWIP_MIN(hs->req->tot_len, LWIP_HTTPD_MAX_REQ_LENGTH);
//            pbuf_copy_partial(hs->req, httpd_req_buf, data_len, 0);
//
//            /* minimum zistujem hlavne koli tomu, aby som vedel kolko Bytov mozem
//             * v dalsom kroku zkopirovat.
//             * TODO: mozno, ked je "hs->buf_len" mensie, tak by som nemal
//             * pokracovat, lebo aj tak to nebudem vediet dekodovat
//             */
//        } else {
//            /* remove all data from httpd_req_buf */
//            printf("!!!!!!!TOTO asi nepotrebujem, a je to zle cele\n");
//            //memset(httpd_req_buf, 0, LWIP_HTTPD_MAX_REQ_LENGTH);
//            //data = (char *) lwip_pbuf->payload;
//
//            data_len = lwip_pbuf->len;
//            if (lwip_pbuf->len != lwip_pbuf->tot_len) { //co by sa nemalo stat
//                printf("Warning: incomplete header due to chained pbufs\n\r");
//            }
//        }

    //printf("data_len: %d\n\r", data_len);

    /* TODO: tu  by som mal zbierat data pred dekodovanim,
     * alebo pred tym, ako nad tym nieco spravim z ssl kniznice*/



    /*TODO v priklade sa na tomto mieste este nastavuje nasledovne:
     *  cp->fd = fd;
        fd = INVALID_SOCKET;
        cp->timeout = SSL_TIMEOUT;
        psGetTime(&cp->time);
        cp->parsebuf = NULL;
        cp->parsebuflen = 0;
     */

    //TODO a) dat do funkcie.
    // b) spravit funkciu ktora to ude volat a nastavit ju
    // ako callback pre tcp_sent()
    // c) na tomto mieste by som MAL dostat 0, lebo som este matrixSSl
    // nepovedal ze mi prisiel "Client Hello"

    hs->copied_data_len = 0;
    data_len = 0;
    do {
      if( (err = copy_to_matrinxssl_inbuf(pcb, hs)) != ERR_OK ) {
        http_close_conn(pcb, hs);
        return err;
      }
    } while( (hs->ssl_state == MATRIXSSL_REQUEST_RECV) && (hs->req->tot_len > hs->copied_data_len) );

    // FIXME 2011_12_3: naozaj mozem v tomto momente povedat ze som tie data zobral?? pravdepodobne neomzem,
    // 2013_02_16 podla vsetkeho mozem. ono sa aj tak iba prestavy state a druha strana sa informuje s dalsim packetom
    /* Inform TCP that we have taken the data, so it can advertise bigger window */
    //tcp_recved(pcb, hs->req->tot_len);



    enum return_codes ret = PROCESS;
    //int ret = WRITE;
    //while( ( ret != CONNECTION_CLOSED ) && ( ret != READ ) ) {
    while( 1 ) {
      switch( ret ) {
        case READ:
          //printf("hs->req->tot_len:%d, hs->copied_data_len:%d \n", hs->req->tot_len, hs->copied_data_len);
          if ( (hs->req != NULL) && (hs->req->tot_len > hs->copied_data_len) ) {
            if( (err = copy_to_matrinxssl_inbuf(pcb, hs)) != ERR_OK ) {
              http_close_conn(pcb, hs);
              return err;
            }
            ret = process_s(pcb, hs );
          } else {
            printf("KONIEC PRIJMU\n\n");
            tcp_recved(pcb, hs->copied_data_len);
            /* mozem uvolnit pamet s datami od druhej strany. */
            /* FIXME: hs->req by mal vzdy byt lwip_pbuf */
            if (hs->req != NULL) {
                /*FIXME 15. 10. 2012 hs->req by mal vzdy byt lwip_pbuf.
                 * na teraz spravim iba kontrolu:
                 */
                if (hs->req != lwip_pbuf) {
                    printf("!!!! ZISTI PRECO SA TO STALO");
                }
                pbuf_free(hs->req);
                hs->req = NULL;
            }
            return ERR_OK;
          }
          break;
        case PROCESS:
          ret = process_s(pcb, hs );
          printf("pxxxrocess_s() vratilo: %d\n", ret);
          break;
        case WRITE:
          //err = send_s(pcb, hs );
          ret = send_s( pcb, hs );
          printf("send_s() vratilo: %d\n", ret);
          break;
        case CLOSE_CONNECTION:
          printf("KONIEC SPOJENIA\n\n");
          tcp_recved(pcb, hs->copied_data_len);
          if (hs->req != NULL) {
            /*FIXME 15. 10. 2012 hs->req by mal vzdy byt lwip_pbuf.
             * na teraz spravim iba kontrolu:
             */
            if (hs->req != lwip_pbuf) {
                printf("!!!! ZISTI PRECO SA TO STALO");
            }
            pbuf_free(hs->req);
            hs->req = NULL;
          }
          http_close_conn(pcb, hs);
          return ERR_OK;
          break;
      }
    }

// FIXME 1. 12. 2011 vyriesit uvolnovanie premennych, hlavne lwip_pbuf
    /* Count number of pbufs in a chain */
    //pbuf_clen(hs->req);

    if (parsed != 0) {
        if (hs->req != NULL) {
            pbuf_free(hs->req);
            hs->req = NULL;
        }
        //FIXME: close_conn dat na rozumnejsie miesto. pbuf_free mozem volat asi aj skorej
    }

    if(hs != NULL ) {
      // mohol som dostat ces SSL Fatal alert a uz mozem mat zatvorene spojenie
      tcp_recved(pcb, hs->copied_data_len);

      /* mozem uvolnit pamet s datami od druhej strany. */
      /* FIXME: hs->req by mal vzdy byt lwip_pbuf */
      if (hs->req != NULL) {
          /*FIXME 15. 10. 2012 hs->req by mal vzdy byt lwip_pbuf.
           * na teraz spravim iba kontrolu:
           */
          if (hs->req != lwip_pbuf) {
              printf("!!!! ZISTI PRECO SA TO STALO");
          }
          pbuf_free(hs->req);
          hs->req = NULL;
      }
    }
    printf("KONIEC PRIJMU DAT\n");

    return ERR_OK;
}

static int process_s( struct tcp_pcb *pcb, struct http_state *hs) {
  switch (hs->ssl_state) {
    case MATRIXSSL_HANDSHAKE_COMPLETE:
        /* If the protocol is server initiated, send data here */
        //goto READ_MORE;
        printf("MATRIXSSL_HANDSHAKE_COMPLETE\n");
        return READ; //break;

    case MATRIXSSL_APP_DATA:
      /* TODO tu musim rozparsovat request a podla toho odpovedat
       * mozno sa chcem nechat inspirovat prikladom z lwIP pre POST.
       * POZOR: v pripade, ak request nie je cely, napr. buf_len je 0, alebo 1
       * tak musim zavolat matrixSslProcessedData(). podla toho co vrati musim
       * pokracovat dalej. (vid. server.c z matrixssl prikladov)
       */
        printf("MATRIXSSL_APP_DATA\n");

        /* Remember, must handle if len == 0! */
//        if ((hs->ssl_state = httpBasicParse(hs, hs->buf, hs->buf_len)) < 0) {
        //FIXME request_parse je aj pri CHYBE vacsie ako 0
        if ((hs->ssl_state = request_parse(hs, (char *) hs->buf, hs->buf_len) ) < 0) {
          _psTrace("Couldn't parse HTTP data.  Closing conn.\n");
          return CLOSE_CONNECTION;
//          http_close_conn(pcb, hs); //closeConn(cp, PS_PROTOCOL_FAIL);
//          return CONNECTION_CLOSED; //continue; /* Next connection */
        }
        if (hs->ssl_state == WRITE ) {
          return WRITE;
        }

        if (hs->ssl_state == HTTPS_COMPLETE) {
          if (CLOSE_CONNECTION == write_response(pcb, hs) ) {
            return CLOSE_CONNECTION;
            // http_close_conn(..) was already called inside write_response(..)
//            return CONNECTION_CLOSED;
          }
//          if (httpWriteResponse(hs->ssl) < 0) {
//              http_close_conn(pcb, hs);
//              //closeConn(cp, PS_PROTOCOL_FAIL);
//              return CONNECTION_CLOSED;
//          }
          /* For HTTP, we assume no pipelined requests, so we
             close after parsing a single HTTP request */
          /* Ignore return of closure alert, it's optional */
          /* nepovinny oznam klienktovi ze koncim ssl spojenie posielam v http_close_conn(...)
           * printf("matrixSslEncodeClosureAlert(cp->ssl)\n");
           * matrixSslEncodeClosureAlert(hs->ssl); */

          printf("matrixSslProcessedData(cp->ssl, &buf, (uint32*)&len)\n");
          hs->ssl_state = matrixSslProcessedData(hs->ssl, &hs->buf, (uint32*)&hs->buf_len);
          if (hs->ssl_state > 0) {
            /* Success. There is a second application data record in the buffer
             * that has been decoded. */
            while(1);//_psTrace(FALSE);
            /* vsetko co tam bolo som mal nacitat v http.c MOZNO by som tu nemal
             * koncit zatvarat spojenie ale iba spravit "return WRITE" aby som
             * klientovy poslal to co som si pripravil v write_response(..) */
            return CLOSE_CONNECTION;
//            http_close_conn(pcb, hs);
//            return CONNECTION_CLOSED;
            //closeConn(cp, PS_SUCCESS);
            //continue; /* Next connection */
          } else if (hs->ssl_state < 0) {
            /* some kind of Failure. */
              return CLOSE_CONNECTION;
//            http_close_conn(pcb, hs);
//            return CONNECTION_CLOSED;
            //closeConn(cp, PS_PROTOCOL_FAIL);
            //continue; /* Next connection */
          }
          /* hs->ssl_statec == 0, write what we have. it will call send_s(..) */
          return WRITE; //goto WRITE_MORE;
        }

        /* We processed a partial HTTP message */
        printf("07 (rc = matrixSslProcessedData(cp->ssl, &buf, (uint32*)&len)) == 0)\n");
        if ((hs->ssl_state = matrixSslProcessedData(hs->ssl, &hs->buf, (uint32*)&hs->buf_len)) == 0) {
          /* Success. This indicates that there are no additional records
           * in the data buffer that require processing. The application
           * protocol is responsible for deciding the next course of action. */
          return READ;//goto READ_MORE;
        }
        return PROCESS;//goto PROCESS_MORE;




    case MATRIXSSL_REQUEST_SEND:
      //FIXME: co tu robim a preco to robim???
        printf("MATRIXSSL_REQUEST_SEND\n");
        if (hs->req != NULL) {
            printf("Dalsi nebude enqueued\n");
            pbuf_free(hs->req);
            hs->req = NULL;
        }
        return WRITE; //break;

    case MATRIXSSL_REQUEST_RECV:
        printf("MATRIXSSL_REQUEST_RECV\n");
        return READ;//break;

    case MATRIXSSL_RECEIVED_ALERT:
        printf("MATRIXSSL_RECEIVED_ALERT\n");
        /* The first byte of the buffer is the level */
        /* The second byte is the description */
        if( *hs->buf == SSL_ALERT_LEVEL_FATAL ) {
          printf("Fatal alert: %d, closing connection.\n", *(hs->buf + 1) );
          return CLOSE_CONNECTION;
//          http_close_conn(pcb, hs);
//          return CONNECTION_CLOSED; //continue; /* Next connection */
        }
        /* Closure alert is normal (and best) way to close */
        if( *(hs->buf + 1) == SSL_ALERT_CLOSE_NOTIFY ) {
          printf("closure aler. (it is normal and best way to close)\n" );
          return CLOSE_CONNECTION;
//            http_close_conn(pcb, hs);
//            return CONNECTION_CLOSED; //continue; /* Next connection */
        }
        printf("Warning alert: %d\n", *(hs->buf + 1));

        printf("((rc = matrixSslProcessedData(cp->ssl, &buf, (uint32*)&len)) == 0)\n");
        hs->ssl_state = matrixSslProcessedData( hs->ssl,
                                                  &hs->buf,
                                                  (u32 *)&hs->buf_len
                                                );
        if ( hs->ssl_state == 0 ) {
          /* No more data in buffer. Might as well read for more. */
          return READ; //goto READ_MORE;
        }

        //FIXME: zatial nemenim funkcionalitu, ale chcem sem
        // sat to iste co som mal v elua_httpsd.c
        //return WRITE;//break;
        return PROCESS;
    default:
        printf("default\n");
        //FIXME: plati to iste ako pri MATRIXSSL_RECEIVED_ALERT
        return WRITE;// break;
  }
}

static int send_s( struct tcp_pcb *pcb, struct http_state *hs) {
  u16_t can_sent;
  err_t err;

  //v matrixssl priklade bolo pred nalsimi riadkami WRITE_MORE:
  printf("02 (len = matrixSslGetOutdata(cp->ssl, &buf)) > 0\n");
  if ((hs->tcp_out_buf_len = matrixSslGetOutdata(hs->ssl, &hs->tcp_out_buf)) > 0) {
      can_sent = tcp_sndbuf(pcb);
      printf("buf_len: %lu, can_sent: %hu\n", hs->tcp_out_buf_len, can_sent);
      if (can_sent > hs->tcp_out_buf_len) {
          //mozem poslat vsetko
          err = tcp_write(pcb, hs->tcp_out_buf, hs->tcp_out_buf_len, 0);
          tcp_output( pcb );
      } else {
          err = tcp_write(pcb, hs->tcp_out_buf, can_sent, 0);
          tcp_output( pcb );
          /* FIXME: v pripade mensich paketov chcem nejakym sposobom po tom
           * ako mi na to druha strana povie ack, chcem poslat zvysok.
           * pri ACK lwIP vola callback zaregistrovany cez tcp_sent(...).
           * kde by som mal poslat zvysok.
           * asi aj na zaciatku callback-u zaregistovaneho cez tcp_recv()
           * by som sa mal pozerat, ci mam poslane vsetko co mam vo vystupnom
           * buffri a ci to mozem poslat. v priade ze neposlem vsetko tak
           * nema vyznam posielat druhej strane na priate data ACK a znazit
           * sa priate data nejako spracovat. malo by to vyznam jedine v pripade,
           * ze by som na to nemusel posielat odpoved.
           */
          // ostatne poslem neskor
      }
      if ( err != ERR_OK ) {
        return CLOSE_CONNECTION;
//          http_close_conn(pcb, hs);
//          return CONNECTION_CLOSED;
      }
  } else if ( hs->tcp_out_buf_len < 0) {
      printf("zatvaram spojenie. matrixSslGetOutdata(...) vratilo chybu: %lu, \n", hs->tcp_out_buf_len);
      return CLOSE_CONNECTION;
//      http_close_conn(pcb, hs);
//      return CONNECTION_CLOSED;
  }
  return READ;
}

/**
 * The poll function is called every 2nd second.
 * If there has been no data sent (which resets the retries) in 8 seconds, close.
 * If the last portion of a file has not been sent in 2 seconds, close.
 * 
 * This could be increased, but we don't want to waste resources for bad connections.
 *
 * TODO: tu mozno chcem kontrolovat, ci nechcem nieco poslat z vykonavania lua skriplu
 *       */
static err_t httpd_poll(void *arg, struct tcp_pcb *pcb) {
    //int len;
    struct http_state *hs = (struct http_state *)arg;
    
    printf("httpd_poll %x\n\r", (unsigned int) pcb);
    
    if(hs->retries > 2){
        if (hs->req != NULL) {
            pbuf_free(hs->req);
            hs->req = NULL;
        }
        http_close_conn(pcb, hs);
    } else {
        hs->retries++;
    }

    //FIXME: skoncit spojenie

    //len = tcp_sndbuf(pcb);
    //printf("len4_1: %d\n\r", len);
    //tcp_write(pcb, webif_css4, strlen(webif_css4), 0);
    //tcp_output(pcb);
    //len = tcp_sndbuf(pcb);
    //printf("len4_2: %d\n\r", len);
    return ERR_OK;
}

/**
 * The pcb had an error and is already deallocated.
 * The argument might still be valid (if != NULL).
 */
static void
httpd_err(void *arg, err_t err)
{
  struct http_state *hs = (struct http_state *)arg;
  LWIP_UNUSED_ARG(err);

//  LWIP_DEBUGF(HTTPD_DEBUG, ("httpd_err: %s", lwip_strerr(err)));
  printf("httpd_err: %d. The pcb had an error and is already deallocated.\n", err);

  if (hs != NULL) {
    http_state_free(hs);
  }
}

/**
 * Data has been sent and acknowledged by the remote host.
 * This means that more data can be sent.
 **/
static err_t httpd_sent(void *arg, struct tcp_pcb *pcb, u16_t len) {
    //bool parsed;
  int ret;
    struct http_state *hs = (struct http_state *)arg;

    //printf("http_sent %p\n", (void*)pcb);
    printf("http_sent() hs:%x, len: %d\n\r", (unsigned int) arg, len);

    if (hs == NULL) {
        return ERR_OK;
    }

    hs->retries = 0;
    

    printf("03 ((rc = matrixSslSentData(cp->ssl, transferred)) < 0)\n");
    //FIXME tu musim skontrolovat err po tcp_write a do matrixSslSentData
    // musim niekedy poslat can_sent namiesto hs->buf_len, zalezi
    // ci "if (can_sent > hs->buf_len)"
    if ((hs->ssl_state = matrixSslSentData(hs->ssl, len)) < 0) {
        printf("MAL BY SOM SKONCIT\n");
        http_close_conn(pcb, hs);
        return CONNECTION_CLOSED;
        //closeConn(cp, PS_ARG_FAIL);
        //continue;   /* Next connection */
    }
    if (hs->ssl_state == MATRIXSSL_REQUEST_CLOSE) {
        printf("MATRIXSSL_REQUEST_CLOSE\n");
        http_close_conn(pcb, hs);
        return CONNECTION_CLOSED;
        //closeConn(cp, MATRIXSSL_REQUEST_CLOSE);
        //continue;   /* Next connection */
    } else if (hs->ssl_state == MATRIXSSL_HANDSHAKE_COMPLETE) {
        printf("MATRIXSSL_HANDSHAKE_COMPLETE\n");
        /* If the protocol is server initiated, send data here */
    }
    /* Update activity time */
    //psGetTime(&cp->time);
//    if (rc == MATRIXSSL_REQUEST_SEND || transferred < len) {
//              if (wSanity++ < GOTO_SANITY) goto WRITE_MORE;
//      }
    if (hs->ssl_state == MATRIXSSL_REQUEST_SEND || len < hs->tcp_out_buf_len) {
      ret = send_s( pcb, hs );
      printf("send_s() z httpd_sent() vratilo: %d\n", ret);
    } else if ( (hs->to_send_buf_len == hs->to_send_buf_used_len) &&
                 (hs->to_send_buf_len != 0) ) {
      /* lua execution was interupted in http_out(..) function. prepare next
       * interuption and resume last execution */
      if((ret = setjmp(hs->parent)) != 0) {
        // after longjmp
        if(ret == WRITE) {
          send_s( pcb, hs );
        } else if (ret == HTTPS_COMPLETE) {
          if(CLOSE_CONNECTION == write_response(pcb, hs) ) {
            http_close_conn(pcb, hs);
            return ERR_OK;
          }
          send_s( pcb, hs );
        } else if (ret == HTTPS_PARTIAL) { //READ
          //copy or read
          if (hs->req->tot_len < hs->copied_data_len) {
            err_t err;
            if( (err = copy_to_matrinxssl_inbuf(pcb, hs)) != ERR_OK ) {
              http_close_conn(pcb, hs);
              return err;
            }
            longjmp(hs->lua_child, 1);
          } // else READ
          return ERR_OK;
        }
        return ERR_OK;
      }
      longjmp(hs->lua_child, 1);
    } else if(  (hs->ssl_state == MATRIXSSL_SUCCESS) &&
                (len == hs->tcp_out_buf_len) &&
                (hs->to_send_buf_used_len < hs->to_send_buf_len)
               ) {
      //TODO mozno nechcem skoncit pre nejaky iny pripad
      //keep-alive nespravi http_close_conn(pcb, hs); ale iba
      http_clear_conn(hs);
      return ERR_OK;
    }

//    if(hs->file != NULL) {
//        httpd_send_static(pcb, hs, strlen(hs->file));
//        ret = send_s( pcb, hs );
//        printf("send_s() vratilo: %d\n", ret);
//    }
    
    return ERR_OK;
}


/* This is the callback function that is called when
a connection has been accepted. 
TODO: mozno sa mi na nieco zide tcp_setprio */
static err_t httpd_accept(void *arg, struct tcp_pcb *pcb, err_t err) {   
    struct http_state *hs;
    struct tcp_pcb_listen *lpcb = (struct tcp_pcb_listen*)arg;
    if (err != ERR_OK) {
        return err;
    }
    //FIXME toto je doscasne. na to robit keepalive
    pcb->so_options |= SOF_KEEPALIVE;


    //FIXME lpcb sa ma pouzivat nejako takto asi
    /* Decrease the listen backlog counter */
    tcp_accepted(lpcb);
    
    hs = http_state_alloc();
    if (hs == NULL) {
        printf("http_accept: Out of memory, RST\n\r");
        return ERR_MEM;
    }

    //TODO: spravit testif ((rc = matrixSslNewServerSession(&hs->ssl, keys, certCb)) < 0) {
            //toto nahrad lwIP vecou. close(fd); fd = INVALID_SOCKET;
    //        continue;
    //}
    matrixSslNewServerSession(&hs->ssl, keys, certCb);
    
    /* Tell TCP that this is the structure we wish to be passed for our
     *      callbacks. */
    tcp_arg(pcb, hs);

    /* Set up the function () to be called when data
    arrives. */
    tcp_recv(pcb, httpd_recv);
    tcp_poll(pcb, httpd_poll, HTTPD_POLL_INTERVAL);
    tcp_sent(pcb, httpd_sent);

    tcp_err(pcb, httpd_err);
    return ERR_OK;
}

/* The initialization function. */
err_t httpd_init(void) {   
    struct tcp_pcb *pcb, *lpcb;
    err_t  err;
    /* Create a new TCP PCB. */
    pcb = tcp_new();
    if (pcb == NULL) {
        printf("F: Fail to create PCB\n\r");
        return ERR_BUF;
    }
    /* Bind the PCB to TCP port 80. */
    err = tcp_bind(pcb, NULL,443);
    if (err != ERR_OK) {
        printf("E: tcp_bind %x\n\r", err);
        return err;
    }
    /* Change TCP state to LISTEN. */
    lpcb = tcp_listen(pcb);
    if (lpcb == NULL) {
      printf("E: tcp_listen\n\r");
      err = tcp_close(pcb);
      if (err != ERR_OK) {
        printf("Error %d closing %p\n", err, (void*)pcb);
      }
      return ERR_BUF;
    }
    //httpd_init_webif_head();

    tcp_arg(lpcb, lpcb);
    /* Set up httpd_accet() function to be called
    when a new connection arrives. */
    tcp_accept(lpcb, httpd_accept);
    printf("DALSI RIADOK SOM SEM DOPLNIL ZO ZALOHY, a neskusal som, co to spravi. ale ma tu asi byt. je tam 'init_matrixSsl();' \n");
    init_matrixSsl();
    
    

    return ERR_OK;
}

