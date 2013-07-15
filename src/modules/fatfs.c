/*
 * fatfs.c
 *
 *  Created on: Dec 4, 2012
 *      Author: lduck
 */

#include "lua.h"
#include "lauxlib.h"
#include "lrotable.h"
#include <string.h>
#include <stdio.h>

#include "ff.h"


static void find_file( char *file, char **path, char **ext );

static int fatfs_chdir( lua_State *L )
{
  const char *path = luaL_checkstring(L, 1);

  char *file=NULL;
  char *ext = NULL;
//  find_file(path, &file, &ext);
//  printf("fatfs.find_file(path:'%s'): file:'%s', ext: '%s'\n", path, file, ext);

  if(chdir(path)==0) {
    printf("fatfs.chdir('%s')\n", path);
    lua_pushboolean (L, 1);
    return 1;
  } else {
    lua_pushnil (L);
    lua_pushfstring (L, "Unable to change working directory to '%s'\n", path);
    return 2;
  }

//  printf("fatfs.chdir('%s')\n", path);
//  // remove ending slash. for fatFs "directory/" is not the same as "directory"
//  if( path[strlen(path)-1] == '/') {
//    char *p = (char *) path + strlen(path) -1;
//    *p = '\0';
//    printf("oprava: fatfs.chdir('%s')\n", path);
//  }
//
//  FRESULT res = f_chdir(path);
//  if( res != FR_OK ) {
//    lua_pushnil (L);
//    lua_pushfstring (L,"Unable to change working directory to '%s'\nerror code: %d (check FRESULT enum)\n",
//                    path, res);
//    return 2;
//  }
//  lua_pushboolean (L, 1);
//  return 1;
}

static int fatfs_file_mode( lua_State *L )
{
  FILINFO file_struct;
  FRESULT res;
  const char *file = luaL_checkstring (L, 1);

  printf("fatfs.file_mode('%s')\n", file);
  if( file[strlen(file)-1] == '/') {
    //FIXME: vraj by som nemal menit string ked som sa k nemu dostal cez "const char *"
    char *p = (char *) file + strlen(file) -1;
    *p = '\0';
    printf("oprava: fatfs.file_mode('%s')\n", file);
  }


//  if( strncmp(*file, "/mmc", 4) != 0 ) { // not FatFs path
//    return 0;
//  }

  res = f_stat(file, &file_struct);
  if( res == FR_OK ) {
    if(file_struct.fattrib & AM_DIR) {
      lua_pushstring( L, "directory");
    } else {
      lua_pushstring( L, "file");
    }
    return 1;
  }
  return 0;
}


/* FIXME: presunut do samostatneho modulu. zatial som to dal sem, ale chcelo by
 * to druhy subor alebo tento premenovat
 */

/* rozdely text podla posledneho lomitka ("/"). napr:
 * "/a/b.lua" na "/a",   "b.lua"
 * "/a/b"     na "/a",   "b"    co je dobre pre adresare
 * "/a/b/"    na "/a/b", ""     CO JE NEPOUZITELNE v povodnom common.lua. fatfs ma tiez problem s / na konci
 * "a"        na "", "a"        aj ked by som tomu nemal davat cestu bez lomitka
 */
static char* split_path( char **path) {
  char *s2 = *path;
  char *last_slash, *s1;

  while( s2 != NULL ) {
    s1 = last_slash;
    last_slash = s2;
    s2++;
    s2 = strchr(s2, '/');
  }
  if(last_slash == *path) { // no slash in path. SHOULD NOT HAPPEN
    *path = last_slash + strlen(last_slash);
    return last_slash;
  }

  if( (last_slash - *path) == strlen(*path) ) {  //posledny znak je "/"
    *s1 = '\0';
    last_slash = s1 + 1;
  } else {
    *last_slash = '\0';
    last_slash++;
  }
  return last_slash;
}

/* najde v mene suboru priponu podla poslednej bodky ("."). ak je bodka posledny
 * znak, alebo v mene nie je bodka tak subor nema priponu. napr:
 * ".aa.b.lua"  na ".aa.b",     "lua"
 * ".aa.b.lua." na ".aa.b.lua", ""
 */
static char* find_ext( char *file ) {
  char *ext;
  char *s1 = file;
  while( s1 != NULL ) {
    ext = s1;
    s1++;
    s1 = strchr(s1, '.');
  }
  if( ext == file ) { //no dot in filename
    ext = file + strlen(file);
    return ext;
  }
  ext++;
  return ext;
}

static void find_file( char *path, char **file, char **ext ) {
//  if( strncmp(*file, "/mmc", 4) != 0 ) { // not FatFs path
//    return 0;
//  }
//  file = file + 4;

  FILINFO file_struct;
  FRESULT res;
  //char *path, *modname, *ext;
  res = f_stat(path, &file_struct);
  if( res == FR_OK ) {
    if( !(file_struct.fattrib & AM_DIR) ) { // file
    // mozno to je aj  if( file_struct.fattrib ~ AM_DIR ) { // file
      *file = split_path(&path);
      *ext = find_ext(*file);
    }
  }
}

//static void test() {
//  char *ext;
//  ext = splitext(".a");
//  printf("splitext(%s): %s\n", ".a", ext);
//  ext = splitext("a");
//  printf("splitext(%s): %s\n", "a", ext);
//  ext = splitext("a.");
//  printf("splitext(%s): %s\n", "a.", ext);
//  ext = splitext("a.lua");
//  printf("splitext(%s): %s\n", "a.lua", ext);
//  ext = splitext("a.b.lua");
//  printf("splitext(%s): %s\n", "a.b.lua", ext);
//
//  char path[10] = "/a/b.lua";
//  char *p;
//  p = &path[0];
//  printf("splithpath(%s)", path);
//  fflush(stdout);
//  ext = splithpath(&p);
//  printf("\nfilename: %s\n", ext);
//  fflush(stdout);
//  printf("new_path: %s\n ", p);
//
//  p = &path[0];
//  strcpy(p, "/a/b");
//  ext = splithpath(&p);
//  printf("\nfilename: %s\n", ext);
//  printf("new_path: %s\n ", p);
//
//  p = &path[0];
//  strcpy(p, "/a/b/");
//  ext = splithpath(&p);
//  printf("\nfilename: %s\n", ext);
//  printf("new_path: %s\n ", p);
//
//  p = &path[0];
//  strcpy(p, "a");
//  ext = splithpath(&p);
//  printf("\nfilename: %s\n", ext);
//  printf("new_path: %s\n ", p);
//}








#define MIN_OPT_LEVEL  2 // the minimum optimization level at which we use rotables
#include "lrodefs.h"
const LUA_REG_TYPE fatfs_map[] =
{
  { LSTRKEY( "chdir" ), LFUNCVAL( fatfs_chdir ) },
  { LSTRKEY( "file_mode" ), LFUNCVAL( fatfs_file_mode ) },
  { LNILKEY, LNILVAL }
};
// note: no more LRO_something, just Lsomething (for example LRO_STRKEY becomes LSTRKEY)

LUALIB_API int luaopen_fatfs( lua_State *L )
{
  LREGISTER( L, "fatfs", fatfs_map ); // note: no more luaL_register, no "return 1"
}
