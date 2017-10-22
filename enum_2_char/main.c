#include <stdio.h>
#include <stdlib.h>

_Bool printEnum(int);

int main(int argc, char **argv)
{
    int input;

    if (argc > 1)
    {
        input = atoi(argv[1]);
    }
    else
        return EXIT_FAILURE;
    if (printEnum(input))
    {
        printf("error - [%d]\n", input);
    }
    return EXIT_SUCCESS;
}
