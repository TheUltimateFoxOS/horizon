#include <ext2_fs.h>

#include <utils/string.h>

#include <driver/disk.h>

#include <memory/heap.h>

#include <utils/log.h>

using namespace fs;

void clear_buffer(ext2_fs_t* fs) {
    memset(fs->block_buffer, 0, fs->block_size);
}

void fs::read_disk_block(ext2_fs_t* fs, uint32_t block, uint8_t* buffer) {
	driver::global_disk_manager->read(fs->drive_number, block * fs->sectors_per_block, fs->sectors_per_block, (void*) buffer);
}

void fs::write_disk_block(ext2_fs_t* fs, uint32_t block, uint8_t* buffer) {
	driver::global_disk_manager->write(fs->drive_number, block * fs->sectors_per_block, fs->sectors_per_block, (void*) buffer);
}

void fs::read_disk_blocks(ext2_fs_t* fs, uint32_t block, uint32_t count, uint8_t* buffer) {
    driver::global_disk_manager->read(fs->drive_number, block * fs->sectors_per_block, count * fs->sectors_per_block, (void*) buffer);
}

void fs::write_disk_blocks(ext2_fs_t* fs, uint32_t block, uint32_t count, uint8_t* buffer) {
    driver::global_disk_manager->write(fs->drive_number, block * fs->sectors_per_block, count * fs->sectors_per_block, (void*) buffer);
}

void fs::read_disk(ext2_fs_t* fs, uint32_t address, uint32_t size, uint8_t* buffer) { //Read a specific portion of the disk
    uint32_t sector = address / 512;
    uint32_t offset = address % 512;
    uint32_t copied = 0;

    void* copy_buffer = memory::malloc(512);
    
    while (copied != size) {
        memset(copy_buffer, 0, 512);
        driver::global_disk_manager->read(fs->drive_number, sector, 1, copy_buffer);

        uint32_t to_copy = 512 - offset;
        if (to_copy > size - copied) {
            to_copy = size - copied;
        }

        memcpy(buffer + copied, (void*) (((uint64_t) copy_buffer) + offset), to_copy);
        copied += to_copy;

        sector++;
        offset = 0;
    }

    memory::free(copy_buffer);
}

void fs::write_disk(ext2_fs_t* fs, uint32_t address, uint32_t size, uint8_t* buffer) { //Write a specific portion of the disk
    uint32_t sector = address / 512;
    uint32_t offset = address % 512;
    uint32_t copied = 0;

    void* copy_buffer = memory::malloc(512);
    
    while (copied != size) {
        memset(copy_buffer, 0, 512);

        if (offset != 0) {
            driver::global_disk_manager->read(fs->drive_number, sector, 1, copy_buffer);
        }

        memcpy((void*) (((uint64_t) copy_buffer) + offset), buffer + copied, 512 - offset);
        driver::global_disk_manager->write(fs->drive_number, sector, 1, copy_buffer);

        copied += 512 - offset;

        sector++;
        offset = 0;
    }

    memory::free(copy_buffer);
}

void fs::read_inode(ext2_fs_t* fs, uint32_t inode_idx, ext2_inode_t* inode) { //Read an inode from the disk
    uint32_t block_group = (inode_idx - 1) / fs->superblock->inodes_per_group;
    uint32_t inode_index = (inode_idx - 1) % fs->superblock->inodes_per_group;

    ext2_block_group_descriptor_t* bgd = fs->block_group_descriptors + block_group;
    uint32_t inode_table_block = bgd->inode_table;

    uint32_t offset = inode_index * sizeof(ext2_inode_t);
    uint32_t block = inode_table_block + (offset / fs->block_size);
    offset %= fs->block_size;

    clear_buffer(fs);
    read_disk_block(fs, block, fs->block_buffer);
    memcpy(inode, fs->block_buffer + offset, sizeof(ext2_inode_t));
}

void fs::write_inode(ext2_fs_t* fs, uint32_t inode_idx, ext2_inode_t* inode) { //Write an inode to the disk
    uint32_t block_group = (inode_idx - 1) / fs->superblock->inodes_per_group;
    uint32_t inode_index = (inode_idx - 1) % fs->superblock->inodes_per_group;

    clear_buffer(fs);
    
    ext2_block_group_descriptor_t* bgd = fs->block_group_descriptors + block_group;
    uint32_t inode_table_block = bgd->inode_table;

    uint32_t offset = inode_index * sizeof(ext2_inode_t);
    uint32_t block = inode_table_block + (offset / fs->block_size);
    offset %= fs->block_size;

    clear_buffer(fs);
    read_disk_block(fs, block, fs->block_buffer);
    memcpy(inode, fs->block_buffer + offset, sizeof(ext2_inode_t));
    write_disk_block(fs, block, fs->block_buffer);
}

void fs::readdir(ext2_fs_t* fs, int idx, ext2_inode_t* inode, ext2_dir_t* child_dir) { //Read a directory entry from a parent inode
    memset(child_dir, 0, sizeof(ext2_dir_t)); //Set the inode to 0

    if (inode->mode & 0xF000 != 0x4000) { //Don't try to read anything that isn't a directory
        return;
    }
}

void fs::path_to_inode(ext2_fs_t* fs, char* path, ext2_inode_t* inode) {
    memset(inode, 0, sizeof(ext2_inode_t)); //Set the inode to 0

    if (path[0] != '/') { //We can only handle absolute paths
        return;
    }

    ext2_inode_t tmp_inode;
	memset(&inode, 0, sizeof(ext2_inode_t));
	read_inode(fs, EXT2_ROOT_INODE, &tmp_inode);

    if (path[1] == 0) { //We are looking for root, so we return the root inode
        memcpy(inode, &tmp_inode, sizeof(ext2_inode_t));
        return;
    } else {
        //TODO
        return;
    }
}