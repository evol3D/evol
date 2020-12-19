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

function add_module(module_path, module_metadata)

  local mod_def, err = loadstring("mod = " .. module_metadata)

  if not mod_def then
    print(err)
    return -1
  end

  mod_def()
  mod.path = module_path
  modulesystem.modules[mod.name] = mod
  modulesystem.size = modulesystem.size + 1

  for _,v in ipairs(mod.categories) do
    if not modulesystem.categories[v] then
      modulesystem.categories[v] = mod.name
    end
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
    return modulesystem.modules[modulesystem.categories[module_category]].path
  else
    return nil
  end
end

function get_module(module)
  local mod = get_module_with_category(module)
  if not mod then
    mod = get_module_with_name(module)
  end
  return mod
end
