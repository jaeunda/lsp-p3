#ifndef PRINT_H
#define PRINT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <linux/types.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>

#include "ext2_fs.h"

#ifndef PRINT_CMD
#define PRINT_CMD
#define ALL 0
#define PRINT 2
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#ifndef BLOCK_LIMIT
#define BLOCK_LIMIT 15
#endif

extern char img_path[PATH_MAX];
extern struct ext2_super_block super_block;
extern int block_size;

extern void print_usage(int command);
extern int ext2_init(int fd);
extern int find_inode(int fd, char *path, __u32 *inode_num);
extern int read_inode(int fd, __u32 inode_num, struct ext2_inode *inode);

int print(int argc, char **argv);
int get_line_num(int argc, char **argv);

int print_contents(int fd, __u32 inode_num, int opt);
int print_block(int fd, __u32 block_num, int *printed_lines, int opt);
int print_indirect_blocks(int fd, __u32 block_num, int level, int *printed_lines, int opt);

#endif
