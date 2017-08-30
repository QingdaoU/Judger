#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    int flags;
    if (!strcmp(argv[2], "w")) {
        flags = O_WRONLY | O_CREAT;
    }
    else {
        flags = O_RDWR | O_CREAT;
    }
    int fd = openat(0, argv[1], flags, 0755);
    if (fd < 0) {
        return errno;
    }
    close(fd);
    return 0;
}
