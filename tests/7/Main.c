/*
 * 执行系统命令
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    system("/bin/echo test");
    return 0;
}