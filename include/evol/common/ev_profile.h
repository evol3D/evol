#include <evol/common/ev_macros.h>

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