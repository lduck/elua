/*
 *	app.h
 *	Release $Name: MATRIXSSL-3-2-2-OPEN $
 *	
 *	Header for MatrixSSL example sockets client and server applications
 */
/*
 *	Copyright (c) PeerSec Networks, 2002-2011. All Rights Reserved.
 *	The latest version of this code is available at http://www.matrixssl.org
 *
 *	This software is open source; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This General Public License does NOT permit incorporating this software 
 *	into proprietary programs.  If you are unable to comply with the GPL, a 
 *	commercial license for this software may be purchased from PeerSec Networks
 *	at http://www.peersec.com
 *	
 *	This program is distributed in WITHOUT ANY WARRANTY; without even the 
 *	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 *	See the GNU General Public License for more details.
 *	
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *	http://www.gnu.org/copyleft/gpl.html
 */
/******************************************************************************/

#ifndef _h_MATRIXSSLAPP
#define _h_MATRIXSSLAPP

#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************/

#include <errno.h>			/* Defines EWOULDBLOCK, etc. */
#include <fcntl.h>			/* Defines FD_CLOEXEC, etc. */
#include <stdlib.h>			/* Defines malloc, exit, etc. */
#include <setjmp.h>


#ifdef POSIX
#include <netdb.h>			/* Defines AF_INET, etc. */
#include <unistd.h>			/* Defines close() */
#include <netinet/tcp.h>	/* Defines TCP_NODELAY, etc. */
#include <arpa/inet.h>		/* inet_addr */
#endif

#ifdef WIN32
#include <Winsock2.h>
#include <Ws2tcpip.h>

#define SIGPIPE			SIGABRT
#define snprintf		_snprintf
#define close			closesocket
#define MSG_DONTWAIT	0
#ifndef EWOULDBLOCK
#define EWOULDBLOCK		WSAEWOULDBLOCK
#endif
#ifndef EINPROGRESS
#define EINPROGRESS		WSAEINPROGRESS
#endif
#define strcasecmp		lstrcmpiA
#endif /* WIN32 */

#include <setjmp.h>

#include "core/coreApi.h"
#include "matrixssl/matrixsslApi.h"

#include "lua.h"


	
/******************************************************************************/
/*
	 Platform independent socket defines for convenience
 */
#ifndef SOCKET
	typedef int32 SOCKET;
#endif
#ifndef INVALID_SOCKET
#define INVALID_SOCKET	-1
#endif
	
#ifdef WIN32
#define SOCKET_ERRNO	WSAGetLastError()
#else
#define SOCKET_ERRNO	errno
#endif
	
/******************************************************************************/
/*
	Configuration Options
*/
#define HTTPS_PORT		4433	/* Port to run the server/client on */

/******************************************************************************/
/*
	Protocol specific defines
 */
/* Maximum size of parseable http element. In this case, a HTTP header line. */
#define HTTPS_BUFFER_MAX 256	
	
/* Return codes from http parsing routine */

enum return_codes {
  HTTPS_ERROR = 0,      /* Invalid/unsupported HTTP syntax */
  HTTPS_PARTIAL,        /* Only a partial request/response was received */ //the same as READ
  HTTPS_COMPLETE,       /* Full request/response parsed */
  CONNECTION_CLOSED, // can finish
  CLOSE_CONNECTION,     // indicate that conection should be closed
  READ,                 // call recv_s
                        /* --pri lwIP mi nestaci skoncit. do matrixSslReceivedData(...) funkcie
                         *  som nemusel dat cele hs->req->tot_len. hlavne ak mam TCP_MSS (v lwipopts.h)
                         *  vacsie  ako SSL_DEFAULT_IN_BUF_SIZE v (matrixsslConfig.h)*/
  WRITE,                // call send_s
  PROCESS,               // call process_s
};

enum http_method {
        HTTP_METHOD_UNKNOW = 0,
        HTTP_METHOD_GET,
        HTTP_METHOD_HEAD, //TODO implementovat
        HTTP_METHOD_POST,
        HTTP_METHOD_PUT,
};

struct conn_flags {

};

struct mimetype {
  const char *extn;
  const char *mime;
};

struct http_response {
  u16 stat_code;
  const char *stat_msg;
  const char *mime;
  FILE *file;
  u32 file_size;

};

struct http_request {
  const char *mime;
  enum http_method method;
  char *to_parse_buf;   /* MY MALLOC. unparsed partial data */
  u32 to_parse_buf_len;
  char *content_buf;    /* malloc by matrixSSL. input data for POST and PUT */
  u32 content_buf_len;  /* data available in content_buf */
  u32 content_left; /* Initialized from header Content-Length. decrementing at read*/
  u8 connection_flags : 3; /* 0bxx1  HTTP/1.1, 0bxx0 HTTP/x.x
                              0bx1x  "Connection: close"
                              0b1xx 0x4 Connection: keep-alive*/

                    /* 1 if we received "Connection: close".
                          2 if we didn't receive request with "Connection: keep-alive"
                            and request version is other than HTTP/1.1 */
//        enum http_version version;
//        int redirect_status;
//        char *url;
//        char *headers[UH_LIMIT_HEADERS];
//        struct auth_realm *realm;
};

typedef struct {
	DLListEntry		List;
	ssl_t			*ssl;
	SOCKET			fd;
	psTime_t		time;		/* Last time there was activity */
	uint32			timeout;	/* in milliseconds*/
	uint32			flags;
	unsigned char	*parsebuf;		/* Partial data */
	uint32			parsebuflen;
} httpConn_t;


/* FIXME: zatial som to dal sem, ale tento subor vobec nechcem icludovat, takze
 * to treba presunut niekam inam.
 */
struct http_state {
  struct tcp_pcb *pcb; /* with this have to remember just one pointer in LUA_REGISTRYINDEX*/
    struct pbuf *req;
    char *file;
    char *to_free;
    //char *unparsed_request;/* Pointer to the first unparsed byte in buf. */
    u_char retries;
    //matrixssl
    int32 ssl_state;
    ssl_t *ssl;
    unsigned char *buf; /* malloc by matrixSSL. TODO: asi rozdelit na viacero */
    int32 buf_len;      /* must be signed */
    u16 copied_data_len;  /* how much I copied with pbuf_copy_partial(...) */


//    psTime_t time;       /* Last time there was activity */
//    uint32 timeout;    /* in milliseconds*/
//    uint32 flags;
//    char *unparsed_line;

    lua_State *L;
    u_char server_state;  /* indicates if we are parsing done with parsing headers
                              and reading input (request part after "\r\n\r\n" ) */

    //WRITE
    char *to_send_buf; /* malloc by matrixSSL */
    int32 to_send_buf_len;
    int32 to_send_buf_used_len;
//    int32 encoded;       /* how many Bytes from to_send_buf was already
//                         * processed by matrixSslEncodeWritebuf */
    unsigned char *tcp_out_buf; /*data from matrixSslGetOutdata() to tcp_write() */
    int32 tcp_out_buf_len;

    jmp_buf parent;
    jmp_buf lua_child;
    void *stack;
    uint32 stack_size;

    jmp_buf cont_lua_read;
    jmp_buf read_more;

    struct http_request *http_req; /* informations about HTTP request */
    struct http_response *http_resp;
};

#define GET_MIN(x , y)  (((x) < (y)) ? (x) : (y))

/* http_state->parsing_state values
 * to know, what we already parsed and what should be next */
#define FISRT_LINE      0 /* still need more data to read full first line */
#define HEADERS         1 /* parsing headers. we didn't find "\r\n\r\n" */
#define BODY            2 /* buffering request body that is after "\r\n\r\n" */
#define WRITING_RES     3 /* writing response to network */
#define DUMMY_READ      4 /* read all data (headers/content) from client
                             to prevent lwIP sending RST to client */
#define REQUEST_PARSE_DONE 5
#define WRITE_HEADERS   6
#define WRITE_BODY      7

extern int32 httpBasicParse(struct http_state *cp, unsigned char *buf, uint32 len);
extern u8 wsapi_run (struct http_state *hs);//lua_State *L);
/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _h_MATRIXSSLAPP */

/******************************************************************************/
