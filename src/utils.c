#include <utils.h>

#ifdef _WIN32

#include <windows.h>

void sleep_ms(double milliseconds)
{
  Sleep(milliseconds);
}

#else

#include <unistd.h>
#include <time.h>

void sleep_ms(double milliseconds)
{
  struct timespec tim = 
  {
    .tv_sec = 0,
    .tv_nsec = milliseconds * 1000000,
  };
  nanosleep(&tim, NULL);
}

#endif
