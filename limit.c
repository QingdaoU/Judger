#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>
int main()
{
    int *a = NULL;
    int v = 150;
    struct rlimit memory_limit;
    memory_limit.rlim_cur = memory_limit.rlim_max = 90000000;
    setrlimit(RLIMIT_AS, &memory_limit);
    printf("111111111111111\n");
    a = (int *)malloc(v);
    printf("222222222222222\n");
    if(a == NULL){
        printf("error\n");
    }
    else {
        memset(a, 0, v);
        printf("success\n");
    }
    return 0;
}
