#ifndef EXT2_UTIL_H
#define EXT2_UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#ifndef NAME_MAX
#define NAME_MAX 256
#endif

char **divide_line(char *str, int *argc, const char *del);

#endif
