/*
 * 测试内存限制，允许分配的内存为限制参数2倍，本测试用例分配了300M，超过两倍范围内，不能分配成功，运行出错
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    // 300m
    int big_size = 300 * 1024 * 1024;

    int *b = NULL;

    b = (int *)malloc(big_size);
    memset(b, 0, big_size);
    return 0;
}
