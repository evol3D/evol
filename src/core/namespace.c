#include <evol/core/namespace.h>

U32 NamespaceInit(NS *ns)
{
  // TODO
}

U32 NamespaceSetName(NS *ns, const char *name)
{
  ns->name = sdsnew(name);
}

uint32_t NamespaceAddFn(NS *ns, NSFn fn)
{
  // TODO
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
  printf("%s %s(", fn->returnType, fn->name);
  _ev_printFunctionParams(fn);
  printf(")\n");
}

void NamespacePrint(NS *ns)
{
  // TODO
}

void NamespaceDeinit(NS *ns)
{
  // TODO
}

uint32_t FunctionInit(NSFn *fn)
{
  // TODO
}

uint32_t FunctionSetName(NSFn *fn, const char *name)
{
  // TODO
}

uint32_t FunctionSetReturnType(NSFn *fn, const char *ret_type)
{
  // TODO
}

uint32_t FunctionPushParams(NSFn *fn, ...)
{
  // TODO
}

uint32_t FunctionBind(NSFn *fn, FN_PTR fn_impl)
{
  // TODO
}

void FunctionDeinit(NSFn *fn)
{
  // TODO
}
