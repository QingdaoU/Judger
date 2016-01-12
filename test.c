#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>
int main()
{
    int *a = NULL;
    // 150M
    int v = 150000000;
    a = (int *)malloc(v);
    if(a == NULL){
        printf("error\n");
    }
    else {
        memset(a, 0, v);
        printf("success\n");
    }
    return 15;
}
