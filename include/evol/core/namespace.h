#pragma once

#include <evol/common/ev_types.h>

#define NAMESPACE(name) EV_CONCAT(EVNS_, name)

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

EVCOREAPI U32 NamespaceInit(NS *ns);
EVCOREAPI U32 NamespaceSetName(NS *ns, CONST_STR name);
EVCOREAPI U32 NamespaceAddFn(NS *ns, NSFn *fn);
EVCOREAPI void NamespacePrint(NS *ns);
EVCOREAPI void NamespaceDeinit(NS *ns);

EVCOREAPI U32 FunctionInit(NSFn *fn);
EVCOREAPI U32 FunctionSetName(NSFn *fn, CONST_STR name);
EVCOREAPI U32 FunctionSetReturnType(NSFn *fn, CONST_STR ret_type);
EVCOREAPI U32 FunctionPushParams(NSFn *fn, ...);
EVCOREAPI U32 FunctionBind(NSFn *fn, FN_PTR fn_impl);
EVCOREAPI void FunctionDeinit(NSFn *fn);

U64 NS_hash(const void *item, U64 seed0, U64 seed1);
int NS_cmp(const void *item1, const void *item2, void *_udata);
bool NS_free(void *item, void *_udata);
