#include "../headers/other.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

void sleepForFiveSeconds()
{
#ifdef _WIN32
        Sleep(5000);
#else
        sleep(5);
#endif
}