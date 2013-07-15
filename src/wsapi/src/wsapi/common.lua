-----------------------------------------------------------------------------
-- wsapi.common - common functionality for adapters and launchers
--
-- Author: Fabio Mascarenhas
-- Copyright (c) 2007 Kepler Project
--
-----------------------------------------------------------------------------

--print( fatfs.file_mode("/junk251.jnk") )
--print( fatfs.file_mode("/sensors/aa.txt") )
--
--print( fatfs.chdir("/sensors") )
--/sensors/aa.txt

--local lfs = require "lfs"
--local fatfs = require "fatfs"
--local _, ringer = pcall(require, "wsapi.ringer")
local _G = _G

module("wsapi.common", package.seeall)

-- Meta information is public even if begining with an "_"
_G.wsapi._COPYRIGHT   = "Copyright (C) 2007-2010 Kepler Project"
_G.wsapi._DESCRIPTION = "WSAPI - the Lua Web Server API"
_G.wsapi._VERSION     = "WSAPI 1.3.4"

-- HTTP status codes
status_codes = {
   [100] = "Continue",
   [101] = "Switching Protocols",
   [200] = "OK",
   [201] = "Created",
   [202] = "Accepted",
   [203] = "Non-Authoritative Information",
   [204] = "No Content",
   [205] = "Reset Content",
   [206] = "Partial Content",
   [300] = "Multiple Choices",
   [301] = "Moved Permanently",
   [302] = "Found",
   [303] = "See Other",
   [304] = "Not Modified",
   [305] = "Use Proxy",
   [307] = "Temporary Redirect",
   [400] = "Bad Request",
   [401] = "Unauthorized",
   [402] = "Payment Required",
   [403] = "Forbidden",
   [404] = "Not Found",
   [405] = "Method Not Allowed",
   [406] = "Not Acceptable",
   [407] = "Proxy Authentication Required",
   [408] = "Request Time-out",
   [409] = "Conflict",
   [410] = "Gone",
   [411] = "Length Required",
   [412] = "Precondition Failed",
   [413] = "Request Entity Too Large",
   [414] = "Request-URI Too Large",
   [415] = "Unsupported Media Type",
   [416] = "Requested range not satisfiable",
   [417] = "Expectation Failed",
   [500] = "Internal Server Error",
   [501] = "Not Implemented",
   [502] = "Bad Gateway",
   [503] = "Service Unavailable",
   [504] = "Gateway Time-out",
   [505] = "HTTP Version not supported",
}

-- Makes an index metamethod for the environment, from
-- a function that returns the value of a server variable
-- a metamethod lets us do "on-demand" loading of the WSAPI
-- environment, and provides the invariant the the WSAPI
-- environment returns the empty string instead of nil for
-- variables that do not exist
function sv_index(func)
  if type(func) == "table" then
    return function (env, n)
             local v = func[n]
             env[n] = v or ""
             return v or ""
           end
  else
    return function (env, n)
             local v = func(n)
             env[n] = v or ""
             return v or ""
           end
  end
end

-- Makes an wsapi_env.input object from a low-level input
-- object and the name of the method to read from this object
function input_maker(obj, read_method)
   local input = {}
   read = obj[read_method or "read"]

   function input:read(n)
     n = n or self.length or 0
     if n > 0 then return read(obj, n) end
   end
   return input
end

-- Windows only: sets stdin and stdout to binary mode so
-- sending and receiving binary data works with CGI
--function setmode()
--   pcall(lfs.setmode, io.stdin, "binary")
--   pcall(lfs.setmode, io.stdout, "binary")
--end

-- Returns the actual WSAPI handler (a function) for the
-- WSAPI application, whether it is a table, the name of a Lua
-- module, a Lua script, or the function itself
--function normalize_app(app_run, is_file)
function normalize_app(app_run)
   -- v lapi.c je lua_type(), pripadne ine funkcie pre zistenie typu objektu
   local t = type(app_run)
   print("normalize_app: " .. t)
   if t == "function" then
      return app_run
   elseif t == "table" then
      return app_run.run
--   elseif t == "string" then
--      print("!!!!!!!!!!!!!!!!!! v C nemam ostetrene, ze v v normalize_app moze byt type 'string'")
--      if is_file then
--         return normalize_app(dofile(app_run))
--      else
--         return normalize_app(require(app_run))
--      end
   else
      error("not a valid WSAPI application")
   end
end

-- Sends the respose body through the "out" pipe, using
-- the provided write method. Gets the body from the
-- response iterator
function send_content(out, res_iter, write_method)
   local write = out[write_method or "write"]
   local flush = out.flush
   local ok, res = xpcall(res_iter, debug.traceback)
   while ok and res do
      write(out, res)
      if flush then flush(out) end
      ok, res = xpcall(res_iter, debug.traceback)
   end
   if not ok then
      write(out,
            "======== WSAPI ERROR DURING RESPONSE PROCESSING: \n<pre>" ..
              tostring(res) .. "\n</pre>")
   end
end

-- Sends the complete response through the "out" pipe,
-- using the provided write method
function send_output(out, status, headers, res_iter, write_method, res_line)
   local write = out[write_method or "write"]
   if type(status) == "number" or status:match("^%d+$") then
     status = status .. " " .. status_codes[tonumber(status)]
   end
   if res_line then
     write(out, "HTTP/1.1 " .. (status or "500 Internal Server Error") .. "\r\n")
   else
     write(out, "Status: " .. (status or "500 Internal Server Error") .. "\r\n")
   end
   for h, v in pairs(headers or {}) do
      if type(v) ~= "table" then
         write(out, h .. ": " .. tostring(v) .. "\r\n")
      else
         for _, v in ipairs(v) do
            write(out, h .. ": " .. tostring(v) .. "\r\n")
         end
      end
   end
   write(out, "\r\n")
   send_content(out, res_iter, write_method)
end

-- Formats the standard error message for WSAPI applications
function error_html(msg)
   return string.format([[
        <html>
        <head><title>WSAPI Error in Application</title></head>
        <body>
        <p>There was an error in the specified application.
        The full error message follows:</p>
<pre>
%s
</pre>
        </body>
        </html>
      ]], tostring(msg))
end

-- Body for a 500 response
function status_500_html(msg)
   return error_html(msg)
end

-- Body for a 404 response
function status_404_html(msg)
   return string.format([[
        <html>
        <head><title>Resource not found</title></head>
        <body>
        <p>%s</p>
        </body>
        </html>
      ]], tostring(msg))
end

function status_200_html(msg)
   return string.format([[
        <html>
        <head><title>Resource not found</title></head>
        <body>
        <p>%s</p>
        </body>
        </html>
      ]], tostring(msg))
end

local function make_iterator(msg)
  local sent = false
  return function ()
           if sent then return nil
           else
             sent = true
             return msg
           end
         end
end

-- Sends an error response through the "out" pipe, replicated
-- to the "err" pipe (for logging, for example)
-- msg is the error message
function send_error(out, err, msg, out_method, err_method, http_response)
   local write = out[out_method or "write"]
   local write_err = err[err_method or "write"]
   write_err(err, "WSAPI error in application: " .. tostring(msg) .. "\n")
   local msg = error_html(msg)
   local status, headers, res_iter = "500 Internal Server Error", {
        ["Content-Type"] = "text/html",
        ["Content-Length"] = #msg
      }, make_iterator(msg)
   send_output(out, status, headers, res_iter, out_method, http_response)
   return status, headers
end

-- Sends a 404 response to the "out" pipe, "msg" is the error
-- message
function send_404(out, msg, out_method, http_response)
   local write = out[out_method or "write"]
   local msg = status_404_html(msg)
   local status, headers, res_iter = "404 Not Found", {
        ["Content-Type"] = "text/html",
        ["Content-Length"] = #msg
      }, make_iterator(msg)
   send_output(out, status, headers, res_iter, out_method, http_response)
   return status, headers
end

-- Runs the application in the provided WSAPI environment, catching errors and
-- returning the appropriate error repsonses
function run_app(app, env)
   print("run_app(app, env):")
   --return xpcall(function () return ( normalize_app(app) ) (env) end,
   return xpcall(function () return ( normalize_app(app) ) (env) end,
                 function (msg)
                    if type(msg) == "table" then
                       env.STATUS = msg[1]
                       return _M["status_" .. msg[1] .. "_html"](msg[2])
                    else
                       return debug.traceback(msg, 2)
                    end
                 end)
end

-- Builds an WSAPI environment from the configuration table "t"
function wsapi_env(t)
   print("wsapi_env(t)")
   local env = {}
   setmetatable(env, { __index = sv_index(t.env) })
   env.input = input_maker(t.input, t.read_method)
   env.error = t.error
   env.input.length = tonumber(env.CONTENT_LENGTH) or 0
   if env.PATH_INFO == "" then env.PATH_INFO = "/" end
   return env
end

-- Runs an application with data from the configuration table "t",
-- sending the WSAPI error/not found responses in case of errors
function run(app, t)
   print("run(app, t):")
   local env = wsapi_env(t)
   local ok, status, headers, res_iter =
      run_app(app, env)
   if ok then
     if not headers["Content-Length"] then
       if t.http_response then
         headers["Transfer-Encoding"] = "chunked"
         local unchunked = res_iter
         res_iter = function ()
                      local msg = unchunked()
                      if msg then
                        return string.format("%x\r\n%s\r\n", #msg, msg)
                      end
                    end
       end
     end
     send_output(t.output, status, headers, res_iter, t.write_method, t.http_response)
   else
     if env.STATUS == 404 then
       return send_404(t.output, status, t.write_method, t.http_response)
     else
       return send_error(t.output, t.error, status, t.write_method, t.err_method, t.http_response)
     end
   end
   return status, headers
end


-- Version of require skips searching package.path
function require_file(filename, modname)
  print("require_file(filename, modname)")
  package.loaded[modname] = true
  local res = loadfile(filename)(modname)
  if res then
    package.loaded[modname] = res
  end
  return package.loaded[modname]
end

-- Loads the script for a WSAPI application (require'ing in case of
-- a .lua script and dofile'ing it in case of other extensions),
-- returning the WSAPI handler function for this application
-- also moves the current directory to the application's path
function load_wsapi(path, file, modname, ext)
  print("load_wsapi(path, file, modname, ext)")
  --lfs.chdir(path)
  --fatfs.chdir(path)
  local app
  if ext == "lua" then
    app = require_file(file, modname)
  else
    app = dofile(file)
  end
  return normalize_app(app)
end

