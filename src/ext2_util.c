#include "ext2_util.h"

char **divide_line(char *str, int *argc, const char *del){
    *argc = 0;
    char *token = strtok(str, del);
    if (!token) return NULL;

    char **argv = (char **)calloc(128, sizeof(char *)); 
    while (token != NULL){
        argv[*argc] = strdup(token);
        (*argc)++;
        token = strtok(NULL, del);
    }
    argv[*argc] = NULL;
    return argv;
}
