#include <stdio.h>
#include <stdlib.h>

int
main(void)
{
    fprintf(stdout, "Output stream");
    fprintf(stderr, "Error stream");

    return EXIT_SUCCESS;
}
