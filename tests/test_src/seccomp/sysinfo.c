// maybe used in qsort function
#include <unistd.h>
#include <stdio.h>

int main() {
    printf("%ld", sysconf(_SC_PAGE_SIZE));
    return 0;
}
