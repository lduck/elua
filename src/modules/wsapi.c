/*
 * wsapi.c
 *
 *  Created on: Dec 19, 2012
 *      Author: lduck
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <setjmp.h>

#include "lua.h"
#include "lauxlib.h"
#include "lrotable.h"

#include "wsapi.h"
#include "apps/app.h"


/*
 * Set the "http_state" struct to a Lua state under LUA_REGISTRYINDEX.
 */
void set_lua_hs_struct (struct http_state *hs) {
  if(hs->L == NULL) {
    return;
  }
  printf("saving hs: %p to lua state\n", hs);
  lua_pushlightuserdata(hs->L, (void *) hs);  /* push address */
  lua_setfield(hs->L, LUA_REGISTRYINDEX, HTTP_STATE_STRUCT);
}


/*
 * Returns the "http_state" struct from a Lua state.
 */
struct http_state *get_lua_hs_struct (lua_State *L) {
  struct http_state *hs;

  lua_getfield(L, LUA_REGISTRYINDEX, HTTP_STATE_STRUCT);
  if (!lua_islightuserdata(L, -1)) {
    lua_pop(L, 1);
    return NULL;
  }
  hs = (struct http_state *) lua_touserdata(L, -1);

  //printf("lading hs: %p from lua state\n", hs);

  lua_pop(L, 1);
  return hs;

//  /* variable with an unique address */
//  static const char Key = 'k';
//
//  /* store a number */
//  lua_pushlightuserdata(L, (void *)&Key);  /* push address */
//  lua_pushnumber(L, myNumber);  /* push value */
//  /* registry[&Key] = myNumber */
//  lua_settable(L, LUA_REGISTRYINDEX);
//
//  /* retrieve a number */
//  lua_pushlightuserdata(L, (void *)&Key);  /* push address */
//  lua_gettable(L, LUA_REGISTRYINDEX);  /* retrieve value */
//  myNumber = lua_tonumber(L, -1);  /* convert to number */

}


static void error_message(lua_State *L, const char *message) {
  if(L==NULL){
    fprintf(stderr, "%s\n", message);
    fflush(stderr);
  } else {
    fprintf(stderr, "%s:\n%s", message, lua_tostring(L, -1) );
    fflush(stderr);
  }
}

static int http_out (lua_State *L) {
  struct http_state *hs = get_lua_hs_struct(L);
  uint32 lua_buf_len;

  //FIXME: skontrolovat ci na vstupe nie je int
  const char *lua_buf = lua_tolstring(L, -1, (size_t *) &lua_buf_len);
  fprintf(stderr, "lua_buf_len je: %d\n", lua_buf_len);
  //fprintf(stderr, "%s", lua_buf);

  while(TRUE) {
    if(hs->to_send_buf==NULL) {
      if ((hs->to_send_buf_len = matrixSslGetWritebuf(hs->ssl, &hs->to_send_buf, lua_buf_len) ) < 0 ) {
        //http_close_conn(pcb, hs);
        //return CONNECTION_CLOSED;
        fprintf(stderr, "Ukoncit pre nedostatok pamate");
        fflush(stderr);
        while(1);//FIXME ukoncit pre nedostatok pamate
      }
      hs->to_send_buf_len--; //FIXME nahlasit chybu. toto je hotfix
      hs->to_send_buf_used_len = 0;
  //    if(hs->to_send_buf_len >= lua_buf_len) {
  //      memcpy(hs->to_send_buf, lua_buf, lua_buf_len);
  //      hs->to_send_buf_copied_len = lua_buf_len;
  //    }
    }
    int32 can_copy = hs->to_send_buf_len - hs->to_send_buf_used_len;
    can_copy = GET_MIN(can_copy, lua_buf_len);
    fprintf(stderr, "to_send_buf_len je: %d\t to_send_buf_copied_len:%d, can_copy:%d\n", hs->to_send_buf_len, hs->to_send_buf_used_len, can_copy);
    memcpy(hs->to_send_buf + hs->to_send_buf_used_len, lua_buf, can_copy);
    hs->to_send_buf_used_len += can_copy;

    if(can_copy == lua_buf_len) {
      return 0; //we fit to hs->to_send_buf_len
    }

    /* TODO v pripade ak som sa zmestil ale uz je hs->to_send_buf plny tak chcem
     * mozno zapisat do siete a potom sa vratit do lua scriptu
     */
    lua_buf += can_copy;
    lua_buf_len -= can_copy;
    fprintf(stderr, "matrixSslEncodeWritebuf(..,hs->to_send_buf_copied_len:%d)\n", hs->to_send_buf_used_len);
    if( matrixSslEncodeWritebuf(hs->ssl, hs->to_send_buf_used_len ) < 0 ) {
      //http_close_conn(pcb, hs);
      //return CONNECTION_CLOSED;
      fprintf(stderr, "Ukoncit pre nedostatok pamate");
      fflush(stderr);
      while(1);//FIXME ukoncit pre nedostatok pamate
    }
    //write,sleep
    //if(hs->to_send_buf_len == hs->to_send_buf_copied_len) {
      // buffer from matrixSslGetWritebuf(..) is full
    printf("Frame 0: PC=%p\n", __builtin_return_address(0));
    printf("Frame 0: SP=%p\n", __builtin_frame_address(0));

      if(setjmp(hs->lua_child) == 0) {
        // after setjmp
        longjmp(hs->parent, WRITE);//hs->to_send_buf_copied_len);
      }
      // after longjmp(hs->cont_lua_write, ..)
      hs->to_send_buf = NULL;
      hs->to_send_buf_used_len = 0;
    //}
  }


//  if(hs->to_send_buf==NULL) {
//    //FIXME: kontrola ci malloc a realloc neviracia NULL
//    hs->to_send_buf = malloc(lua_buf_len);
//    memcpy(hs->to_send_buf, lua_buf, lua_buf_len);
//    hs->to_send_buf_len = lua_buf_len;
//  } else {
//    hs->to_send_buf = realloc(hs->to_send_buf, lua_buf_len + hs->to_send_buf_len );
//    memcpy(hs->to_send_buf + hs->to_send_buf_len, lua_buf, lua_buf_len);
//    hs->to_send_buf_len += lua_buf_len;
//  }


  return 0;
}

static int http_read (lua_State *L) {
  //we accept just int arg, which says how many chars we will read
  if (lua_type(L, -1) != LUA_TNUMBER) {
    const char *msg = lua_pushfstring(L, "bad argument type. INT expected, got %s",
                                          luaL_typename(L, -1));
    return luaL_argerror(L, -1, msg);
  }
  size_t read_len = (size_t) lua_tointeger(L, -1);

  struct http_state *hs = get_lua_hs_struct(L);
//  if(hs->content_length == 0) {
//    if(wsapi_get_int_header(hs->L, "CONTENT_LENGTH", &hs->content_length) == 0) {
//      // "CONTENT_LENGTH" was not in the table
//      const char *msg = lua_pushfstring(L, "HTTP request does not have CONTENT_LENGTH header");
//      return luaL_argerror(L, -1, msg);
//    }
//  }


  /* FIXME: 30. 1. 2013, aspon do dokumentacie/Implementacie treba napisat
   * "performance improvement" pre luaL_addlstring co je v linku:
   * http://lua-users.org/lists/lua-l/2010-03/msg00374.html
   */

  luaL_Buffer b;
  luaL_buffinit(L, &b);
  //char *p = luaL_prepbuffer(&b);

//  char *str = (char *) malloc( sizeof(char) * 10);
//  sprintf(str, "1234\\05678");

  if(hs->http_req->content_buf_len >= read_len) {
    luaL_addlstring(&b,hs->http_req->content_buf, read_len);
    hs->http_req->content_buf += read_len;
    hs->http_req->content_buf_len -= read_len;
    hs->http_req->content_left -= read_len;
  } else {
    do { //FIXME should not try to read more than hs->content_length
      luaL_addlstring(&b, hs->http_req->content_buf, hs->http_req->content_buf_len);
      read_len -= hs->http_req->content_buf_len;
      hs->http_req->content_buf_len = 0;
      // a nacitat viac
      if(setjmp(hs->lua_child) == 0) {
        // after setjmp
        longjmp(hs->parent, HTTPS_PARTIAL);//hs->to_send_buf_copied_len);
      }
//      while(1);
//      setjmp(hs->cont_lua_read);
//      longjmp(hs->read_more, read_len);
    } while(read_len > 0);
  }

//char *p = luaL_prepbuffer(&b);
//  memcpy(p, str, read_len);
//  luaL_addsize(&b, read_len);
  luaL_pushresult(&b);  /* close buffer */
  //const char *s = lua_tostring(L, -1);
  //printf("http_read:%s",s);
  //success = (read_len == 0) ? test_eof(L, f) : read_chars(L, f, read_len);
  return 1;
}

#define MIN_OPT_LEVEL  2 // the minimum optimization level at which we use rotables
#include "lrodefs.h"
const LUA_REG_TYPE wsapi_map[] =
{
  { LSTRKEY( "output" ), LFUNCVAL( http_out ) },
  { LSTRKEY( "http_read" ), LFUNCVAL( http_read ) },
//  { LSTRKEY( "input" ), LFUNCVAL( fatfs_file_mode ) },
  { LNILKEY, LNILVAL }
};
// note: no more LRO_something, just Lsomething (for example LRO_STRKEY becomes LSTRKEY)

LUALIB_API int luaopen_wsapi( lua_State *L )
{
  LREGISTER( L, "wsapi_loader", wsapi_map ); // note: no more luaL_register, no "return 1"
}
