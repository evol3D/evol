local ffi = require 'ffi'
ffi.cdef[[ const char * getMetadata() ]]

modulesystem = {}
modulesystem.modules = {}
modulesystem.size = 0
modulesystem.categories = {}

local function access(lib, symbol) return lib[symbol] end
local function has_value(arr, val)
  for _,v in ipairs(arr) do
    if(v == val) then
      return true
    end
  end
  return false
end

function add_module(module_path)
  local ev_mod = ffi.load(module_path)

  if pcall(access, ev_mod, "getMetadata") then

    local luadata = ffi.string(ev_mod.getMetadata())
    local mod_def, err = loadstring("mod = " .. luadata)

    if not mod_def then
      print(err)
      return -1
    else
      mod_def()
      mod.path = module_path
      modulesystem.modules[mod.name] = mod
      modulesystem.size = modulesystem.size + 1
    end

  else
    print("Module '" .. module_path .. "' doesn't implement the getMetadata() function. Ignoring ...")
    return -1
  end
  return 0
end

function get_module_with_name(module_name)
  local mod = modulesystem.modules[module_name]
  if mod then
    return mod.path
  else
    return nil
  end
end

function get_module_with_category(module_category)
  if modulesystem.categories[module_category] then
    return modulesystem.categories[module_category].path
  end
  for name, data in pairs(modulesystem.modules) do
    if has_value(data.categories, module_category) then
      modulesystem.categories[module_category] = data
      return data.path
    end
  end
  return nil
end

function get_module(module)
  local mod = get_module_with_category(module)
  if not mod then
    mod = get_module_with_name(module)
  end
  return mod
end
