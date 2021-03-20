#pragma once

#include <evol/evol.h>
#include <evol/core/evstore.h>
#include <evol/common/ev_types.h>
#include <evol/common/ev_macros.h>
#include <evol/core/event.h>
#include <evol/core/eventsystem.h>

#define EV_CONSTRUCTOR_FN_NAME ev_init
#define EV_DESTRUCTOR_FN_NAME  ev_fini
#define EV_UPDATE_FN_NAME      ev_update
#define EV_BINDINGS_FN_NAME    ev_bindings

#if defined(EV_OS_LINUX)
# define EV_CONSTRUCTOR_ATTR __attribute__((constructor))
# define EV_DESTRUCTOR_ATTR  __attribute__((destructor))
#elif defined(EV_OS_WINDOWS)
# define EV_CONSTRUCTOR_ATTR
# define EV_DESTRUCTOR_ATTR
#else
#error("Unsupported Operating System")
#endif

#define EV_CONSTRUCTOR EV_EXPORT I32 EV_CONSTRUCTOR_FN_NAME()
#define EV_DESTRUCTOR  EV_EXPORT I32 EV_DESTRUCTOR_FN_NAME()
#define EV_UPDATE      EV_EXPORT I32 EV_UPDATE_FN_NAME(REAL deltaTime)
#define EV_BINDINGS              I32 EV_BINDINGS_FN_NAME(evolengine_t *evol_instance)

#if defined(EV_MODULE_DEFINE)

#if defined (EVMOD_TYPES_DEF)
# if defined(TYPE)
#  undef TYPE
# endif

# define TYPE(name, ...) typedef struct __VA_ARGS__ name;
# include <meta/evmod.types>

# undef TYPE
#endif

#if defined (EVMOD_EVENTS_DEF)
# if defined(PRIMARY)
#  undef PRIMARY
# endif
# if defined(SECONDARY)
#  undef SECONDARY
# endif

# define PRIMARY EVENT_DECLARE_PRIMARY
# define SECONDARY EVENT_DECLARE_SECONDARY
# include <meta/evmod.events>
# undef SECONDARY
# undef PRIMARY
#endif


#if defined (EVMOD_NAMESPACES_DEF)
EV_BINDINGS;
#define SELF(name) EV_CONCAT(Self, name)
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
# define EV_NS_DEF_END(name) } NAMESPACE(name); \
  NAMESPACE(name) SELF(name); \
  EVMODAPI NAMESPACE(name) *EV_CONCAT(GetNamespace, name)(){ return &SELF(name); }
# include <meta/evmod.namespaces>
# undef FNPARAM_UNWRAP_IMPL_LAST2
# undef FNPARAM_UNWRAP_IMPL_LAST
# undef FNPARAM_UNWRAP_IMPL2
# undef FNPARAM_UNWRAP_IMPL
# undef FNPARAM_UNWRAP
# undef EV_NS_DEF_BEGIN
# undef EV_NS_DEF_END
# undef EV_NS_DEF_FN
#endif

EV_CONSTRUCTOR;
EV_DESTRUCTOR;

void STATIC_INIT()
{
#if defined(EVMOD_EVENTS_DEF)
# define PRIMARY EVENT_INIT_PRIMARY
# define SECONDARY EVENT_INIT_SECONDARY
# define TYPE(name, ...)
# include <meta/evmod.events>
  EventSystem.sync();
#endif

#if defined(EVMOD_NAMESPACES_DEF)
// TODO core functions for namespace registration
evstore_entry_t store_entry;
EvStoreResult res = evstore_get_checktype(GLOBAL_STORE, "EVOL_INSTANCE", EV_TYPE_PTR, &store_entry);
if(res != EV_STORE_ENTRY_FOUND)
  return;

evolengine_t *evol_instance = store_entry.data;

// The following is working code that actually populates a namespace inside the core's
// namespaces map. It will be useful when implementing the scripting engine. It is only
// commented because it's not doing any useful right now.

//#define FNPARAM_IMPL(type, name) (NSFnParam){EV_STRINGIZE(type), EV_STRINGIZE(name)},
//#define FNPARAM(...) FNPARAM_IMPL __VA_ARGS__
//#define EXPAND_PARAMS(...) FOREACH(FNPARAM, __VA_ARGS__) (NSFnParam){NULL, NULL}
//
//# define EV_NS_DEF_BEGIN(name)                         \
//  {                                                    \
//  NS ns;                                               \
//  NamespaceInit(&ns);                                  \
//  NamespaceSetName(&ns, EV_STRINGIZE(name));           
//
//# define EV_NS_DEF_FN(retType, name, ...)              \
//  {                                                    \
//  NSFn fn;                                             \
//  FunctionInit(&fn);                                   \
//  FunctionSetName(&fn, EV_STRINGIZE(name));            \
//  FunctionSetReturnType(&fn, EV_STRINGIZE(retType));   \
//  FunctionPushParams(&fn,                              \
//  EXPAND_PARAMS(__VA_ARGS__)                           \
//  );                                                   \
//  NamespaceAddFn(&ns, &fn);                            \
//  }
//
//# define EV_NS_DEF_END(name)                           \
//  evol_registerNS(evol_instance, &ns);                 \
//  }
//# include <meta/evmod.namespaces>
//
#define EV_NS_BIND_FN(ns, fn, handle) \
    SELF(ns).fn = handle;               \
    //evol_bindNSFunction(evol_instance, EV_STRINGIZE(ns), EV_STRINGIZE(fn), handle);
  EV_BINDINGS_FN_NAME(evol_instance);
#endif
}

void STATIC_DEINIT() {}

const char MODULE_DATA[] =
#if defined(EVMOD_LUA)
#include EV_STRINGIZE(EVMOD_LUA)
#else
{'\0'};
#endif

EVMODAPI const char * getMetadata() { return MODULE_DATA; }


# if defined(EV_OS_WINDOWS)
# include <windows.h>

BOOL __stdcall DllMain( HMODULE _hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID _lpReserved)
{
  (void)_lpReserved,_hModule;
  switch (ul_reason_for_call) {
  case DLL_PROCESS_ATTACH:
    STATIC_INIT();
    EV_CONSTRUCTOR_FN_NAME();
    break;
  case DLL_THREAD_ATTACH:
    break;
  case DLL_THREAD_DETACH:
    break;
  case DLL_PROCESS_DETACH:
    EV_DESTRUCTOR_FN_NAME();
    STATIC_DEINIT();
    break;
  default:
    break;
  }
  return TRUE;
}
# else

EV_CONSTRUCTOR_ATTR void evmod_constructor_fn() { STATIC_INIT(); EV_CONSTRUCTOR_FN_NAME(); }
EV_DESTRUCTOR_ATTR  void evmod_destructor_fn()  { EV_DESTRUCTOR_FN_NAME(); STATIC_DEINIT(); }

# endif

#if defined(EVMOD_EVENTS_DEF)
#undef TYPE
#undef SECONDARY
#undef PRIMARY
#endif


#undef EV_MODULE_DEFINE
#endif
