--local util = require"wsapi.util"

--module("wsapi.put_requestt", package.seeall)

module("hello", package.seeall)

function run(wsapi_env)
  --local headers = { ["Content-type"] = "text/html", ["Content-Length"] = 0 }
  local headers = { ["keep-alive"] = 8, ["Content-Length"] = 0 }
--  local reqest = wsapi.request.new(wsapi_env)
--  
--  script_name  = wsapi_env.SCRIPT_NAME
--  
--  wsapi_env.Content-Range --bytes 0-1024/64641
--  wsapi_env.CONTENT_LENGTH --1024
--  
--  local function parse_range(range, size)
--      local partial, start, stop = false
--      -- parse bytes=start-stop
--      if range then
--        start, stop = wsapi_env.Content-Range:match('bytes (%d*)-?(%d*)')
--        partial = true§
--      end
--      start = tonumber(start) or 0
--      stop = tonumber(stop) or wsapi_env.CONTENT_LENGTH - 1
--      return start, stop, partial
--  end
--  
--  
--  
  local function empty_func()
    local i =0;
    local file = io.open(wsapi_env.SCRIPT_NAME, w)
    local length = tonumber(wsapi_env.CONTENT_LENGTH) or 0
     --print(wsapi_env.input:read(length))
    file:write(wsapi_env.input:read(length))
    file:clode()
--    while i < length do
--        print(wsapi_env.input:read(4))
--        i = i+4
--    end
  end

  return 201, headers, empty_func
end
return _M

--local from, to = request.headers.Range:match("bytes ([0-9]*)-([0-9]*)") 

-- http://tools.ietf.org/html/rfc2616 3.12 Range Units
-- The only range unit defined by HTTP/1.1 is "bytes". HTTP/1.1
-- implementations MAY ignore ranges specified using other units.
