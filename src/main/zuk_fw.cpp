#include <stdio.h>
#include <stdlib.h>

#include <zuk_main.h>

/**
 * @brief main
 *
 * @return 0: normal quit, 1: normal reboot
 */
int main(int argc, char **argv)
{
    zuk_init(argc, argv);
    zuk_final(argc, argv);
    return 0;
}

