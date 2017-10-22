#include <stdio.h>
#include <stdlib.h>

//#include "ns.h"
_Bool isCharging();

int main(int argc, char **argv)
{
    if (isCharging())
        printf("charging\n");
    return EXIT_SUCCESS;
}
