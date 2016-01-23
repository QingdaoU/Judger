#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[]) {
    int *p = NULL;
    int j;
    char input[100];
    // 150M
    int v = 150000000;

    printf("start\n");

    for (j = 0; j < argc; j++)
        printf("argv[%d]: %s\n", j, argv[j]);

    scanf("%s", input);
    printf("%s\n", input);

    p = (int *) malloc(v);
    if (p == NULL) {
        printf("malloc failed\n");
    }
    else {
        memset(p, 0, v);
        printf("malloc succeeded\n");
    }
    printf("begin to execute command\n");
    system("/bin/ls /tmp");
    return 0;
}
