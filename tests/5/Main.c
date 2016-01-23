/*
 * 自定义构造函数执行fork
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

__attribute((constructor))
void myinit(void)
{
    if(fork() < 0) {
        printf("fork failed\n");
    }
    else {
        printf("fork succeeded\n");
    }
}

int main(void)
{
    return 0;
}