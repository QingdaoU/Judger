#include <unistd.h>
#include <stdio.h>

int main()
{
    pid_t pid = fork();
    if (pid > 0) {
        printf("i'm parent");
    }
    else if (pid == 0) {
        printf("i'm children");
    }
    else {
        printf("fork failed");
    }
    return 0;
}