#include <evol/core/namespace.h>
#include <hashmap.h>

U64
NS_hash(
  const PTR item, 
  U64 seed0, 
  U64 seed1)
{
  NS *ns = (NS *)item;
  return hashmap_murmur(ns->name, strlen(ns->name), seed0, seed1);
}

I32
NS_cmp(
  const PTR item1, 
  const PTR item2, 
  PTR _udata)
{
  EV_UNUSED_PARAM(_udata);
  NS *ns1 = (NS *)item1;
  NS *ns2 = (NS *)item2;

  return strcmp(ns1->name, ns2->name);
}

bool
NS_free(
  PTR item, 
  PTR _udata)
{
  EV_UNUSED_PARAM(_udata);
  if(item)
    NamespaceDeinit((NS*)item);
  return true;
}


U64
NSFn_hash(
  const PTR item, 
  U64 seed0, 
  U64 seed1)
{
  NSFn *fn = (NSFn *)item;

  return hashmap_murmur(fn->name, strlen(fn->name), seed0, seed1);
}

I32
NSFn_cmp(
  const PTR item1, 
  const PTR item2, 
  PTR _udata)
{
  EV_UNUSED_PARAM(_udata);
  NSFn *fn1 = (NSFn *)item1;
  NSFn *fn2 = (NSFn *)item2;

  return strcmp(fn1->name, fn2->name);
}

bool
NSFn_free(
  PTR item, 
  PTR _udata)
{
  EV_UNUSED_PARAM(_udata);
  if (item)
    FunctionDeinit((NSFn*)item);
  return true;
}

U32 
NamespaceInit(
  NS *ns)
{
  if (!ns)
    return 0;

  ns->name = NULL;
  ns->functions = hashmap_new(sizeof(NSFn), 16, 0, 0, NSFn_hash, NSFn_cmp, NULL);

  if (ns->functions)
    return 1;
}

U32 
NamespaceSetName(
  NS *ns, 
  CONST_STR name)
{
  if (ns->name)
    evstring_clear(&(ns->name));
  evstring_pushstr(&(ns->name), name);
}

U32 
NamespaceAddFn(
  NS *ns, 
  NSFn *fn)
{
  if (!ns->functions)
    return 1;
  NSFn *oldFn;
  oldFn = hashmap_set(ns->functions, fn);
  if (oldFn) {
    FunctionDeinit(oldFn);
  } else {
    if (hashmap_oom(ns->functions)) {
      return 1;
    }
  }
  return 0;
}

#include <stdio.h>
void 
_ev_printFunctionParams(
  NSFn *fn)
{
  NSFnParam *param;
  vec_foreach(param, fn->params) {
    printf(" %s %s,", param->type, param->name);
  }
}

void 
_ev_printFunction(
  NSFn *fn)
{
  printf("\t%s %s(", fn->returnType, fn->name);
  _ev_printFunctionParams(fn);
  printf("),\n");
}

bool
NSFn_print(
  PTR item, 
  PTR _udata)
{
  EV_UNUSED_PARAM(_udata);
  if (!item)
    return true;
  NSFn *fn = (NSFn *)item;
  _ev_printFunction(fn);
  return true;
}

void 
NamespacePrint(
  NS *ns)
{
  printf("\n%s {\n", ns->name);
  hashmap_scan(ns->functions, NSFn_print, NULL);
  printf("}\n\n");
}

void 
NamespaceDeinit(
  NS *ns)
{
  if (ns->name)
    evstring_free(ns->name);
  if (ns->functions) {
    hashmap_scan(ns->functions, NSFn_free, NULL);
    hashmap_free(ns->functions);
  }
  ns->name = NULL;
  ns->functions = NULL;
}

void
NSFnParam_cpy(
  NSFnParam *dst, 
  const NSFnParam *src)
{
  dst->name = evstring_clone(src->name);
  dst->type = evstring_clone(src->type);
}

void
NSFnParam_destr(
  NSFnParam *data)
{
  if (data->type)
    evstring_free(data->type);
  if (data->name)
    evstring_free(data->name);
}

U32
FunctionInit(
  NSFn *fn)
{
  fn->name       = NULL;
  fn->returnType = NULL;
  fn->handle     = NULL;
  fn->params     = vec_init(NSFnParam, NSFnParam_cpy, NSFnParam_destr);

  if (!fn->params)
    return 1;
  return 0;
}

U32 
FunctionSetName(
  NSFn *fn, 
  CONST_STR name)
{
  if (fn->name)
    evstring_clear(&(fn->name));
  evstring_pushstr(&(fn->name), name);
}

U32 
FunctionSetReturnType(
  NSFn *fn, 
  CONST_STR ret_type)
{
  if (fn->returnType)
    evstring_clear(&(fn->returnType));
  evstring_pushstr(&(fn->returnType), ret_type);
}

#include <stdarg.h>
U32 
FunctionPushParams(
  NSFn *fn, 
  ...)
{
  va_list args;
  va_start(args, fn);
  NSFnParam param;
  while (true) {
    param = va_arg(args, NSFnParam);
    if (param.name == NULL || param.type == NULL)
      break;

    vec_push(&fn->params, &param);
  }
  va_end(args);
}

U32 
FunctionBind(
  NSFn *fn, 
  FN_PTR fn_impl)
{
  fn->handle = fn_impl;
}

void 
FunctionDeinit(
  NSFn *fn)
{
  if (fn->name)
    evstring_free(fn->name);
  if (fn->returnType)
    evstring_free(fn->returnType);
  if (fn->params)
    vec_fini(fn->params);
  fn->name   = NULL;
  fn->returnType = NULL;
  fn->handle = NULL;
  fn->params     = NULL;
}
