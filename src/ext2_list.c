#include "ext2_list.h"
int ext2_init(int fd){
/* SUPER BLOCK */
	if (lseek(fd, (off_t)SUPER_BLOCK_OFFSET, SEEK_SET) < 0){
		perror("lseek superblock");
		return -1;
	}
	if (read(fd, &super_block, sizeof(struct ext2_super_block)) < 0){
		perror("read superblock");
		return -1;
	}
	if (super_block.s_magic != EXT2_SUPER_MAGIC){
		perror("magic number error");
		return -1;
	}
	// block size
	block_size = 1024 << super_block.s_log_block_size;

	return 0;
}
int find_inode(int fd, char *path, __u32 *inode_num){
	int depth;
	char **target_dir = divide_line(path, &depth, "/");

	__u32 cur_inode = 2;
	bool is_found = false;
	
	if (depth == 1 && !strcmp(target_dir[0], ".")){
		*inode_num = cur_inode;
		is_found = true;
		return 0;
	}

	for (int k=0; k<depth; k++){
		struct ext2_inode inode;
		
		if (!target_dir[k])
			return -1;
		if (read_inode(fd, cur_inode, &inode) < 0){
			perror("read inode");
			return -1;
		}
		
		// i_block[i]
		for (int i=0; i<EXT2_NDIR_BLOCKS; i++) {
			if (inode.i_block[i] == 0) {
				continue;
			}
			if (lseek(fd, (off_t) inode.i_block[i] * block_size, SEEK_SET) < 0){
				perror("lseek i-block");
				return -1;
			}

			int read_bytes = 0;
			while (read_bytes < block_size){
				struct ext2_dir_entry dir_entry;
				if (read(fd, &dir_entry, sizeof(struct ext2_dir_entry)) < 0){
					perror("read dir_entry");
					return -1;
				}
				if (!strncmp(target_dir[k], dir_entry.name, (size_t)dir_entry.name_len)){
					cur_inode = dir_entry.inode;
					is_found = true;
					break;
				}
				read_bytes += dir_entry.rec_len;
				if (lseek(fd, (off_t) inode.i_block[i] * block_size + read_bytes, SEEK_SET) < 0){
					perror("lseek i_block");
					return -1;
				}
			}
			if (is_found){
				if (k != depth -1) is_found = false;
				break;
			}
		}
	}
	if (cur_inode == 2)
		return -1;
	if (!is_found)
		return -1;
	*inode_num = cur_inode;
	return 0;
}
int scan_directory(int fd, FileNode **head, const FileNode *file_node, int depth) {
    struct ext2_inode inode;
    if (read_inode(fd, file_node->inode_num, &inode) < 0) {
        perror("read inode");
        return -1;
    }

    for (int i = 0; i < EXT2_NDIR_BLOCKS; i++) {
        if (inode.i_block[i] == 0) continue;

        unsigned char block[block_size];
        if (lseek(fd, (off_t)inode.i_block[i] * block_size, SEEK_SET) < 0) {
            perror("lseek i_block");
            return -1;
        }

        if (read(fd, block, block_size) != block_size) {
            perror("read block");
            return -1;
        }

        int offset = 0;
        while (offset < block_size) {
            struct ext2_dir_entry *entry = (struct ext2_dir_entry *)(block + offset);

            if (entry->rec_len < 8) {
                // rec_len이 너무 작으면 더 이상 읽지 않고 종료
                break;
            }

            if (entry->inode == 0 || entry->name_len == 0) {
                offset += entry->rec_len;
                continue;
            }

            if ((entry->name_len == 1 && entry->name[0] == '.') ||
                (entry->name_len == 2 && entry->name[0] == '.' && entry->name[1] == '.') ||
                (entry->name_len == 10 && !strncmp(entry->name, "lost+found", 10))) {
                offset += entry->rec_len;
                continue;
            }

            struct ext2_inode child_inode;
            if (read_inode(fd, entry->inode, &child_inode) < 0) {
                perror("read child inode");
                return -1;
            }

            FileNode *child_node = (FileNode *)calloc(1, sizeof(FileNode));
            child_node->inode_num = entry->inode;
            child_node->depth = depth + 1;
            child_node->is_recursive = file_node->is_recursive;
            memcpy(child_node->filename, entry->name, entry->name_len);
            child_node->filename[entry->name_len] = '\0';
            child_node->mode = child_inode.i_mode;
            child_node->size = child_inode.i_size;
            child_node->next = NULL;

            if (add_node(head, child_node) < 0) {
                perror("add node");
                return -1;
            }

            if (file_node->is_recursive && EXT2_S_ISDIR(child_inode.i_mode)) {
                if (scan_directory(fd, head, child_node, depth + 1) < 0) {
                    perror("scan child dir");
                    return -1;
                }
            }

            offset += entry->rec_len;
        }
    }

    return 0;
}
int read_inode(int fd, __u32 inode_num, struct ext2_inode *inode){
	struct ext2_group_desc group_desc;

	int group_num = (inode_num - 1) / super_block.s_inodes_per_group;
	off_t gd_offset = (block_size == 1024) ? 2048 : block_size;
	gd_offset += group_num * sizeof(struct ext2_group_desc);

	if (lseek(fd, (off_t)gd_offset, SEEK_SET) < 0){
		perror("lseek inode");
		return -1;
	}
	if (read(fd, &group_desc, sizeof(group_desc)) < 0){
		perror("read inode");
		return -1;
	}
	
	int index = (inode_num - 1) % super_block.s_inodes_per_group;
	off_t inode_offset = block_size * group_desc.bg_inode_table + index * super_block.s_inode_size;
	
	if (lseek(fd, (off_t)inode_offset, SEEK_SET) < 0){
		perror("lseek inode");
		return -1;
	}
	if (read(fd, inode, sizeof(struct ext2_inode)) < 0){
		perror("read inode");
		return -1;
	}

	return 0;
}
int add_node(FileNode **head, FileNode *new_node){
	if (*head == NULL){
		*head = new_node;
		return 0;
	}

	FileNode *node_p = *head;
	while (node_p->next != NULL){
		node_p = node_p->next;
	}
	
	node_p->next = new_node;
	return 0;
}
int free_list(FileNode **head){
	if (*head == NULL) return 0;

	FileNode *cur_node = *head;
	while (cur_node != NULL){
		FileNode *temp = cur_node;
		cur_node = cur_node->next;
		free(temp);
	}

	*head = NULL;
	return 0;
}
