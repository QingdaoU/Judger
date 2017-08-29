#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    DIR *dir = opendir(argv[1]);  
    int dir_fd = dirfd(dir);  
    int flags;
    if (!strcmp(argv[3], "w")) {
        flags = O_WRONLY | O_CREAT;
    }
    else {
        flags = O_RDWR | O_CREAT;
    }
    int fd = openat(dir_fd, argv[2], flags, 0755);
    if (fd < 0) {
        return errno; 
    }
    close(fd);
    return 0;
}