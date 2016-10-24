#include <stdio.h>
int main()
{
    FILE *f = fopen("/tmp/fffffffffffffile.txt", "w");
    if (f == NULL) {
        return 1;
    }
    fprintf(f, "%s", "test");
    return 0;
}