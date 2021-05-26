/*!
 * \file namespace_import.h
 */
#if defined(__cplusplus)
extern "C" {
#endif

#include <evol/common/ev_macros.h>
#include <evol/core/module.h>
#include <evol/core/namespace.h>
#include <evol/evol.h>

#if !defined(NAMESPACE_MODULE)
#error "NAMESPACE_MODULE needs to be defined"
#endif

# if defined(EV_NS_DEF_BEGIN)
#  undef EV_NS_DEF_BEGIN
# endif
# if defined(EV_NS_DEF_END)
#  undef EV_NS_DEF_END
# endif
# if defined(EV_NS_DEF_FN)
#  undef EV_NS_DEF_FN
# endif

# define FNPARAM_UNWRAP_IMPL_LAST2(type, name) type name 
# define FNPARAM_UNWRAP_IMPL_LAST(...) FNPARAM_UNWRAP_IMPL_LAST2 __VA_ARGS__
# define FNPARAM_UNWRAP_IMPL2(type, name) type name, 
# define FNPARAM_UNWRAP_IMPL(...) FNPARAM_UNWRAP_IMPL2 __VA_ARGS__
# define FNPARAM_UNWRAP(...) FOREACH_LAST(FNPARAM_UNWRAP_IMPL, FNPARAM_UNWRAP_IMPL_LAST, __VA_ARGS__)

# define EV_NS_DEF_BEGIN(name) typedef struct NAMESPACE(name) {
# define EV_NS_DEF_FN(retType, name, ...) retType(*name)(FNPARAM_UNWRAP(__VA_ARGS__));
# define EV_NS_DEF_END(name) } NAMESPACE(name);
#include EV_STRINGIZE(NAMESPACE_MODULE/meta/evmod.namespaces)
# undef FNPARAM_UNWRAP_IMPL_LAST2
# undef FNPARAM_UNWRAP_IMPL_LAST
# undef FNPARAM_UNWRAP_IMPL2
# undef FNPARAM_UNWRAP_IMPL
# undef FNPARAM_UNWRAP
# undef EV_NS_DEF_BEGIN
# undef EV_NS_DEF_END
# undef EV_NS_DEF_FN

#define IMPORT_NAMESPACE(name, mod) EV_CONCAT(IMPORT_NAMESPACE_, name)(mod);
#define IMPORT_NAMESPACE_FN(name) EV_CONCAT(IMPORT_NAMESPACE_, name)(evolmodule_t mod)
#define imports(module, ...) FOREACH_UDATA(IMPORT_NAMESPACE, module, EV_EXPAND __VA_ARGS__)

# define EV_NS_DEF_BEGIN(name)                                                                                             \
  static NAMESPACE(name) *name;                                                                                                   \
  EV_UNUSED static void IMPORT_NAMESPACE_FN(name)                                                                                           \
  {                                                                                                                        \
    NAMESPACE(name)*(*getns)() = (NAMESPACE(name)*(*)())evol_getmodfunc(mod, EV_STRINGIZE(EV_CONCAT(GetNamespace, name))); \
    if (!getns)                                                                                                            \
      return;                                                                                                              \
    name = getns();                                                                                                        \
  }
#define EV_NS_DEF_END(...)
#define EV_NS_DEF_FN(...)

//    NAMESPACE(name) *ns = &name;                                                                          \
//    STR nsName = EV_STRINGIZE(name);                                                                      \
//    evstore_entry_t store_entry;                                                                          \
//    EvStoreResult res = evstore_get_checktype(GLOBAL_STORE, "EVOL_INSTANCE", EV_TYPE_PTR, &store_entry);  \
//    if(res != EV_STORE_ENTRY_FOUND)                                                                       \
//      return;                                                                                             \
//    evolengine_t *evol_instance = store_entry.data;
//
//# define EV_NS_DEF_FN(retType, name, ...)                                                                 \
//    ns->name = evol_getNSBinding(evol_instance, nsName, EV_STRINGIZE(name));
//
//# define EV_NS_DEF_END(name)                                                                              \
//  }
#include EV_STRINGIZE(NAMESPACE_MODULE/meta/evmod.namespaces)
# undef EV_NS_DEF_BEGIN
# undef EV_NS_DEF_END
# undef EV_NS_DEF_FN

#undef NAMESPACE_MODULE

#if defined(__cplusplus)
}
#endif
