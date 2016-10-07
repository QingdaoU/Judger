#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    char *argv[] = {"/bin/echo", "Helloworld", NULL};
    char *env[] = {NULL};

    execve("/bin/echo", argv, env);
    perror("execve");
    exit(EXIT_FAILURE);
}