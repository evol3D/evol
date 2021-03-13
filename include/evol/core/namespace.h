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

EVCOREAPI uint32_t NamespaceInit(NS *ns);
EVCOREAPI uint32_t NamespaceSetName(NS *ns, const char *name);
EVCOREAPI uint32_t NamespaceAddFn(NS *ns, NSFn *fn);
EVCOREAPI void NamespacePrint(NS *ns);
EVCOREAPI void NamespaceDeinit(NS *ns);

EVCOREAPI uint32_t FunctionInit(NSFn *fn);
EVCOREAPI uint32_t FunctionSetName(NSFn *fn, const char *name);
EVCOREAPI uint32_t FunctionSetReturnType(NSFn *fn, const char *ret_type);
EVCOREAPI uint32_t FunctionPushParams(NSFn *fn, ...);
EVCOREAPI uint32_t FunctionBind(NSFn *fn, FN_PTR fn_impl);
EVCOREAPI void FunctionDeinit(NSFn *fn);

