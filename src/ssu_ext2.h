#ifndef SSU_EXT2_H
#define SSU_EXT2_H

#define BUFFER_SIZE 8192

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define TRUE 1
#define FALSE 0

#define ALL 0
#define TREE 1
#define PRINT 2
#define HELP 3
#define EXIT 4

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <linux/types.h>
#include <stdbool.h>
#include <errno.h>

/* GLOBAL VARIABLES */
char img_path[PATH_MAX];

/* FUNCTIONS */
int prompt();
void print_usage(int command);

extern char **divide_line(char *str, int *argc, const char *del);
extern int tree(int argc, char *argv[]);
extern int print(int argc, char *argv[]);

#endif
