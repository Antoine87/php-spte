#include <stdio.h>
#include <stdlib.h>

#if defined SPTE_MSVC

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#elif defined SPTE_UNIX

#include <unistd.h>

#else
#error This program requires either a POSIX or Winsock API
#endif

static void
usage()
{
    puts("Usage: wait <milliseconds>\n"
         "   <milliseconds> Number of milliseconds passed to sleep().\n");
}

int
main(int argc, char *argv[])
{
    if (argc < 2) {
        usage();
        exit(EXIT_FAILURE);
    }

#ifdef SPTE_MSVC
    Sleep((DWORD) atoi(argv[1]));
#else
    sleep(atoi(argv[1]) / 1000);
#endif

    return EXIT_SUCCESS;
}
