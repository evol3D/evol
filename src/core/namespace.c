#define EV_CORE_FILE
#include <evol/core/namespace.h>
#include <hashmap.h>

U64
NS_hash(const void *item, U64 seed0, U64 seed1)
{
  NS *ns = (NS *)item;
  return hashmap_murmur(ns->name, strlen(ns->name), seed0, seed1);
}

int
NS_cmp(const void *item1, const void *item2, void *_udata)
{
  EV_UNUSED_PARAM(_udata);
  NS *ns1 = (NS *)item1;
  NS *ns2 = (NS *)item2;

  return strcmp(ns1->name, ns2->name);
}

bool
NS_free(void *item, void *_udata)
{
  EV_UNUSED_PARAM(_udata);
  if(item)
    NamespaceDeinit((NS*)item);
  return true;
}


U64
NSFn_hash(const void *item, U64 seed0, U64 seed1)
{
  NSFn *fn = (NSFn *)item;

  return hashmap_murmur(fn->name, strlen(fn->name), seed0, seed1);
}

int
NSFn_cmp(const void *item1, const void *item2, void *_udata)
{
  EV_UNUSED_PARAM(_udata);
  NSFn *fn1 = (NSFn *)item1;
  NSFn *fn2 = (NSFn *)item2;

  return strcmp(fn1->name, fn2->name);
}

bool
NSFn_free(void *item, void *_udata)
{
  EV_UNUSED_PARAM(_udata);
  if (item)
    FunctionDeinit((NSFn*)item);
  return true;
}

U32 NamespaceInit(NS *ns)
{
  if (!ns)
    return 0;

  ns->name = NULL;
  ns->functions = hashmap_new(sizeof(NSFn), 16, 0, 0, NSFn_hash, NSFn_cmp, NULL);

  if (ns->functions)
    return 1;
}

U32 NamespaceSetName(NS *ns, const char *name)
{
  if (ns->name)
    sdsfree(ns->name);
  ns->name = sdsnew(name);
}

U32 NamespaceAddFn(NS *ns, NSFn *fn)
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
void _ev_printFunctionParams(NSFn *fn)
{
  NSFnParam *param;
  vec_foreach(param, fn->params) {
    printf(" %s %s,", param->type, param->name);
  }
}

void _ev_printFunction(NSFn *fn)
{
  printf("\t%s %s(", fn->returnType, fn->name);
  _ev_printFunctionParams(fn);
  printf("),\n");
}

bool
NSFn_print(void *item, void *_udata)
{
  EV_UNUSED_PARAM(_udata);
  if (!item)
    return true;
  NSFn *fn = (NSFn *)item;
  _ev_printFunction(fn);
  return true;
}

void NamespacePrint(NS *ns)
{
  printf("\n%s {\n", ns->name);
  hashmap_scan(ns->functions, NSFn_print, NULL);
  printf("}\n\n");
}

void NamespaceDeinit(NS *ns)
{
  if (ns->name)
    sdsfree(ns->name);
  if (ns->functions) {
    hashmap_scan(ns->functions, NSFn_free, NULL);
    hashmap_free(ns->functions);
  }
  ns->name = NULL;
  ns->functions = NULL;
}

void
NSFnParam_cpy(NSFnParam *dst, const NSFnParam *src)
{
  dst->name = sdsnew(src->name);
  dst->type = sdsnew(src->type);
}

void
NSFnParam_destr(NSFnParam *data)
{
  if (data->type)
    sdsfree(data->type);
  if (data->name)
    sdsfree(data->name);
}

U32
FunctionInit(NSFn *fn)
{
  fn->name = NULL;
  fn->returnType = NULL;
  fn->handle     = NULL;
  fn->params     = vec_init(NSFnParam, NSFnParam_cpy, NSFnParam_destr);

  if (!fn->params)
    return 1;
  return 0;
}

U32 FunctionSetName(NSFn *fn, const char *name)
{
  if (fn->name)
    sdsfree(fn->name);
  fn->name = sdsnew(name);
}

U32 FunctionSetReturnType(NSFn *fn, const char *ret_type)
{
  if (fn->returnType)
    sdsfree(fn->returnType);
  fn->returnType = sdsnew(ret_type);
}

#include <stdarg.h>
U32 FunctionPushParams(NSFn *fn, ...)
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

U32 FunctionBind(NSFn *fn, FN_PTR fn_impl)
{
  fn->handle = fn_impl;
}

void FunctionDeinit(NSFn *fn)
{
  if (fn->name)
    sdsfree(fn->name);
  if (fn->returnType)
    sdsfree(fn->returnType);
  if (fn->params)
    vec_fini(fn->params);
  fn->name   = NULL;
  fn->returnType = NULL;
  fn->handle = NULL;
  fn->params     = NULL;
}
