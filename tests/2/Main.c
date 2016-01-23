/*
 * 正常运行的程序
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    // 1k
    int small_size = 1024;
    // 150m
    int big_size = 150 * 1024 * 1024;

    int *s = NULL, *b = NULL;

    s = (int *)malloc(small_size);
    if(s){
        printf("malloc small size succedeed\n");
    }
    else{
        printf("malloc small size failed\n");
        return -1;
    }

    b = (int *)malloc(big_size);
    if(b){
        printf("malloc big size succedeed\n");
    }
    else{
        printf("malloc big size failed\n");
        return -2;
    }
    return 0;
}