#include <ext2_utils.h>

#include <driver/clock.h>
#include <driver/disk.h>

using namespace fs;

bool fs::is_ext2(ext2_superblock_t* superblock) {
	if (superblock == nullptr) {
		return false;
	}
	if (superblock == 0) {
		return false;
	}

	if (superblock->magic != 0xef53) {
		return false;
	}
	if (superblock->major_revision_level != 1) {
		return false;
	}

	if (superblock->total_blocks == 0 || superblock->blocks_per_group == 0) {
		return false;
	}
	if (1024 << superblock->log2_block_size == 0) {
		return false;
	}

	#warning This doesn't tell us if it's ext2 3 or 4 and a whole bunch of other things

	return true;
}

bool fs::is_ext2(int disk_id, uint8_t* buffer) {
	driver::global_disk_manager->read(disk_id, 2, 2, (void*) buffer);
	return is_ext2((ext2_superblock_t*) buffer);
}