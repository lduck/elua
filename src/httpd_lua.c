/*
 * httpd_lua.c
 *
 *  Created on: Dec 11, 2012
 *      Author: lduck
 */

#include <stdio.h>
//#include <stdlib.h>
#include <string.h>
#include "type.h"

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "lrodefs.h"

#include "apps/app.h"

static void error_message(lua_State *L, const char *message) {
  if(L==NULL){
    fprintf(stderr, "%s\n", message);
    fflush(stderr);
  } else {
    fprintf(stderr, "%s\n%s", message, lua_tostring(L, -1) );
    fflush(stderr);
  }
}

static int http_out (lua_State *L) {
  error_message(L, lua_tostring(L, -1));
  return 0;
}


/*
 * Cookie write function for Apache requests.
 */
static ssize_t cookie_write (void *cookie, const char *buf, size_t size) {
        lua_State *L;
        L = (lua_State *) cookie;
        fprintf(stderr, "cookie_write start:");
        error_message(L,  buf);
        fprintf(stderr, "cookie_write end\n");
        return size;
}

/*
 * Cookie IO functions for Apache request output.
 */
static cookie_io_functions_t out_io_functions = {
        NULL,
        cookie_write,
        NULL,
        NULL
};

#include <errno.h>
/*
 * Closes the Lua filehandle of a cookie file.
 */
static int filehandle_close (lua_State *L) {
        FILE **fp;
        FILE *f;

        fp = (FILE **) luaL_checkudata(L, 1, LUA_FILEHANDLE);
        f = *fp;
        if (fclose(f)) {
                int save_errno = errno;
                lua_pushnil(L);
                lua_pushfstring(L, "%s", strerror(save_errno));
                lua_pushinteger(L, save_errno);
                return 3;
        }
        *fp = NULL;

        lua_pushboolean(L, 1);
        return 1;
}


const LUA_REG_TYPE flibb[] = {
  {LSTRKEY("output"), LFUNCVAL(http_out)},
  {LNILKEY, LNILVAL}
};
//const LUA_REG_TYPE flib[] = {
//    {LSTRKEY("close"), LFUNCVAL(io_close)},
//    {LSTRKEY("flush"), LFUNCVAL(f_flush)},
//    {LSTRKEY("lines"), LFUNCVAL(f_lines)},
//    {LSTRKEY("read"), LFUNCVAL(f_read)},
//    {LSTRKEY("seek"), LFUNCVAL(f_seek)},
//    {LSTRKEY("setvbuf"), LFUNCVAL(f_setvbuf)},
//    {LSTRKEY("write"), LFUNCVAL(f_write)},
//    {LSTRKEY("__gc"), LFUNCVAL(io_gc)},
//    {LSTRKEY("__tostring"), LFUNCVAL(io_tostring)},
//  #if LUA_OPTIMIZE_MEMORY > 0
//    {LSTRKEY("__index"), LROVAL(flib)},
//  #endif
//    {LNILKEY, LNILVAL}
//  };

static unsigned char wsapi_env_get_value(lua_State *L, const char *key){
  if (key == NULL) {
    return 0;  /* error */
  }
  lua_getfield(L, LUA_REGISTRYINDEX, "WSAPI_ENV");  /* get registry.WSAPI_ENV */
  if (!lua_istable(L, -1)) { /* exists and it is table? */
    lua_pop(L, 1); /* remove WSAPI_ENV from stack */
    return 0;  /* error */
  }
  lua_pushlstring(L, key, strlen(key));
  lua_gettable(L, -2); /* if key is not in the table it will call "wsapi_env_index()" function */
  return 1;
}

static const char *wsapi_get_str_header(lua_State *L, const char *key){
  if(wsapi_env_get_value(L, key) == 0) {
    return NULL;  /* error */
  }
  const char *value;
  if (lua_isstring(L, -1)) {
    value = lua_tostring(L, -1);
    lua_pop(L, 1); /* remove from stack WSAPI_ENV */
    return value;
  } else {
    return NULL;  /* value is not of string type */
  }
}

unsigned char wsapi_get_int_header(lua_State *L, const char *key, u32 *value){
  if(wsapi_env_get_value(L, key) == 0) {
    return 0;  /* error */
  }
  if (lua_isnumber(L, -1)) {
    *value = (u32) lua_tonumber(L, -1);
    lua_pop(L, 1); /* remove from stack WSAPI_ENV */
    return 1;
  } else {
    if (lua_isstring(L, -1)) {
      const char *str_value = lua_tostring(L, -1);
      if( strlen(str_value)==0 ) { /* so the string is "". maybe result of "wsapi_env_index()" function */
        *value = 0;
        return 1;
      }
    }
    lua_pop(L, 2); /* remove from stack WSAPI_ENV and the non-integer value */
    return 0;  /* error */
  }
}


void wsapi_add_str_header(lua_State *L, const char *key, const char *value){
  if (key == NULL) {
    return;  /* error */
  }
  lua_getfield(L, LUA_REGISTRYINDEX, "WSAPI_ENV");  /* get registry.WSAPI_ENV */
  if (!lua_istable(L, -1)) { /* exists and it is table? */
    lua_pop(L, 1); /* remove WSAPI_ENV from stack */
    return;  /* error */
  }

  if (value == NULL) {
    lua_pushliteral(L, "");
  } else {
    lua_pushlstring(L, value, strlen(value));
  }
  lua_setfield(L, -2, key);
  lua_pop(L, 1); /* remove WSAPI_ENV from stack */
}


int wsapi_env_index(lua_State *L) {
  const char *key = luaL_checkstring(L, -1);
  printf("wsapi_env_index: '%s'\n", key);
  lua_pushstring(L, "");
  return 1;
}

void wsapi_create_env(lua_State *L) {
//  luaL_newmetatable(L, "WSAPI_ENV");
//  lua_pushcfunction(L, wsapi_env_index);
//  lua_setfield(L, -2, "__index");
//  lua_setglobal(L, "WSAPI_ENV"); //for now set is as global variable

  luaL_newmetatable(L, "WSAPI_ENV");
  lua_createtable(L, 0, 1);
  lua_pushcfunction(L, wsapi_env_index);
  lua_setfield(L, -2, "__index");
  lua_setmetatable(L, -2);
//  lua_setglobal(L, "WSAPI_ENV"); //for now set is as global variable

//  lua_createtable(L, 0, 0);
//  lua_createtable(L, 0, 1);
//  lua_pushcfunction(L, wsapi_env_index);
//  lua_setfield(L, -2, "__index");
//  lua_setmetatable(L, -2);
//  lua_setglobal(L, "WSAPI_ENV");

  lua_pop(L, 1); /* remove WSAPI_ENV from stack */
}


lua_State * wsapi_prepare() {
  //int status;
  lua_State *L = lua_open();            /* create state */

  if (L == NULL) {
    error_message(L, "cannot create Lua state: not enough memory");
    return NULL;
  }
  lua_gc(L, LUA_GCSTOP, 0);             /* stop collector during initialization */
  luaL_openlibs(L);                     /* open libraries */
  lua_gc(L, LUA_GCRESTART, 0);

  wsapi_create_env(L);

  return L;
}



u8 wsapi_run (struct http_state *hs) {//lua_State *L) {
  printf("hs->L: %p AFTER switch\n", hs->L);

  //lua_State *L = wsapi_prepare();


  //const char line[] = "print '<html><body>'";
  //status = luaL_loadbuffer(L, line, 20, line);
  //if( luaL_loadfile(L, "/mmc/wsapi/cgi.lc") ) { /* Load but don't run the Lua script */

//  lua_getglobal(L, "require");
//    lua_pushstring(L, hello);
//    lua_getglobal(L, "require");
//      lua_pushstring(L, name);
//      return report(L, docall(L, 1, 1));

  char * script_name;
  printf("hs->http_req->method: %d, %d\n", hs->http_req->method, HTTP_METHOD_PUT );
  if(hs->http_req->method == HTTP_METHOD_PUT ) {
    script_name = "/mmc/wsapi/put_request.lc";
  } else {
    script_name = wsapi_get_str_header(hs->L, "SCRIPT_NAME");
  }

  if( luaL_loadfile(hs->L, script_name) ) { /* Load but don't run the Lua script */
//  if( luaL_loadfile(L, "/mmc/tests/hello.lc") ) { /* Load but don't run the Lua script */
  //if( luaL_loadfile(L, "/mmc/tests/helloscript.lua") ) { /* Load but don't run the Lua script */
    error_message(hs->L, "luaL_loadfile() failed");
    while(1);
  }
  if (lua_pcall(hs->L, 0, 0, 0)) {          /* PRIMING RUN. FORGET THIS AND YOU'RE TOAST */
    error_message(hs->L, "lua_pcall() failed");     /* Error out if Lua file has an error */
  }




//  int type = lua_type(L, 1);
//  printf("type:%d", type);

//  lua_getglobal(L, "hello");      /*hello.lc->run() */
//  if (lua_pcall(L, 0, 0, 0)) {          /* register module */
//      error_message(L, "lua_pcall() failed");     /* Error out if Lua file has an error */
//    }
//  lua_getfield(L, -1, "run");      /*hello.lc->run() */
//  int hello_run_base = lua_gettop(L);
//  if (lua_pcall(L, 0, 0, 0)) {          /* Run the function */
//    error_message(L, "lua_pcall() failed");     /* Error out if Lua file has an error */
//  }
//
////  type = lua_type(L, 1);
////  printf("type:%d", type);
//
  if( luaL_loadfile(hs->L, "/mmc/wsapi/common.lc") ) { /* Load but don't run the Lua script */
    error_message(hs->L, "luaL_loadfile() failed");
    while(1);
  }

  if (lua_pcall(hs->L, 0, 0, 0)) {          /* PRIMING RUN. FORGET THIS AND YOU'RE TOAST */
    error_message(hs->L, "lua_pcall() failed");     /* Error out if Lua file has an error */
    while(1);
  }

//  FILE *f;

//
//  /* create file environment */
// lua_newtable(L);
// lua_pushcfunction(L, filehandle_close);
// lua_setfield(L, -2, "__close");

  /* register request file */
//  f = fopencookie(L, "r", out_io_functions);
//  *((FILE **) lua_newuserdata(L, sizeof(FILE *))) = f;
//  luaL_getmetatable(L, LUA_FILEHANDLE);
//  lua_setmetatable(L, -2);
//  lua_pushvalue(L, -2);
//  lua_setfenv(L, -2);
//  lua_setfield(L, -3, "input");

  /* register response file */
//  f = fopencookie(L, "w", out_io_functions);
//  setvbuf(f, NULL, _IONBF, 0);
//  *((FILE **) lua_newuserdata(L, sizeof(FILE *))) = f;
//  luaL_getmetatable(L, LUA_FILEHANDLE);
//  lua_setmetatable(L, -2);
//  lua_pushvalue(L, -2);
//  lua_setfenv(L, -2);
//  lua_setfield(L, -2, "output");  //povodne bola -3, ale s tym to nefunguje

  /* pop file environment */
  //lua_pop(L, 1);


  //lua_State *L = hs->L;

//  luaL_rometatable(L, LUA_FILEHANDLE, (void*)flib);
//  luaL_register_light
//  lua_pushlightfunction

//  luaL_rometatable(L, "http", (void*)flib);
//  lua_getglobal(L, "http");
  lua_getglobal(hs->L, "wsapi_loader");

  lua_getglobal(hs->L, "io");      //wsapi_loader, io

  lua_newtable(hs->L);             // wsapi_loader", io, t
  lua_pushliteral(hs->L, "input"); // wsapi_loader", io, t, input,
  //lua_getfield(hs->L, -3, "stdin");  //t = {input = io.stdin}
  lua_getfield(hs->L, -4, "http_read");
  lua_rawset(hs->L, -3);    // t = {input = io.stdin}


//  lua_pushliteral(L, "output");
//  /* register request file */
//  f = fopencookie(L, "r", out_io_functions);
//  *((FILE **) lua_newuserdata(L, sizeof(FILE *))) = f;
//  luaL_getmetatable(L, LUA_FILEHANDLE);
//  lua_setmetatable(L, -2);
//  lua_pushvalue(L, -2);
//  lua_setfenv(L, -2);
//  lua_rawset(L, -3);



  lua_pushliteral(hs->L, "output");
//  lua_getfield(L, -3, "stdout");
  lua_getfield(hs->L, -4, "output");
//  lua_pushlightfunction(L, http_out);
  lua_rawset(hs->L, -3);    // t = {input = io.stdin, output = io.stdout }

  lua_pushliteral(hs->L, "error");
  lua_getfield(hs->L, -3, "stderr");
  lua_rawset(hs->L, -3);    // t = {input = io.stdin, output = io.stdout, error = io.stderr }

  lua_pushliteral(hs->L, "env");

/* jeden so sposobov pridania zaznamov do tabulky */
  luaL_getmetatable(hs->L, "WSAPI_ENV");

/* jeden so sposobov pridania zaznamov do tabulky
  lua_newtable(L);      // my_env
  lua_pushliteral(L, "SCRIPT_NAME");
  lua_pushliteral(L, "tests/hello.lc");
  lua_rawset(L, -3);    // my_env = { SCRIPT_NAME = "tests/hello.lc" }
*/

  lua_rawset(hs->L, -3);    // t = {input = io.stdin, output = io.stdout, error = io.stderr,  env = my_env}

  // TODO: tu uz mozem zmazat prvy zaznam ("io") zo  stacku

  lua_getglobal(hs->L, "hello");

  lua_getglobal(hs->L, "wsapi");
  lua_getfield(hs->L, -1, "common");
  lua_getfield(hs->L, -1, "run");   /*common.lc->run() */

  lua_getfield(hs->L, -4, "run");   /*hello.lc->run() */
  lua_pushvalue(hs->L, -6);

  if (lua_pcall(hs->L, 2, 0, 0)) {          /* Run the function */
    error_message(hs->L, "lua_pcall() failed");     /* Error out if Lua file has an error */
    while(1);
  }

  hs->stack_size = 0;
  free(hs->stack);
  hs->stack = NULL;
  // after setjmp
  longjmp(hs->parent, HTTPS_COMPLETE);

//  if( luaL_loadfile(L, "/mmc/tests/vykonanie.lua") ) { /* Load but don't run the Lua script */
//    error_message(L, "luaL_loadfile() failed");
//  }
//
//  if (lua_pcall(L, 0, 0, 0)) {          /* PRIMING RUN. FORGET THIS AND YOU'RE TOAST */
//    error_message(L, "lua_pcall() failed");     /* Error out if Lua file has an error */
//  }
//
//  lua_getglobal(L, "hello");
//
//  lua_getglobal(L, "vykonanie");
//  lua_getfield(L, -1, "run");      /*hello.lc->run() */
//
//  lua_getfield(L, -3, "square");
//  lua_getfield(L, -4, "value");
//  if (lua_pcall(L, 2, 0, 0)) {          /* Run the function */
//    error_message(L, "lua_pcall() failed");     /* Error out if Lua file has an error */
//  }
//
//  lua_getglobal(L, "hello");
//  //lua_getglobal(L, "square");                 /* Tell it to run callfuncscript.lua->square() */
//  lua_getfield(L, -1, "square");                 /* Tell it to run callfuncscript.lua->square() */
//      lua_pushnumber(L, 6);                       /* Submit 6 as the argument to square() */
//      if (lua_pcall(L, 1, 1, 0))                  /* Run function, !!! NRETURN=1 !!! */
//        error_message(L, "lua_pcall() failed");
//
//  lua_getglobal(L, "runn");      /*hello.lc->run() */
//  if (lua_pcall(L, 0, 0, 0)) {          /* Run the function */
//    error_message(L, "lua_pcall() failed");     /* Error out if Lua file has an error */
//  }

  //report(L, status);

  //lua_close(L);

  //return (status || s.status) ? EXIT_FAILURE : EXIT_SUCCESS;
  return 0;
}
