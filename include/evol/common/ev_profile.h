#include <evol/common/ev_macros.h>

/*
----------------------------WARNING----------------------------
If remotery ever fails to export its functions on visual studio
through __declspec(dllexport), then this is because of the fact
that #23 had a hacky way to export statically linked functions
from the evol DLL. If the exports start failing, then the issue
should be checked for hints on how to fix it.
More info: https://github.com/evol3D/evol/issues/23
*/

#if (defined(EV_PROFILING_ENABLED) && defined(EV_BUILD_RELEASE))
#define RMT_ENABLED 1
#else
#define RMT_ENABLED 0
#endif

#include <Remotery.h>

enum CPUSampleFlags {
    EV_CPUSF_None = RMTSF_None,
    EV_CPUSF_Aggregate = RMTSF_Aggregate,
    EV_CPUSF_Recursive = RMTSF_Recursive,
};

#define ev_BeginCPUSample(Label, Flags) rmt_BeginCPUSample(Label, Flags)
#define ev_EndCPUSample() rmt_EndCPUSample()