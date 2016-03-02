/*
 * uid gid test
 */

#include <stdio.h>
#include <unistd.h>

int main()
{
    printf("uid %d gid %d\n", getuid(), getgid());
    return 0;
}
