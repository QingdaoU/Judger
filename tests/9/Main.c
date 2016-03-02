/*
 * 测试环境变量参数
 */
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[]) {
    printf("%s\n%s\n", getenv("hello"), getenv("123"));
    return 0;
}
