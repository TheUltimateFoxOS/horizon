#include <elf/kernel_module.h>

#include <driver/driver.h>
#include <driver/disk.h>

#include <fs/vfs.h>
#include <fat32_vfs.h>
#include <fat32_utils.h>

#include <memory/page_frame_allocator.h>


#include <utils/log.h>
#include <utils/string.h>

void init() {}

void device_init() {}

void fs_init() {
	debugf("Searching for FAT32 filesystems...\n");
	
	int fat32_count = 1;
	uint8_t* buffer = (uint8_t*) memory::global_allocator.request_page();

	for (int disk_num = 0; disk_num < driver::global_disk_manager->num_disks; disk_num++) {
		memset(buffer, 0, 4096);

		if (!fs::is_fat32(disk_num, buffer)) {
			debugf("Disk %d is not FAT32.\n", disk_num);
			continue;
		}

		char* name = new char[64];
		memset(name, 0, 64);

		fs::fat32_mount* fat32_mount = new fs::fat32_mount(disk_num, name);
		if (!driver::disk_device::get_disk_label(name, fat32_mount)) {
			sprintf(name, "fat32_%d", fat32_count);
			fat32_count++;
		}

		fs::vfs::global_vfs->register_mount(name, fat32_mount);

		debugf("Disk %d is FAT32! Registered as %s.\n", disk_num, name);
	}

	memory::global_allocator.free_page(buffer);
}

define_module("fat32", null_ptr_func, null_ptr_func, fs_init);