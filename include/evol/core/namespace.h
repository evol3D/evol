#pragma once

#include <evol/common/ev_types.h>

// TODO change all to sds
typedef struct NamespaceFunctionParameter {
  SDS type;
  SDS name;
} NSFnParam;

typedef struct NamespaceFunction {
  SDS name;
  SDS returnType;
  FN_PTR handle;
  vec(NSFnParam) params;
} NSFn;

typedef struct Namespace {
  SDS name;
  struct hashmap *functions;
} NS;

uint32_t NamespaceInit(NS *ns);
uint32_t NamespaceSetName(NS *ns, const char *name);
uint32_t NamespaceAddFn(NS *ns, NSFn fn);
void NamespacePrint(NS *ns);
void NamespaceDeinit(NS *ns);

uint32_t FunctionInit(NSFn *fn);
uint32_t FunctionSetName(NSFn *fn, const char *name);
uint32_t FunctionSetReturnType(NSFn *fn, const char *ret_type);
uint32_t FunctionPushParams(NSFn *fn, ...);
uint32_t FunctionBind(NSFn *fn, FN_PTR fn_impl);
void FunctionDeinit(NSFn *fn);

