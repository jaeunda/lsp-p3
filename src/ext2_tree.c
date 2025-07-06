#include "ext2_tree.h"
int tree(int argc, char *argv[]){
	char root_path[PATH_MAX];
	strcpy(root_path, argv[1]);

	// path
	if (strlen(root_path) == 0 || root_path[0] == '-'){
		print_usage(TREE);
		return -1;
	}

	// parse option
	int opt = 0;
	if ((opt = get_option(argc, argv)) < 0){
		print_usage(TREE);
		return -1;
	}

	// open
	int fd;
	if ((fd = open(img_path, O_RDONLY)) < 0){
		fprintf(stderr, "Error: cannot open the ext2 img\n\n");
		return -1;
	}

	// ext2 init
	if (ext2_init(fd) < 0){
		perror("ext2 init");
		close(fd);
		return -1;
	}

	// find inode
	__u32 inode_num = 0;
	if (find_inode(fd, root_path, &inode_num) < 0){
		fprintf(stderr, "Error: Path does not exist\n\n");
		print_usage(TREE);
		close(fd);
		return -1;
	}

	// is directory
	struct ext2_inode root_inode;
	if (read_inode(fd, inode_num, &root_inode) < 0){
		perror("read inode");
		close(fd);
		return -1;
	}

	if (!EXT2_S_ISDIR(root_inode.i_mode)){
		fprintf(stderr, "Error: Input path is not a directory\n\n");
		close(fd);
		return -1;
	}

	// add input directory to list
	FileNode *head = NULL;
	
	// init
	FileNode *root_node = (FileNode *)calloc((size_t)1, sizeof(FileNode));
	root_node->inode_num = inode_num;
	root_node->depth = 0;
	root_node->is_recursive = (opt & OPT_R) ? true : false;
	strcpy(root_node->filename, root_path);
	root_node->mode = root_inode.i_mode;
	root_node->size = root_inode.i_size;
	root_node->next = NULL;

	if (add_node(&head, root_node) < 0){
		perror("add node");
		close(fd);
		return -1;
	}

	// scan directory
	if (scan_directory(fd, &head, root_node, 0) < 0){
		perror("scan directory");
		close(fd);
		return -1;
	}

	// print list
	if (print_tree(&head, opt) < 0){
		perror("print tree");
		close(fd);
		return -1;
	}

	close(fd);
	
	return 0;
}
int get_option(int argc, char **argv){
	int opt;
	int ret = 0;

	if (argc == 2){
		return ret;
	}

	optind = 0;
	optind = 2;
	opterr = 0;
	optopt = 0;
	optarg = NULL;

	while ((opt = getopt(argc, argv, "rsp")) != -1){
		switch (opt){
			case 'r':
				ret |= OPT_R;
				break;
			case 's':
				ret |= OPT_S;
				break;
			case 'p':
				ret |= OPT_P;
				break;
			case '?':
				fprintf(stderr, "Error: Invalid option\n\n");
				return -1;
			default:
				fprintf(stderr, "Error: Invalid option\n\n");
				return -1;
		}
	}
	return ret;
}
int print_tree(FileNode **head, int opt){
    int directory_cnt = 0;
    int file_cnt = 0;

    if (*head == NULL){
        perror("head is null");
        return -1;
    }

    FileNode *cur_node = *head;
    FileNode *next_node = NULL;

    while (cur_node != NULL){
        char perms[11] = "----------";
        if (get_permission(perms, cur_node->mode) < 0){
            perror("get permission");
            return -1;
        }

        // count
        if (S_ISDIR(cur_node->mode))
            directory_cnt++;
        else
            file_cnt++;

        // determine whether this is the last sibling
        int is_last = 1;
        next_node = cur_node->next;
        while (next_node) {
            if (next_node->depth < cur_node->depth) break;
            if (next_node->depth == cur_node->depth) {
                is_last = 0;
                break;
            }
            next_node = next_node->next;
        }

        // indentation and tree branches
        for (int i = 0; i < cur_node->depth - 1; i++) {
            // check if vertical bar should be drawn
            FileNode *check = cur_node->next;
            int need_bar = 0;
            while (check) {
                if (check->depth == i + 1) {
                    need_bar = 1;
                    break;
                }
                if (check->depth <= i) break;
                check = check->next;
            }
            printf(need_bar ? "│   " : "    ");
        }

        if (cur_node->depth > 0) {
            printf(is_last ? "└── " : "├── ");
        }

        // filename and optional info
        if ((opt & OPT_S) && (opt & OPT_P))
            printf("[%s %d] %s\n", perms, cur_node->size, cur_node->filename);
        else if (opt & OPT_S)
            printf("[%d] %s\n", cur_node->size, cur_node->filename);
        else if (opt & OPT_P)
            printf("[%s] %s\n", perms, cur_node->filename);
        else
            printf("%s\n", cur_node->filename);

        cur_node = cur_node->next;
    }

    printf("\n%d directories, %d files\n\n", directory_cnt, file_cnt);

    if (free_list(head) < 0){
		perror("free list");
        return -1;
    }
    return 0;
}
int get_permission(char *perms, __u16 mode){
	// perms[11] = "-----------";

	if (S_ISDIR(mode)) perms[0] = 'd';
    if (S_ISLNK(mode)) perms[0] = 'l';

    if (mode & S_IRUSR) perms[1] = 'r';  
    if (mode & S_IWUSR) perms[2] = 'w'; 
    if (mode & S_IXUSR) perms[3] = 'x';

    if (mode & S_IRGRP) perms[4] = 'r';
    if (mode & S_IWGRP) perms[5] = 'w';
    if (mode & S_IXGRP) perms[6] = 'x';

    if (mode & S_IROTH) perms[7] = 'r';
    if (mode & S_IWOTH) perms[8] = 'w';
    if (mode & S_IXOTH) perms[9] = 'x';

	return 0;
}
