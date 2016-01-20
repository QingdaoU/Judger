#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
int main(int argc, char *argv[]) {
    int *a = NULL;
    int j;
    char *newargv[] = {"/", NULL};
    char *env[] = {NULL};
    printf("start\n");
    // 150M
    int v = 150000000;
    //fork();
    // printf("%s\n", getenv("LD_PRELOAD"));
    for (j = 0; j < argc; j++)
                   printf("argv[%d]: %s\n", j, argv[j]);
    //execve("/bin/echo", newargv, env);
    a = (int *) malloc(v);
    if (a == NULL) {
        printf("error\n");
    }
    else {
        memset(a, 0, v);
        printf("success\n");
    }
    printf("end\n");
    return 0;
}
