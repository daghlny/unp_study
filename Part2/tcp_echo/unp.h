
#pragma once
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#define LISTENQ 10
#define MAXLINE 1024

void
err_sys(const char* errstr)
{
    printf("%s: %d(%s)\n", errstr, errno, strerror(errno));
    exit(-1);
}

