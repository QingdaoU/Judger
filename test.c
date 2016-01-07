#include <unistd.h>
#include <stdio.h>

int main()
{
    char input[1000];
    scanf("%s", input);
    printf("%s", input);
    int i = 900000000;
    while (i) {
        i = i - 1;
    }
    sleep(3);
    return 0;
}