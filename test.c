#include <unistd.h>
#include <stdio.h>

int main()
{
    printf("Hello, I'm test\n");
    int i = 900000000;
    while (i) {
        i = i - 1;
    }
    sleep(3);
    return 0;
}