#include <stdio.h>
#include <stdlib.h>

int
main(void)
{
    int c;

    while (EOF != (c = getchar())) {
        putchar(c);
    }

    return EXIT_SUCCESS;
}
