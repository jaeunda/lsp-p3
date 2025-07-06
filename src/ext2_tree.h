#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <linux/types.h>
#include <fcntl.h>

#include "ext2_fs.h"

#ifndef NAME_MAX
#define NAME_MAX 256
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#ifndef HELP_CMD
#define HELP_CMD
#define ALL 0
#define TREE 1
#endif

/* OPTIONS */
#define OPT_R 0b0001
#define OPT_S 0b0010
#define OPT_P 0b0100
// #define OPT_N 0b1000

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
extern char img_path[PATH_MAX];

/* FUNCTIONS */
int tree(int argc, char *argv[]);
int get_option(int argc, char **argv);
int print_tree(FileNode **head, int opt);
int get_permission(char *perms, __u16 mode);

extern void print_usage(int command);
extern int ext2_init(int fd);
extern int find_inode(int fd, char *path, __u32 *inode_num);
extern int read_inode(int fd, __u32 inode_num, struct ext2_inode *inode);
extern int scan_directory(int fd, FileNode **head, const FileNode *file_node, int depth);
extern int add_node(FileNode **head, FileNode *new_node);
extern int free_list(FileNode **head);

#endif

