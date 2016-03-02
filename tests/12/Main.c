/*
 * 测试内存限制，允许分配的内存为限制参数2倍，本测试用例分配了150M，在两倍范围内，能分配成功，最后被标记为内存超限
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    // 150m
    int big_size = 150 * 1024 * 1024;

    int *b = NULL;

    b = (int *)malloc(big_size);
    memset(b, 0, big_size);
    return 0;
}
