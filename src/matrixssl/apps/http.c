/*
 *	http.c
 *  Release $Name: MATRIXSSL-3-2-2-OPEN $
 *
 *	Simple INCOMPLETE HTTP parser for example applications
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

///#include "app.h"
#include "apps/app.h"
#include "apps/uhttpd-mimetypes.h"

#include <string.h>
#include <ctype.h>

#include <setjmp.h>

#define MAX_PATH 1024
#define CRLF "\r\n"
#define CRLF_LEN 2

//enum http_method {
//  GET,
//  POST,
//  HEAD,
//};



#include <sys/cdefs.h>

/*
 * Find the first occurrence of find in s, where the search is limited to the
 * first slen characters of s.
 */
char *
strnstr(s, find, slen)
        const char *s;
        const char *find;
        size_t slen;
{
        char c, sc;
        size_t len;

        if ((c = *find++) != '\0') {
                len = strlen(find);
                do {
                        do {
                                if (slen-- < 1 || (sc = *s++) == '\0')
                                        return (NULL);
                        } while (sc != c);
                        if (len > slen)
                                return (NULL);
                } while (strncmp(s, find, len) != 0);
                s--;
        }
        return ((char *)s);
}

/* @note The input-string need not be, and the output string will not be, null-terminated.
 * @param buf output string/destination of decoded url
 * @param blen is the size of buf
 * @param src input-string that will be decoded
 * @param slen is the length of src
 * @return the length of the decoded string, -1 on buffer overflow, -2 on malformed string.
 */
u32 urldecode( char *buf, u32 blen, const char *src, u32 slen)
{
  u32 i;
  u32 len = 0;

#define hex(x) \
        (((x) <= '9') ? ((x) - '0') : \
                (((x) <= 'F') ? ((x) - 'A' + 10) : \
                        ((x) - 'a' + 10)))

        for (i = 0; (i < slen) && (len < blen); i++)
        {
                if (src[i] == '%')
                {
                        if (((i+2) < slen) && isxdigit(src[i+1]) && isxdigit(src[i+2]))
                        {
                                buf[len++] = (unsigned char)(16 * hex(src[i+1]) + hex(src[i+2]));
                                i += 2;
                        }
                        else
                        {
                                /* Encoding error: it's hard to think of a
                                ** scenario in which returning an incorrect
                                ** 'decoding' of the malformed string is
                                ** preferable to signaling an error condition. */
                                #if 0 /* WORSE_IS_BETTER */
                                    buf[len++] = '%';
                                #else
                                    //return -2;
                                    return 0;
                                #endif
                        }
                }
                else
                {
                        buf[len++] = src[i];
                }
        }

        return (i == slen) ? len : 0;
}

/* from mini_httpd.c v:1.19
 * TODO:
 * 1. path sa prechadza od zaciatku do konca pre kazdu odstranovanu vec
 * 2. ak sa odstranovana vec vyskytuje viac krat, tak sa kopiruje viac ako je treba
 */
//static u32 de_dotdot( char* path, u32 path_len ) {
//  char* cp;
//  char* cp2;
//  u32 shift;
//  int l;
//
//  /* Collapse any multiple / sequences. */
//  while ( ( cp = strnstr( path, "//", path_len) ) != NULL ) {
//    shift = 2;
//    for ( cp2 = cp + 2; *cp2 == '/'; cp2++ ) {// in case there is '///' or more
//      shift++;
//    }
//    cp++;
//    path_len -= shift; //shift by mal byt to iste ako (cp2 - cp)
//    strncpy( cp, cp2, path_len - (cp - path) );
//  }
//
//  if( path_len > 0) {
//      return 0; //should not happen
//  } //TODO: mozno ked  je path_len < 2 tak mozem 'return path_len;'
//
//  /* Remove leading . (dot) like ./path or ../path */
//  cp = path;
//  while ( ( path_len > 0 ) && ( *cp == '.' ) && ( (cp - path) < 3) ) {
//    cp++;
//    path_len--;
//  }
//  if( cp!=path ) {
//    strncpy( path, cp, path_len );
//  }
//
////  cp = path;
////  while ( ( path_len > 0 ) && ( strncmp( cp, "./", 2 ) == 0 ) ) {
////      path_len -= 2;
////      cp += 2;
////  }
////  if( path_len > 0 ) {
////    strncpy( path, cp, path_len );
////  } else {
////      return 0;
////  }
//
//  /* and any /./ sequences. */
//  shift =0;
//  cp = path;
//  while ( ( cp = strnstr( cp, "/./", path_len) ) != NULL ) {
//    path_len -= 2;
//    strncpy( cp, cp + 2, path_len );
//    cp = path;
//  }
//
//
//  //FIXME nieco som uz pomenil, ale je to podla mna zel
//  /* Alternate between removing leading ../ and removing xxx/../ */
//  for (;;)
//      {
//      while ( strncmp( file, "../", 3 ) == 0 )
//          (void) strcpy( file, file + 3 );
//      cp = strstr( file, "/../" );
//      if ( cp == (char*) 0 )
//          break;
//      for ( cp2 = cp - 1; cp2 >= file && *cp2 != '/'; --cp2 )
//          continue;
//      (void) strcpy( cp2 + 1, cp + 4 );
//      }
//
//  /* Also elide any xxx/.. at the end. */
//  while ( ( l = strlen( file ) ) > 3 &&
//          strcmp( ( cp = file + l - 3 ), "/.." ) == 0 ) {
//    for ( cp2 = cp - 1; cp2 >= file && *cp2 != '/'; --cp2 )
//        continue;
//    if ( cp2 < file )
//        break;
//    *cp2 = '\0';
//  }
//
//  return path_len;
//}

static const char * mime_lookup(const char *ext) {
  struct mimetype *m = &uh_mime_types[0];

  while (m->extn) {
    if (!strcasecmp(ext, m->extn)) {
      return m->mime;
    }
    m++;
  }
  return "application/octet-stream";
}


int32 parse_first_line(struct http_state *hs, char * line, u32 line_len )
{
  char *method  = line;
  char *url = NULL;
  char *protocol_version = NULL;
  char *query = NULL;

  char path[MAX_PATH];
  u32 path_len;

  /* find request path */
  if( (url = strchr(line, ' ')) != NULL ) {
    *url++ = '\0';
  } else {
    hs->server_state = WRITE_HEADERS;
    hs->http_resp->stat_code = 400;
    hs->http_resp->stat_msg = "Bad Request";
    return 400;
  }

  /* find http version */
  if( (protocol_version = strchr(url, ' ')) != NULL ) {
    *protocol_version++ = '\0';
  } else {
    hs->server_state = WRITE_HEADERS;
    hs->http_resp->stat_code = 400;
    hs->http_resp->stat_msg = "Bad Request";
    return 400;
  }
  if( !strcmp(protocol_version, "HTTP/1.1") ) {
    hs->http_req->connection_flags |= 0x01;
  }

  /* find query after path. if not there then query == NULL*/
//  query = strchr( url, '?' );
  if( (query = strchr( url, '?' )) != NULL ) {
      path_len = (u32) (query - url);
  } else {
      path_len = (u32) ( (protocol_version -1) - url);
  }

  /* check method */
//  if (method && !strcmp(method, "GET"))
//    method = UH_HTTP_MSG_GET;
//  else if (method && !strcmp(method, "POST"))
//          req->method = UH_HTTP_MSG_POST;
//  else if (method && !strcmp(method, "HEAD"))
//          req->method = UH_HTTP_MSG_HEAD;
//  else {return 405}

  if( !strcmp(method, "GET") ) {
    hs->http_req->method = HTTP_METHOD_GET;
  } else if( !strcmp(method, "HEAD") ) {
    hs->http_req->method = HTTP_METHOD_HEAD;
  } else if( !strcmp(method, "POST") ) {
    hs->http_req->method = HTTP_METHOD_POST;
  } else if( !strcmp(method, "PUT") ) {
    hs->http_req->method = HTTP_METHOD_PUT;
  } else {
    hs->server_state = WRITE_HEADERS;
    hs->http_resp->stat_code = 500;
    hs->http_resp->stat_msg = "Not Implemented";
    return 501; // 501, "Not Implemented");
    //return 405; // 405, "Method Not Allowed");
  }

  //printf("hs->http_req->method: %d, %d\n", hs->http_req->method, HTTP_METHOD_PUT );
  /* check_path:
   * urldecode
   * dedot path -
   */
  path_len = urldecode(path, MAX_PATH, url, path_len);
  if(path_len == 0) { //FIXME: urldecode() vracia UNsigned int
    hs->server_state = WRITE_HEADERS;
    hs->http_resp->stat_code = 400;
    hs->http_resp->stat_msg = "Bad Request";
    return 400; /* Bad filename. 'decoding' of the malformed url string or
                 * buffer overflow in urldecode function */
  }

  //TODO spravit odstranovanie ./ ../  // z cesty. v uhttp-utils je mozny sposob ako to riesit v canonpath()
//  if ( *path != '/') {
//    return 400; // Bad filename.
//  } else if ( path_len > 1 ) {
//    de_dotdot(path, path_len);
//  }
//  if(path_len == 0) {
//      return 400;
//  }

  path[path_len] = '\0';

#include "ff.h"


  if(hs->http_req->method != HTTP_METHOD_PUT ) {
    FILINFO fno;
    /*FIXME ak nebudem testovat ci je to subor alebo adresar, tak toto tu nemusim resit
     * a v send_response mozem pouzit f_size(FILE *f) http://elm-chan.org/fsw/ff/en/size.html */
    if(f_stat(path+4, &fno) != FR_OK ) {
      _psTraceStr("Subor %s NEexistuje. chyba pri f_stat()\n", path);
      hs->server_state = WRITE_HEADERS;
      hs->http_resp->stat_code = 404;
      hs->http_resp->stat_msg = "Not Found. No such file or directory";
      return 404;
    }

    hs->http_resp->file_size = fno.fsize;

    printf("Subor %s  hs->http_req->file_size:%d\n", path,  hs->http_resp->file_size);

    FILE *f;
    if( (f = fopen(path, "r")) == NULL) {
      fclose(f);
      hs->http_resp->file = NULL;
      _psTraceStr("Subor %s NEexistuje\n", path);
      //"Not Found", "No such file or directory"
      hs->server_state = WRITE_HEADERS;
      hs->http_resp->stat_code = 404;
      hs->http_resp->stat_msg = "Not Found. No such file or directory";
      return 404;
    }

    // find extension
    const char *ext = strrchr(path, '.');
    if(ext != NULL ) {
      ext++;
      if( strcmp(ext, "lc") && strcmp(ext, "lua") ) {
        hs->http_resp->mime = mime_lookup(ext);
        ext = NULL;
      }
    }

    if(ext == NULL ) {
      hs->http_resp->file = f;
      hs->L = NULL; /* not necessary */
      return 200; // OK
    }

    fclose(f);
  }

  hs->http_resp->file = NULL; /* not necessary */
  hs->L = wsapi_prepare();
  if(hs->L == NULL) {
    hs->server_state = WRITE_HEADERS;
    hs->http_resp->stat_code = 500;
    hs->http_resp->stat_msg = "Internal Server Error";
    return 500;
  }

    _psTraceStr("Subor %s existuje alebo to je ADRESAR\n", path);

  _psTraceStr("method: %s\n", method);
  _psTraceStr("path: %s\n", path);
  _psTraceStr("query: %s\n", query);
  _psTraceStr("protocol_version: %s\n", protocol_version);

  wsapi_add_str_header(hs->L, "REQUEST_METHOD", method);
  wsapi_add_str_header(hs->L, "SCRIPT_NAME", path);  //SCRIPT_NAME=/cgi-bin/hello.lua
  wsapi_add_str_header(hs->L, "QUERY_STRING", query); //QUERY_STRING=t=964933&xxx=1000
  wsapi_add_str_header(hs->L, "REQUEST_URI", url);  //REQUEST_URI=/cgi-bin/hello.lua?t=964933&xxx=1000
  wsapi_add_str_header(hs->L, "SERVER_PROTOCOL", protocol_version);
  /* TODO SCRIPT_FILENAME je SCRIPT_FILENAME s DOCUMENT_ROOT
   * wsapi_add_str_header(hs->L, "SCRIPT_FILENAME", );
   * wsapi_add_str_header(hs->L, "DOCUMENT_ROOT", );
   * wsapi_add_str_header(hs->L, "PWD", ); //PWD=/usr/lib/cgi-bin
   */

  return 200; // OK
}

/**
 * Used to find out header type and value and add it to wsapi enviroment table.
 * Some header names are replaced with proper CGI names.
 *
 * @param L lua_State for this http request/connection
 * @param unparsed pointer to the beginning of zero-terminated string request line
 * @return char pointer to the place where was '=' replaced be '\0', or
 *              NULL if there was no '=' in the line
 */
char * parse_header_line(struct http_state *hs, char *unparsed) {
  lua_State *L = hs->L;
  _psTraceStr("parse_header_line(%s)\n", unparsed);
  char *value;
  value = strstr(unparsed, ": ");
  if(value == NULL) {
    return NULL;
  }
  *value = '\0';
  value += 2; // Host: 8.8.8.8


  if(!strcasecmp(unparsed, "Content-Length")) {
    hs->http_req->content_left = atoi(value);
    if(L != NULL) {
      wsapi_add_str_header(L, "CONTENT_LENGTH", value); //TODO wsapi_add_int_header(...,atoi(value))
    }
  } else if( !strcasecmp(unparsed, "Connection") ) {
    if( !strcasecmp(value, "close") ) {
      hs->http_req->connection_flags |= 0x02;
    } else if( !strcasecmp(value, "keep-alive") ) {
      hs->http_req->connection_flags |= 0x04;
    }
  }
  if(L != NULL) {
    if (!strcasecmp(unparsed, "Accept")) {
      wsapi_add_str_header(L, "HTTP_ACCEPT", value);
    } else if (!strcasecmp(unparsed, "Accept-Charset")) {
      wsapi_add_str_header(L, "HTTP_ACCEPT_CHARSET", value);
    } else if (!strcasecmp(unparsed, "Accept-Encoding")) {
      wsapi_add_str_header(L, "HTTP_ACCEPT_ENCODING", value);
    } else if (!strcasecmp(unparsed, "Accept-Language")) {
      wsapi_add_str_header(L, "HTTP_ACCEPT_LANGUAGE", value);
    } else if (!strcasecmp(unparsed, "Authorization")) {
      wsapi_add_str_header(L, "HTTP_AUTHORIZATION", value);
    } else if (!strcasecmp(unparsed, "Connection")) {
      wsapi_add_str_header(L, "HTTP_CONNECTION", value);
    } else if (!strcasecmp(unparsed, "Cookie")) {
      wsapi_add_str_header(L, "HTTP_COOKIE", value);
    } else if (!strcasecmp(unparsed, "Host")) {
      wsapi_add_str_header(L, "HTTP_HOST", value);
    } else if (!strcasecmp(unparsed, "Referer")) {
      wsapi_add_str_header(L, "HTTP_REFERER", value);
    } else if (!strcasecmp(unparsed, "User-Agent")) {
      wsapi_add_str_header(L, "HTTP_USER_AGENT", value);
    } else if (!strcasecmp(unparsed, "Content-Type")) {
      wsapi_add_str_header(L, "CONTENT_TYPE", value);
    } else {
      wsapi_add_str_header(L, unparsed, value);
    }
  }
  return value;
}

static void prepare_response(struct http_state *hs) {
  u32 len;
  if(hs->server_state == WRITE_HEADERS ) {
    if(hs->to_send_buf==NULL) {
//      if ((hs->to_send_buf_len = matrixSslGetWritebuf(hs->ssl, &hs->to_send_buf, hs->http_resp->file_size) ) < 0 ) {
//      if ((hs->to_send_buf_len = matrixSslGetWritebuf(hs->ssl, &hs->to_send_buf, 987) ) < 0 ) {
      if ((hs->to_send_buf_len = matrixSslGetWritebuf(hs->ssl, &hs->to_send_buf, 0) ) < 0 ) {
        //return PS_MEM_FAIL;
        while(1);//FIXME chcem vevidet kedy a ci sa stane pri testovani
        //return CLOSE_CONNECTION;
      }
    }
    //hs->to_send_buf_len--; //FIXME hotfix matrixSslGetWritebuf funkcie

    u8 d1, d2;

    //////////////
    len = snprintf( hs->to_send_buf + hs->to_send_buf_used_len, hs->to_send_buf_len - hs->to_send_buf_used_len,
                                     "HTTP/1.%d %d %s\r\n",
                                     hs->http_req->connection_flags & 0x1,
                                     hs->http_resp->stat_code,
                                     hs->http_resp->stat_msg   );
    //todo assert len musi byt <= ako (hs->to_send_buf_len - hs->to_send_buf_used_len)
    if( ( *( hs->to_send_buf+len-2) == '\r' ) &&
        ( *( hs->to_send_buf+len-1 ) == '\n' ) ) {
      hs->to_send_buf_used_len += len;
    }
    /////////////


    //////////////
    len = snprintf( hs->to_send_buf + hs->to_send_buf_used_len, hs->to_send_buf_len - hs->to_send_buf_used_len,
                                     "Content-type: %s\r\n",
                                     hs->http_resp->mime  );
    //todo assert len musi byt <= ako (hs->to_send_buf_len - hs->to_send_buf_used_len)
    if( ( *( hs->to_send_buf + hs->to_send_buf_used_len + len-2) == '\r' ) &&
        ( *( hs->to_send_buf + hs->to_send_buf_used_len + len-1) == '\n' ) ) {
      hs->to_send_buf_used_len += len;
    }
    //////////////


    //////////////
    len = snprintf( hs->to_send_buf + hs->to_send_buf_used_len, hs->to_send_buf_len - hs->to_send_buf_used_len,
                                     "Content-length: %lu\r\n",
                                     hs->http_resp->file_size  );
    //todo assert len musi byt <= ako (hs->to_send_buf_len - hs->to_send_buf_used_len)
    if( ( *( hs->to_send_buf + hs->to_send_buf_used_len + len-2) == '\r' ) &&
        ( *( hs->to_send_buf + hs->to_send_buf_used_len + len-1) == '\n' ) ) {
      hs->to_send_buf_used_len += len;
    }
    //////////////


    //////////////
    if( (hs->http_resp->stat_code == 200) &&
        (hs->http_req->connection_flags & 0x4) &&
        (hs->http_req->connection_flags ^ 0x2) ) {
      len = snprintf( hs->to_send_buf + hs->to_send_buf_used_len, hs->to_send_buf_len - hs->to_send_buf_used_len,
                                       "Connection: %s\r\n\r\n",
                                       "Keep-Alive"  );
    } else {
      len = snprintf( hs->to_send_buf + hs->to_send_buf_used_len, hs->to_send_buf_len - hs->to_send_buf_used_len,
                                       "Connection: %s\r\n\r\n",
                                       "close"  );
    }
    //todo assert len musi byt <= ako (hs->to_send_buf_len - hs->to_send_buf_used_len)
    if( ( *( hs->to_send_buf + hs->to_send_buf_used_len + len-2) == '\r' ) &&
        ( *( hs->to_send_buf + hs->to_send_buf_used_len + len-1) == '\n' ) ) {
      hs->to_send_buf_used_len += len;
    }
    //////////////
    *(hs->to_send_buf + hs->to_send_buf_used_len) = '\0';
    printf("headers: %s---\n\n\n\a",hs->to_send_buf);

    hs->server_state = WRITE_BODY;
  }

  if(hs->server_state == WRITE_BODY ) {
    u32 can_copy = hs->to_send_buf_len - hs->to_send_buf_used_len;
    can_copy = GET_MIN(can_copy, hs->http_resp->file_size);
    hs->to_send_buf_used_len += fread(hs->to_send_buf + hs->to_send_buf_used_len, 1, can_copy, hs->http_resp->file);
    if(hs->to_send_buf_used_len < can_copy ) {
      //EOF or error
      hs->http_resp->file_size = 0;
      while(1);//FIXME chcem vevidet kedy a ci sa stane pri testovani
    } else {
      hs->http_resp->file_size -= hs->to_send_buf_used_len;
    }
  }
}

/* @return u8 code depending to the size of BODY that we already have in comparison to CONTENT_LENGTH header
 *              HTTPS_PARTIAL - we need to read more data
 *              HTTPS_COMPLETE - we should have full BODY
 *              HTTPS_ERROR - CONTENT_LENGTH is unknown
 *
 */
//u8 parse_input(struct http_state *hs, char *unparsed, u32 unparsed_len) {
//  _psTraceInt("parse_input. input len is: %d. ", unparsed_len);
//  _psTraceStr("input is:\n%s\n", unparsed);
//  if(hs->content_length == 0) {
//    unsigned char ret;
//    ret = wsapi_get_int_header(hs->L, "CONTENT_LENGTH", &hs->content_length);
//    if(ret == 0) {
//      // "CONTENT_LENGTH" was not in the table
//      return HTTPS_ERROR;
//    }
//  }
//
//  /* FIXME spajanie noveho unparsed z predchadzajucim pokial to cle nebude mat
//   * dlzku CONTENT_LENGTH
//   */
//
//  if( hs->to_parse_buf_len < hs->content_length ) {
//    return HTTPS_PARTIAL;
//  } else {
//    return HTTPS_COMPLETE;
//  }
//  //HTTPS_PARTIAL, alebo HTTPS_COMPLETE, alebo nejaky error
//}

typedef void (CoroStartCallback)(void *);
typedef struct CallbackBlock
{
        void *context;
        CoroStartCallback *func;
} CallbackBlock;
static CallbackBlock globalCallbackBlock;

void Coro_StartWithArg(CallbackBlock *block)
{
        (block->func)(block->context);
        printf("Scheduler error: returned from coro start function\n");
        exit(-1);
}


void Coro_Start(void)
{
        CallbackBlock block = globalCallbackBlock;
        Coro_StartWithArg(&block);
}

int32 request_parse(struct http_state *hs)
{
  char *unparsed;
  char *inbuf_data_start = NULL;
  u32 unparsed_len;
  char *crlf;
  int32 ret;

  /* SSL/TLS can provide zero length records, which we just ignore here
   * because the code below assumes we have at least one byte */
  if (hs->buf_len == 0) {
    return HTTPS_PARTIAL;
  }

  if(hs->server_state == BODY) {
    //FIXME: prerobit pre  contentswitch ver 2.
    _psTraceInt("parse_input. input len is: %d. ", hs->buf_len);
    _psTraceStr("input is:\n%s\n", hs->buf);

    /* static page - dummy read all content */
    if(hs->http_resp->file != NULL) {
      // assert: hs->L must be null
      hs->http_req->content_left -= hs->http_req->content_buf_len;
      if(hs->http_req->content_left > 0) {
        return HTTPS_PARTIAL;
      }
      hs->server_state = WRITE_HEADERS;
      hs->http_resp->stat_code = 200;
      hs->http_resp->stat_msg = "OK";
      prepare_response(hs);
      return HTTPS_COMPLETE;
    }

    /* wsapi page: */
    hs->http_req->content_buf = hs->buf;
    hs->http_req->content_buf_len = hs->buf_len;


    if((ret = setjmp(hs->parent)) != 0) {
      // after longjmp and return I can use directly. just HTTPS_ERROR is 0
      return ret;
      //return WRITE; //send_s();
      //return HTTPS_PARTIAL;// pbuf_copy_partial(...) or READ
    }
    longjmp(hs->lua_child, 1);


//    if(setjmp(hs->read_more) == 0) {
//      longjmp(hs->cont_lua_read, unparsed_len);
//    } else {
//    //ret = parse_input(hs, unparsed, unparsed_len);
//      return HTTPS_PARTIAL;
//    }
  }

  if(hs->http_req->to_parse_buf_len > 0) {
    // uz som nejake data precital
    /* nove data prilepim na koniec celeho buffra a poznacim si, kde zacinaju
     * co sa mi zide ak sa budm chciet namapovat na hs->inbuf.(parsovanie POSTU) */
    hs->http_req->to_parse_buf = realloc( hs->http_req->to_parse_buf,
                                          hs->http_req->to_parse_buf_len + hs->buf_len );
    if(hs->http_req->to_parse_buf == NULL) {
      return HTTPS_ERROR;
    }
    inbuf_data_start = hs->http_req->to_parse_buf + hs->http_req->to_parse_buf_len;
    memcpy(inbuf_data_start, hs->buf, hs->buf_len);
    hs->http_req->to_parse_buf_len += hs->buf_len;
    //hs->unparsed_line = hs->to_parse_buf;

    unparsed_len = hs->http_req->to_parse_buf_len;
    unparsed = hs->http_req->to_parse_buf;
  } else {
    unparsed_len = hs->buf_len;
    unparsed = hs->buf;
  }

  //while(unparsed != NULL) {
  while(TRUE) {
    crlf = strnstr(unparsed, "\r\n", unparsed_len);

    if(crlf == NULL) {
      /* nemam dost dekodovanych dat. musim dekodovat viac, ale najskor ulozit to co mam.
       * na tomto mieste ukladam iba prvy krat ked nemam dost miesta. inak ukladam
       * na zaciatku tejto funkcie. TODO: to_parse_buf nemusi byt NULL pri keep-alive, lebo
       * ten pointer zatial pouzivam aj na POST/PUT data a vtedy ho mallocoval matrixSSL */
      //if(hs->to_parse_buf == NULL) { // (hs->parsed_buf_len == 0) {
      if(hs->http_req->to_parse_buf_len == 0) {
        //hs->to_parse_buf = realloc(hs->to_parse_buf, hs->to_parse_buf_len + unparsed_len );
        hs->http_req->to_parse_buf = malloc(hs->http_req->to_parse_buf_len + unparsed_len );
        if(hs->http_req->to_parse_buf == NULL) {
          return HTTPS_ERROR;
        }
        memcpy(hs->http_req->to_parse_buf + hs->http_req->to_parse_buf_len, unparsed, unparsed_len);
        hs->http_req->to_parse_buf_len += unparsed_len;
        //hs->unparsed_line = hs->to_parse_buf;
      }
      //FIXME: uvolnit pamet v ktorej su riadky ktore som uz parsoval
      //else if() {}

      return HTTPS_PARTIAL;
    }

    *crlf = '\0';
    /* konrola ci idem parsovat prvy riadok ale dalsie.
     * tak ide o prve volanie funkcie request_parse, tak su oba pointre NULL.
     */
    if( hs->server_state == FISRT_LINE) {
      // nacitali som cely PRVY riadok
      ret = parse_first_line(hs, unparsed, (u32 ) (crlf - unparsed) );
      if(ret == 200 ) {
//        if(hs->http_req->file != NULL) {
//          // assert: hs->L must be null
//          //TODO: serve static page
//          hs->parsing_state = DUMMY_READ;
//        } else {
          hs->server_state = HEADERS; // serve dynamic page
//        }
      } else if(ret == 404 ) {
        //FIXME poslat korektnu 404 stranku
        return HTTPS_ERROR;
      } else { //if(ret == 400 ) {
        //FIXME poslat korektnu chybovu stranku
        return HTTPS_ERROR;
      }
    } else {
      // parse next request lines
      if( parse_header_line(hs, unparsed) == NULL) {
        // did'n find '=' in reqest line
        if(crlf == unparsed){
          //in the line is only "\r\n"
          hs->server_state = BODY;

          hs->http_req->to_parse_buf_len = 0;
          free(hs->http_req->to_parse_buf);
          hs->http_req->to_parse_buf = NULL;

          unparsed += CRLF_LEN;
          unparsed_len -= CRLF_LEN;
          hs->http_req->content_buf_len = unparsed_len;
          if(hs->http_req->content_buf_len != 0 ) {  // is > 0
            if(unparsed_len <= hs->buf_len) {
              //assert inbuf_data_start nie je NULL
              hs->http_req->content_buf = hs->buf + (unparsed - inbuf_data_start);
            } else {
              // data pre POST nie su iba hs->inbuf nemalo by sa stat
              while(1);
              /* this should never happen. solution would be something like this,
               * but the memory will be allocated forever as I get always get
               * "content_buf" from matrixSLL.
              hs->http_req->content_buf = malloc(hs->http_req->to_parse_buf_len );
              memcpy(hs->http_req->content_buf, unparsed, hs->http_req->to_parse_buf_len);
              free(hs->http_req->to_parse_buf); */
            }
          }

          /* static page - dummy read all content */
          if(hs->http_resp->file != NULL) {
            // assert: hs->L must be null
            hs->http_req->content_left -= hs->http_req->content_buf_len;
            if(hs->http_req->content_left > 0) {
              return HTTPS_PARTIAL;
            }
            hs->server_state = WRITE_HEADERS;
            hs->http_resp->stat_code = 200;
            hs->http_resp->stat_msg = "OK";
            prepare_response(hs);
            return HTTPS_COMPLETE;
          }

          /* wsapi page: */

//          if(setjmp(hs->read_more) != 0) {
//            // executed after calling longjmp(hs->read_more) from http_read(..)
//            return HTTPS_PARTIAL; // read more data
//          }
          // this part will execute just after setjmp()
          // TODO s lua_atpanic postat scripty. pouzitie pozriet v lua-web-tools

          if((ret = setjmp(hs->parent)) != 0) {
            // after longjmp and return I can use directly. just HTTPS_ERROR is 0
            return ret;
            //return WRITE; //send_s();
            //return HTTPS_PARTIAL;// pbuf_copy_partial(...) or READ
          }

          //content switch
          printf("hs->L: %p before switch\n", hs->L);
          set_lua_hs_struct(hs);
          hs->stack_size = 5120;
          if((hs->stack = malloc(hs->stack_size)) == NULL ) {
            printf("low memory\n");
            while(1);
          }
          setjmp(hs->lua_child);
          hs->lua_child[9] = (int)hs->stack + (int)hs->stack_size - 16;
//          globalCallbackBlock.func = wsapi_run;
//          globalCallbackBlock.context = hs->L;
          hs->lua_child[10] = wsapi_run;//(int)Coro_Start;//wsapi_run;
          longjmp(hs->lua_child, (int)hs);

          while(1);
          //wsapi_run(hs->L);
          return HTTPS_COMPLETE;

          /* FIXME if(unparsed_len > strlen("\r\n") )
           * tak mozem spravit nieco ako:
              hs->unparsed_line = crlf + strlen("\r\n"); //zaciatok dalsieho riadku
              unparsed_len = unparsed_len - (hs->unparsed_line - unparsed); // kolko este mam parsovat
              unparsed = hs->unparsed_line; // zacnem parsovat na zaciatku dalsieho riadku
           * ak nie tak mozno zistit ci je v headeroch "CONTENT_LENGTH"
           */

          //ret = parse_input(hs, unparsed, unparsed_len);



          //return ret; //HTTPS_PARTIAL alebo HTTPS_COMPLETE

        }
      }

//      if (TRUE) { //koniec reqestu
//        //FIXME:
//        if (hs->to_parse_buf != NULL) {
//          free(hs->to_parse_buf); hs->to_parse_buf = NULL;
//          hs->to_parse_buf_len = 0;
//        }
//        return HTTPS_COMPLETE;
//      } // else if (something bad) {}
    }

    //hs->unparsed_line = crlf + CRLF_LEN; //zaciatok dalsieho riadku
    //unparsed_len = unparsed_len - (hs->unparsed_line - unparsed); // kolko este mam parsovat
    crlf += CRLF_LEN; // //zaciatok dalsieho riadku
    unparsed_len -=  crlf - unparsed; // kolko este mam parsovat
    unparsed = crlf; // zacnem parsovat na zaciatku dalsieho riadku
    // dalsi krok by mal byt hladanie DALSIEHO konca riadku
  }

}



/******************************************************************************/
/*
	EXAMPLE ONLY - SHOULD NOT BE USED FOR PRODUCTION CODE
 
	Process an HTTP request from a client.
	Very simple - we just print it, and return success.
 	No HTTP validation at all is done on the data.
 */
//int32 httpBasicParse(struct http_state *hs, unsigned char *buf, u32 len)
//{
//	unsigned char	*c, *end, *tmp;
//	int32	l;
//
//	/*
//		SSL/TLS can provide zero length records, which we just ignore here
//		because the code below assumes we have at least one byte
//	*/
//	if (len == 0) {
//		return HTTPS_PARTIAL;
//	}
//
//	c = buf;
//	end = c + len;
///*
//	If we have an existing partial HTTP buffer, append to it the data in buf
//	up to the first newline, or 'len' data, if no newline is in buf.
// */
//	if (hs->to_parse_buf != NULL) {
//		for (tmp = c; c < end && *c != '\n'; c++);
//		/* We want c to point to 'end' or to the byte after \r\n */
//		if (*c == '\n') {
//			c++;
//		}
//		l = (int32)(c - tmp);
//		if (l > HTTPS_BUFFER_MAX) {
//			return HTTPS_ERROR;
//		}
//		hs->to_parse_buf = realloc(hs->to_parse_buf, l + hs->to_parse_buf_len);
//		memcpy(hs->to_parse_buf + hs->to_parse_buf_len, tmp, l);
//		hs->to_parse_buf_len += l;
//		/* Parse the data out of the saved buffer first */
//		c = hs->to_parse_buf;
//		end = c + hs->to_parse_buf_len;
//		/* We've "moved" some data from buf into to_parse_buf, so account for it */
//		buf += l;
//		len -= l;
//	}
//
//L_PARSE_LINE:
//	for (tmp = c; c < end && *c != '\n'; c++);
//	if (c < end) {
//		if (*(c - 1) != '\r') {
//			return HTTPS_ERROR;
//		}
//		/* If the \r\n started the line, we're done reading headers */
//		if (*tmp == '\r' && (tmp + 1 == c)) {
//			if ((c + 1) != end) {
//				_psTrace("HTTP data parsing not supported, ignoring.\n");
//			}
//			if (hs->to_parse_buf != NULL) {
//				free(hs->to_parse_buf); hs->to_parse_buf = NULL;
//				hs->to_parse_buf_len = 0;
//				if (len != 0) {
//					_psTrace("HTTP data parsing not supported, ignoring.\n");
//				}
//			}
//			_psTrace("RECV COMPLETE HTTP MESSAGE\n");
//			return HTTPS_COMPLETE;
//		}
//	} else {
//		/* If parsed_buf is non-null, we have already saved it */
//		if (hs->to_parse_buf == NULL && (l = (int32)(end -tmp)) > 0) {
//			hs->to_parse_buf_len = l;
//			hs->to_parse_buf = malloc(hs->to_parse_buf_len);
//			psAssert(hs->to_parse_buf != NULL);
//			memcpy(hs->to_parse_buf, tmp, hs->to_parse_buf_len);
//		}
//		return HTTPS_PARTIAL;
//	}
//	*(c - 1) = '\0';	/* Replace \r with \0 just for printing */
//	_psTraceStr("RECV PARSED: [%s]\n", (char *)tmp);
//	/* Finished parsing the saved buffer, now start parsing from incoming buf */
//	if (hs->to_parse_buf != NULL) {
//		free(hs->to_parse_buf); hs->to_parse_buf = NULL;
//		hs->to_parse_buf_len = 0;
//		c = buf;
//		end = c + len;
//	} else {
//		c++;	/* point c to the next char after \r\n */
//	}
//	goto L_PARSE_LINE;
//
//	return HTTPS_ERROR;
//}

/******************************************************************************/
