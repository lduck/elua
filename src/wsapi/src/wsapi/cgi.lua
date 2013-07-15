-----------------------------------------------------------------------------
-- CGI WSAPI handler
--
-- Author: Fabio Mascarenhas
-- Copyright (c) 2007 Kepler Project
--
-----------------------------------------------------------------------------

--local os = require"os"
local io = require"io"
local common = require"wsapi.common"

--require("modedebug").start()

local function normalize_paths(wsapi_env, filename)
   -- toto spravim pri vytvarani ENV
   wsapi_env.PATH_TRANSLATED = filename
   wsapi_env.SCRIPT_FILENAME = filename

  --toto ostatne treba iba v pripade ak budem davat PATH_INFO ktore bude rovnake (rovnako zacinat ) ako SCRIPT_NAME
   local s, e = wsapi_env.PATH_INFO:find(wsapi_env.SCRIPT_NAME, 1, true)
   --print("normalize_paths: " .. wsapi_env.PATH_INFO)
   if s == 1 then
     wsapi_env.PATH_INFO = wsapi_env.PATH_INFO:sub(e+1)
     if wsapi_env.PATH_INFO == "" then wsapi_env.PATH_INFO = "/" end
   end
end

--rozdely text podla posledneho lomitka ("/"). napr:
-- "/a/b.lua" na "/a", "b.lua"
-- "/a/b/" na "/a/b", ""     CO JE NEPOUZITELNE v povodnom common.lua. fatfs ma tiez problem s / na konci
-- "/a/b" na "/a", "b"       co je dobre pre adresare
local function splitpath(filename)
  local path, file = string.match(filename, "^(.*)[/\\]([^/\\]*)$")
  return path, file
end

-- rozdeli text na meno a priponu podla bodky. respektuje aj bodku na zaciatku, pripadne viacero bodiek, napr:
-- ".aa.b.lua" na ".aa.b", "lua"
local function splitext(filename)
  local modname, ext = string.match(filename, "^(.+)%.([^%.]+)$")
  if not modname then modname, ext = filename, "" end
  return modname, ext
end

-- Gets the data for file or directory "filename" if it exists:
-- path, actual file name, file name without extension, extension,
-- and modification time. If "filename" is a directory it assumes
-- that the actual file is a .lua file in this directory with
-- the same name as the directory (for example, "/foo/bar/bar.lua")
local function find_file(filename)
   --local mode = assert(fatfs.file_mode(filename))
   local mode = "file"
   local path, file, modname, ext
   if mode == "file" then
      path, file = splitpath(filename)
      modname, ext = splitext(file)
   elseif mode == "directory" then
      path, modname = splitpath(filename)
      path = path .. "/" .. modname
      file = modname .. ".lua"
      ext = "lua"
   else
      return nil
   end
   --local mtime = assert(lfs.attributes(path .. "/" .. file, "modification"))
   --local mtime = nil
   return path, file, modname, ext --, mtime
end


local function l_find_module(wsapi_env, filename, launcher, vars)
   normalize_paths(wsapi_env, filename or "", launcher, vars)
   return find_file(wsapi_env.PATH_TRANSLATED)
end

local function l_load_wsapi(path, file, modname, ext)
  print("load_wsapi(path, file, modname, ext)")
  --lfs.chdir(path)
  --fatfs.chdir(path)
  local app
  if ext == "lc" then
    app = common.require_file(file, modname)
  else
    app = dofile(file)
  end
  return common.normalize_app(app)
end


--common.setmode()

--module(...)

local arg_filename = "/Users/lduck/projects/dp/elua/src/wsapi/src/hello.lua"
local my_env = {
  GATEWAY_INTERFACE="CGI/1.1",
REMOTE_ADDR="192.168.1.102",
SCRIPT_NAME="/Users/lduck/projects/dp/elua/src/wsapi/src/hello.lua"
--PATH_INFO="/mmc/w/cgi-bin/hello.lua"
  }

-- Runs an WSAPI application for this CGI request
function run(app_run)
   common.run(app_run, { input = io.stdin, output = io.stdout,
     error = io.stderr, env = my_env }) --os.getenv })
end


local function wsapi_loader(wsapi_env)
  local path, file, modname, ext = --'/mmc/wsapi_ori', 'common.lua', 'common', 'lua'
    --common.find_module(wsapi_env, arg_filename, "wsapi.cgi", { "SCRIPT_FILENAME", "PATH_TRANSLATED" })
    l_find_module(wsapi_env, arg_filename, "wsapi.cgi", { "SCRIPT_FILENAME", "PATH_TRANSLATED" })
    --
  if not path then
    error({ 404, "Resource " .. wsapi_env.SCRIPT_NAME .. " not found" })
  end
  wsapi.app_path = path
  local app = l_load_wsapi(path, file, modname, ext)
  wsapi_env.APP_PATH = path
  return app(wsapi_env)
end 

run(wsapi_loader)
