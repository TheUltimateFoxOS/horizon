#include <ext2_vfs.h>
#include <ext2_fs.h>

#include <utils/assert.h>

#include <memory/heap.h>

#include <utils/log.h>

using namespace fs;
using namespace fs::vfs;

ext2_mount::ext2_mount(int disk_id, char* name) {
	this->fs = (ext2_fs_t*) memory::malloc(sizeof(ext2_fs_t));
	memset(this->fs, 0, sizeof(ext2_fs_t));

	this->fs->drive_number = disk_id;

	this->fs->block_size = 1024;
	this->fs->sectors_per_block = 2;

	this->fs->superblock = (ext2_superblock_t*) memory::malloc(sizeof(ext2_superblock_t)); //Get the superblock data
	memset(this->fs->superblock, 0, sizeof(ext2_superblock_t));
	read_disk_block(this->fs, 1, (uint8_t*) this->fs->superblock);

	this->fs->block_size = 1024 << this->fs->superblock->log2_block_size;
	if (this->fs->block_size == 0) {
		this->fs->block_size = 1024;
		debugf("Warning: Block size is 0, using 1024 instead\n");
	}
	this->fs->sectors_per_block = this->fs->block_size / 512;
	if (this->fs->sectors_per_block == 0) {
		this->fs->sectors_per_block = 2;
		debugf("Warning: Sectors per block is 0, using 2 instead\n");
	}

	this->fs->block_buffer = (uint8_t*) memory::malloc(this->fs->block_size); //Create a buffer for reading/writing blocks

	uint32_t bgd_size = (this->fs->superblock->total_blocks / this->fs->superblock->blocks_per_group + 1) * sizeof(ext2_block_group_descriptor_t); //Get the information for the block group descs
	uint32_t bgd_blocks = bgd_size / this->fs->block_size;
	if (bgd_size % this->fs->block_size != 0) {
		bgd_blocks++;
	}

	#warning Loading all the block group descriptors into memory is a terrible design
	this->fs->block_group_descriptors = (ext2_block_group_descriptor_t*) memory::malloc(bgd_size);
	memset(this->fs->block_group_descriptors, 0, bgd_size);
	read_disk_blocks(this->fs, this->fs->superblock->log2_block_size == 0 ? 2 : 1, bgd_blocks, (uint8_t*) this->fs->block_group_descriptors);

	this->fs->inodes_per_block = this->fs->block_size / sizeof(ext2_inode_t);
	this->fs->inode_table_blocks = this->fs->superblock->inodes_per_group / this->fs->inodes_per_block;

	ext2_inode_t inode; //Check that the root node is actually a directory
	path_to_inode(this->fs, "/", &inode);
	assert(inode.mode & 0xF000 == 0x4000);
}

ext2_mount::~ext2_mount() {
	memory::free(this->fs->superblock);
	memory::free(this->fs);
}

file_t* ext2_mount::open(char* path) {
	debugf("Opening file %s\n", path);

	return nullptr; // <----- YES THIS BLOCKS
	file_t* file = new file_t;
	memset(file, 0, sizeof(file_t));

	file->mount = this;
	strcpy(file->buffer, path);
	file->data = memory::malloc(sizeof(ext2_inode_t));
	path_to_inode(this->fs, path, (ext2_inode_t*) file->data);
	file->size = ((ext2_inode_t*) file->data)->size; //TO BE CHECKED

	return file;
}

void ext2_mount::close(file_t* file) {
	debugf("Closing file\n");

	delete (ext2_inode_t*) file->data;
	delete file;
}

void ext2_mount::read(file_t* file, void* buffer, size_t size, size_t offset) {
	
}

void ext2_mount::write(file_t* file, void* buffer, size_t size, size_t offset) {
	
}

void ext2_mount::delete_(file_t* file) {
	
}

void ext2_mount::mkdir(char* path) {
	
}

dir_t ext2_mount::dir_at(int idx, char* path) {
	return dir_t {
		.is_none = true
	};
}

void ext2_mount::touch(char* path) {
	
}
