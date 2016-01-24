#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    // 150m
    int big_size = 150 * 1024 * 1024;

    int *b = NULL;

    b = (int *)malloc(big_size);
    memset(b, 0, big_size);
    return 0;
}
