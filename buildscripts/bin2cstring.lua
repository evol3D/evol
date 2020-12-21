local compile, filename = arg[1]:match"^(+?)(.*)"
local status = arg[2]

local content = compile=="+"
and string.dump(assert(loadfile(filename)))
or assert(io.open(filename,"rb")):read"*a"

local function boilerplate(fmt)
  return string.format(fmt,
    status and "("..status.."=" or "",
    filename,
    status and ")" or "",
    status and status.."=" or "",
    filename)
end

local dump do
  local numtab={}; for i=0,255 do numtab[string.char(i)]=("%3d,"):format(i) end
  function dump(str)
    return (str:gsub(".", numtab):gsub(("."):rep(80), "%0\n") .. " 0")
  end
end

io.write(boilerplate[=[ { ]=], dump(content), boilerplate[=[ }; ]=])
