#include <stdio.h>
#include <errno.h>
int main(int argc, char *argv[])
{
    FILE *f = fopen(argv[1], argv[2]);
    if (f == NULL) {
        return errno;
    }
    return 0;
}