#ifndef EXT2_FS_H
#define EXT2_FS_H

#define SUPER_BLOCK_OFFSET 1024
#define EXT2_SUPER_MAGIC 0xEF53
#define EXT2_NAME_LEN 255

#define EXT2_NDIR_BLOCKS 12
#define EXT2_IND_BLOCK EXT2_NDIR_BLOCKS
#define EXT2_DIND_BLOCK (EXT2_IND_BLOCK + 1)
#define EXT2_TIND_BLOCK (EXT2_DIND_BLOCK + 1)
#define EXT2_N_BLOCKS (EXT2_TIND_BLOCK + 1)

#define EXT2_S_IFMT 0xF000
#define EXT2_S_IFDIR 0x4000
#define EXT2_S_IFREG 0x8000

#define EXT2_S_ISDIR(m) (((m) & EXT2_S_IFMT) == EXT2_S_IFDIR)
#define EXT2_S_ISREG(m) (((m) & EXT2_S_IFMT) == EXT2_S_IFREG)

struct ext2_super_block {
	__u32 s_inodes_count;
	__u32 s_blocks_count;
	__u32 s_r_blocks_count;
	__u32 s_free_blocks_count;
	__u32 s_free_inodes_count;
	__u32 s_first_data_block;
	__u32 s_log_block_size;
	__s32 s_log_frag_size;
	__u32 s_blocks_per_group;
	__u32 s_frags_per_group;
	__u32 s_inodes_per_group;
	__u32 s_mtime;
	__u32 s_wtime;
	__u16 s_mnt_count;
	__s16 s_max_mnt_count;
	__u16 s_magic;
	__u16 s_state;
	__u16 s_errors;
	__u16 s_minor_rev_level;
	__u32 s_lastcheck;
	__u32 s_checkinterval;
	__u32 s_creator_os;
	__u32 s_rev_level;
	__u16 s_def_resuid;
	__u16 s_def_resgid;
	__u32 s_first_ino;
	__u16 s_inode_size;
};
struct ext2_group_desc {
	__u32 bg_block_bitmap; 
	__u32 bg_inode_bitmap;
	__u32 bg_inode_table;
	__u16 bg_free_blocks_count;
	__u16 bg_free_inodes_count;
	__u16 bg_used_dirs_count;
	__u16 bg_flags;
	__u32 be_reserved[2];
	__u16 bg_itable_unused;
	__u16 bg_checksum;
};
struct ext2_inode {
	__u16 i_mode;
	__u16 i_uid;
	__u32 i_size;
	__u32 i_atime;
	__u32 i_ctime;
	__u32 i_mtime;
	__u32 i_dtime;
	__u16 i_gid;
	__u16 i_links_count;
	__u32 i_blocks;
	__u32 i_flags;
	union {
		struct {
			__u32 l_i_version;
		} linux1;
		struct {
			__u32 h_i_translator;
		} hurd1;
		struct {
			__u32 m_i_reserved1;
		} masix1;
	} osdi;
	__u32 i_block[EXT2_N_BLOCKS];
};
struct ext2_dir_entry {
	__u32 inode;
	__u16 rec_len;
	__u8 name_len;
	__u8 file_type;
	char name[EXT2_NAME_LEN];
};
#endif


