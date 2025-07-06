#ifndef EXT2_LIST_H
#define EXT2_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <linux/types.h>
#include <sys/stat.h>

#include "ext2_fs.h"

#ifndef NAME_MAX
#define NAME_MAX 256
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

extern char **divide_line(char *str, int *argc, const char *del);

/* LINKED_LIST */
#ifndef FILE_NODE
#define FILE_NODE
typedef struct _node {
	__u32 inode_num;
	int depth;
	bool is_recursive;
	char filename[NAME_MAX];
	__u16 mode;
	__u32 size;
	struct _node *next;
} FileNode;
#endif

/* GLOBAL VARIABLES */
struct ext2_super_block super_block;
int block_size;

int ext2_init(int fd);
int find_inode(int fd, char *path, __u32 *inode_num);
int scan_directory(int fd, FileNode **head, const FileNode *file_node, int depth);
int read_inode(int fd, __u32 inode_num, struct ext2_inode *inode);
int add_node(FileNode **head, FileNode *new_node);
int free_list(FileNode **head);

#endif
