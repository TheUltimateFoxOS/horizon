#pragma once

#include <fs/vfs.h>

#include <driver/disk.h>

#define EXT2_DIRECT_BLOCKS 12
#define EXT2_ROOT_INODE 2

#define EXT2_FIFO 0x1000
#define EXT2_CHAR 0x2000
#define EXT2_DIR 0x4000
#define EXT2_BLKD 0x6000
#define EXT2_FILE 0x8000
#define EXT2_LINK 0xA000
#define EXT2_SOCK 0xC000

namespace fs {
	struct ext2_superblock_t {
		uint32_t total_inodes;
		uint32_t total_blocks;

		uint32_t su_reserved_blocks;

		uint32_t free_blocks;
		uint32_t free_inodes;

		uint32_t first_data_block;

		uint32_t log2_block_size;
		uint32_t log2_fragment_size;

		uint32_t blocks_per_group;
		uint32_t fragments_per_group;
		uint32_t inodes_per_group;

		uint32_t last_mount_time;
		uint32_t last_write_time;

		uint16_t mount_count;
		uint16_t max_mount_count;

		uint16_t magic;
		uint16_t state;
		uint16_t error;

		uint16_t minor_revision_level;

		uint32_t last_check_time;
		uint32_t interval_between_checks;
		uint32_t os_id;

		uint16_t major_revision_level;
		
		uint16_t su_user_id;
		uint16_t su_group_id;

		uint8_t unuused[940];
	} __attribute__((packed));

	struct ext2_block_group_descriptor_t {
		uint32_t block_bitmap;
		uint32_t inode_bitmap;
		uint32_t inode_table;

		uint16_t free_blocks;
		uint16_t free_inodes;
		uint16_t num_dirs;

		uint16_t padding;
		uint8_t reserved[3];
	} __attribute__((packed));

	struct ext2_inode_t {
		uint16_t mode;
		uint16_t user_id;

		uint32_t size;

		uint32_t last_access;
		uint32_t creation_time;
		uint32_t last_modification;
		uint32_t deletion_time;

		uint16_t group_id;

		uint16_t hard_links;
		uint32_t disk_sectors;
		uint32_t flags;

		uint32_t os_specific_1;
		
		uint32_t direct_blocks[EXT2_DIRECT_BLOCKS];
		uint32_t singly_block;
		uint32_t doubly_block;
		uint32_t triply_block;

		uint32_t generation_number;
		
		uint32_t file_acl;
		uint32_t dir_acl;

		uint32_t fragment_block;

		uint16_t blocks_high;
		uint16_t file_acl_high;
		uint16_t uid_high;
		uint16_t gid_high;
		uint16_t checksum_lo;
		uint16_t reserved;
	} __attribute__((packed));

	struct ext2_dir_t {
		uint32_t inode;
		uint16_t size;
		uint8_t name_length;
		uint8_t file_type;
		char name[];
	} __attribute__((packed));

	struct ext2_fs_t {
		int drive_number = 0;

		ext2_superblock_t* superblock;
		uint8_t* block_buffer;

		uint32_t block_size;
		uint32_t sectors_per_block;

		ext2_block_group_descriptor_t* block_group_descriptors;

		uint32_t inodes_per_block;
		uint32_t inode_table_blocks;
	} __attribute__((packed));

	void read_disk_block(ext2_fs_t* fs, uint32_t block, uint8_t* buffer);
    void write_disk_block(ext2_fs_t* fs, uint32_t block, uint8_t* buffer);

	void read_disk_blocks(ext2_fs_t* fs, uint32_t block, uint32_t count, uint8_t* buffer);
	void write_disk_blocks(ext2_fs_t* fs, uint32_t block, uint32_t count, uint8_t* buffer);

	void read_disk(ext2_fs_t* fs, uint32_t address, uint32_t size, uint8_t* buffer);
	void write_disk(ext2_fs_t* fs, uint32_t address, uint32_t size, uint8_t* buffer);

	void read_inode(ext2_fs_t* fs, uint32_t inode_idx, ext2_inode_t* inode);
	void write_inode(ext2_fs_t* fs, uint32_t inode_idx, ext2_inode_t* inode);

	void readdir(ext2_fs_t* fs, int idx, ext2_inode_t* inode, ext2_dir_t* child_dir);
	void path_to_inode(ext2_fs_t* fs, char* path, ext2_inode_t* inode);
}