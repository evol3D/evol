#include <evol/utils/sleep.h>
#include <evol/common/ev_macros.h>

#ifdef EV_OS_WINDOWS

#include <windows.h>

void 
sleep_ms(
    double milliseconds)
{
  Sleep(milliseconds);
}

#else

#include <unistd.h>
#include <time.h>

void 
sleep_ms(
    double milliseconds)
{
  struct timespec tim = 
  {
    .tv_sec = 0,
    .tv_nsec = milliseconds * 1000000,
  };
  nanosleep(&tim, NULL);
}

#endif

