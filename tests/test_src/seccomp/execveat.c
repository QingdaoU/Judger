#define _GNU_SOURCE
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/syscall.h>


int execveat_(int fd, const char *path, char **argv, char **envp, int flags)
{
#ifdef __NR_execveat
	return syscall(__NR_execveat, fd, path, argv, envp, flags);
#endif
}

int main() {
#ifndef __NR_execveat
    printf("syscall not found");
    return 0;
#else
    char *envp[] = {"test=1", NULL};
    char *argv[] = {"hello", NULL};

    execveat_(1, "/bin/true", argv, envp, 0);
    printf("failed %d", errno);
    return 1;
#endif
}
