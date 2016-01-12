#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[]) {
    int *a = NULL;
    // 150M
    int v = 150000000;
    printf("%s\n", getenv("LD_PRELOAD"));
    for (int j = 0; j < argc; j++)
                   printf("argv[%d]: %s\n", j, argv[j]);
    a = (int *) malloc(v);
    if (a == NULL) {
        printf("error\n");
    }
    else {
        memset(a, 0, v);
        printf("success\n");
    }
    return 15;
}
