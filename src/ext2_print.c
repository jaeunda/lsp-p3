#include "ext2_print.h"
int print(int argc, char **argv){
	char root_path[PATH_MAX];
	strcpy(root_path, argv[1]);

	// path validation
	if (strlen(root_path) == 0 || root_path[0] == '-'){
		print_usage(PRINT);
		return -1;
	}

	// parse option
	int opt = 0;
	if ((opt = get_line_num(argc, argv)) < 0){
		print_usage(PRINT);
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
		print_usage(PRINT);
		close(fd);
		return -1;
	}

	// is regular file
	struct ext2_inode root_inode;
	if (read_inode(fd, inode_num, &root_inode) < 0){
		perror("read inode");
		close(fd);
		return -1;
	}
	if (!EXT2_S_ISREG(root_inode.i_mode)){
		fprintf(stderr, "Error: Input path is not a file\n\n");
		close(fd);
		return -1;
	}

	// print contents
	if (print_contents(fd, inode_num, opt) < 0){
		perror("print_contents");
		close(fd);
		return -1;
	}
	printf("\n");

	close(fd);

	return 0;
}
int print_contents(int fd, __u32 inode_num, int opt){
	struct ext2_inode inode;
	if (read_inode(fd, inode_num, &inode) < 0){
		perror("print_contents");
		return -1;
	}

	int printed_lines = 0;

	// Direct blocks
	for (int i=0; i<12; i++){
		if (inode.i_block[i] == 0) continue;
		int result = print_block(fd, inode.i_block[i], &printed_lines, opt);
		if (result < 0)
			return -1;
		if (result == 1)
			return 0;
	}

	// Single indirect
	if (inode.i_block[12] != 0){
		if (print_indirect_blocks(fd, inode.i_block[12], 1, &printed_lines, opt) == 1)
			return 0;

	}

	// Double indirect
	if (inode.i_block[13] != 0){
		if (print_indirect_blocks(fd, inode.i_block[13], 2, &printed_lines, opt) == 1)
			return 0;
	}

	// Triple indirect
	if (inode.i_block[14] != 0){
		if (print_indirect_blocks(fd, inode.i_block[14], 3, &printed_lines, opt) == 1)
			return 0;
	}
	
	return 0;
}
int get_line_num(int argc, char **argv){
	// RETURN VALUE
		// 		  -n: line number
		// no option: 0
		//     error: -1
	int opt;
	int n_value = 0;

	if (argc == 2){
		return 0;
	}

	optind = 0;
	optind = 2;
	opterr = 0;
	optopt = 0;
	optarg = NULL;

	while ((opt = getopt(argc, argv, "n:")) != -1){
		switch (opt) {
			case 'n':
				if (optarg == NULL){
					fprintf(stderr, "Error: -n requires an argument\n\n");
					return -1;
				}
				for (int i=0; optarg[i] != '\0'; i++){
					if (!isdigit(optarg[i])){
						fprintf(stderr, "Error: -n option requires a positive integer\n\n");
						return -1;
					}
				}
				n_value = atoi(optarg);
				if (n_value <= 0){
					fprintf(stderr, "Error: -n option requires a positive integer\n\n");	
					return -1;
				}
				break;
			case '?':
				if (optopt == 'n')
					fprintf(stderr, "Error: -n requires an argument\n\n");
				else
					fprintf(stderr, "Error: Invalid option -- '%c'\n\n", optopt);
				return -1;
			default:
				return 0;
		}
	}
	return n_value;
}
int print_block(int fd, __u32 block_num, int *printed_lines, int opt){
	char *block_buf = (char *)calloc((size_t)1, (size_t)(block_size+1));
	if (!block_buf){
		perror("block_buf calloc");
		return -1;
	}

	if (lseek(fd, (off_t)block_num * block_size, SEEK_SET) < 0){
		perror("print_block()");
		free(block_buf);
		return -1;
	}
	
	int read_size;
	if ((read_size = read(fd, (char *)block_buf, (size_t)block_size)) < 0){
		perror("print_block()");
		free(block_buf);
		return -1;
	}
	block_buf[read_size] = '\0';

	char *line = strtok(block_buf, "\n");
	while (line != NULL){
		printf("%s\n", line);
		(*printed_lines)++;

		if (opt > 0 && (*printed_lines) >= opt){
			free(block_buf);
			return 1;
		}
		line = strtok(NULL, "\n");
	}
	free(block_buf);
	return 0;
}
int print_indirect_blocks(int fd, __u32 block_num, int level, int *printed_lines, int opt){
	if (level == 0){
		return print_block(fd, block_num, printed_lines, opt);
	}

	__u32 *block_entries = (__u32 *)calloc((size_t)1, (size_t)block_size);
	if (!block_entries){
		perror("block entry calloc");
		return -1;
	}

	if (lseek(fd, (off_t)block_num * block_size, SEEK_SET) < 0){
		perror("print_indirect_blocks()");
		free(block_entries);
		return -1;
	}

	if (read(fd, (__u32 *)block_entries, (size_t)block_size) < 0){
		perror("print_indirect_blocks()");
		free(block_entries);
		return -1;
	}

	int entries = block_size / sizeof(__u32);
	for (int i=0; i<entries; i++){
		if (block_entries[i] == 0)
			continue;
		int result;
		if ((result = print_indirect_blocks(fd, block_entries[i], level-1, printed_lines, opt)) == 1){
			free(block_entries);
			return 1;
		}
	}
	free(block_entries);
	return 0;
}
