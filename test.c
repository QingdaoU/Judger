#include <unistd.h>
#include <stdio.h>
#include <signal.h>


int main()
{
    char input[1000];
    //scanf("%s", input);
    //printf("%s", input);
    printf("Hello world\n");
    int i = 900000000;
    while (i) {
        i = i - 1;
    }
    sleep(8);
    return 0;
}