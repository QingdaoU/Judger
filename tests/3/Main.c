/*
 * 不允许的系统调用 clone
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    pid_t pid;
    printf("fork test\n");
    pid = fork();
    if(pid < 0){
        printf("fork failed\n");
    }
    else {
        printf("fork succeeded\n");
    }
    return 0;
}