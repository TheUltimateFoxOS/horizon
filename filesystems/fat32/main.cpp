#include <elf/kernel_module.h>

#include <driver/driver.h>
#include <driver/disk.h>

#include <fs/vfs.h>
#include <fat32_vfs.h>
#include <fat32_utils.h>

#include <memory/page_frame_allocator.h>

#include <input/input.h>

#include <utils/log.h>
#include <utils/string.h>

void init() {}

void device_init() {}

void fs_init() {
	debugf("Searching for FAT32 filesystems...\n");
	for (int i = 0; i < driver::global_disk_manager->num_disks; i++) {
		if (!fs::is_fat32(i)) {
			debugf("Disk %d is not FAT32.\n", i);
			continue;
		}

		char* name = new char[64];
		memset(name, 0, 64);
		sprintf(name, "fat32_%d", i);

		fs::fat32_mount* fat32_mount = new fs::fat32_mount(i, name);
		fs::vfs::global_vfs->register_mount(name, fat32_mount);

		debugf("Disk %d is FAT32! Registered as %s.\n", i, name);
	}
}

define_module("fat32", null_ptr_func, null_ptr_func, fs_init);