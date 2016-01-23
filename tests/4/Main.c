/*
 * 不允许的系统调用 exec
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int arg, char **args) {
    char *argv[] = {"/bin/echo", "1234567890", NULL};
    char *envp[] = {NULL};
    printf("execve test\n");
    execve("/bin/echo", argv, envp);
}
