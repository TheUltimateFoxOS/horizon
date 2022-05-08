#pragma once

#include <stdint.h>

#include <driver/disk.h>

#define ROOT_DIRENT_BLOCK 1

#define BLOCK_SIZE 1024 //Only use multiples of 512!
#define BLOCK_DATA_SIZE (BLOCK_SIZE - 8)
#define SECTORS_PER_BLOCK (BLOCK_SIZE / 512)

#define FOXFS_DIR 0x01
#define FOXFS_FIL 0x02

namespace fs {
	using namespace vfs;

	struct foxfs_data_block_t {
		uint8_t data[BLOCK_DATA_SIZE];
		uint64_t next;
	};

	struct foxfs_dirent_t {
		char name[256];
		uint8_t file_type;
		uint32_t size;
		uint64_t parent_address; //This is a memory address for ram and a block number for disk
		size_t parent_data_offset;
		uint64_t data_address; //This is a memory address for ram and a block number for disk
	} __attribute__((packed));

	struct foxfs_fs_t {
		bool ramfs;
		int disk_id;
		foxfs_dirent_t* root_node;
	};

	int read_disk_block(foxfs_fs_t* fs, uint32_t block, uint8_t* buffer);
    int write_disk_block(foxfs_fs_t* fs, uint32_t block, uint8_t* buffer);

	uint64_t create_data_block(foxfs_fs_t* fs);

	int read_dirent_data(foxfs_fs_t* fs, uint64_t data_address, void* buffer, size_t size, size_t offset);
	int update_dirent_read(foxfs_fs_t* fs, foxfs_dirent_t* dirent);
	int read_dirent(foxfs_fs_t* fs, foxfs_dirent_t* dirent, void* buffer, size_t size, size_t offset);

	int write_dirent_data(foxfs_fs_t* fs, uint64_t data_address, void* buffer, size_t size, size_t offset);
	int update_dirent_write(foxfs_fs_t* fs, foxfs_dirent_t* dirent);
	int write_dirent(foxfs_fs_t* fs, foxfs_dirent_t* dirent, void* buffer, size_t size, size_t offset);

	int readdir(foxfs_fs_t* fs, foxfs_dirent_t* dirent, int idx, foxfs_dirent_t* child_dir);
	int path_to_dirent(foxfs_fs_t* fs, char* path, foxfs_dirent_t* dirent);
	int create_dirent(foxfs_fs_t* fs, char* path, uint8_t file_type);
}