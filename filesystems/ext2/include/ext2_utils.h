#pragma once

#include <fs/vfs.h>
#include <driver/disk.h>
#include <ext2_fs.h>

namespace fs {
	using namespace vfs;

	bool is_ext2(ext2_superblock_t* superblock);
	bool is_ext2(int disk_id, uint8_t* buffer);
}