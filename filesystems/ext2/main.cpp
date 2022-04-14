#include <elf/kernel_module.h>

#include <driver/driver.h>
#include <driver/disk.h>

#include <fs/vfs.h>
#include <ext2_vfs.h>
#include <ext2_utils.h>

#include <memory/page_frame_allocator.h>

#include <input/input.h>

#include <utils/log.h>
#include <utils/string.h>

void init() {}

void device_init() {}

void fs_init() {
	debugf("Searching for EXT2 filesystems...\n");
	
	int ext2_count = 1;
	uint8_t* buffer = (uint8_t*) memory::global_allocator.request_page();

	for (int disk_num = 0; disk_num < driver::global_disk_manager->num_disks; disk_num++) {
		memset(buffer, 0, 4096);

		if (!fs::is_ext2(disk_num, buffer)) {
			debugf("Disk %d is not EXT2.\n", disk_num);
			continue;
		}

		char* name = new char[64];
		memset(name, 0, 64);

		fs::ext2_mount* ext2_mount = new fs::ext2_mount(disk_num, name);
		if (!driver::disk_device::get_disk_label(name, ext2_mount)) {
			sprintf(name, "ext2_%d", ext2_count);
			ext2_count++;
		}

		fs::vfs::global_vfs->register_mount(name, ext2_mount);

		debugf("Disk %d is EXT2! Registered as %s.\n", disk_num, name);
	}

	memory::global_allocator.free_page(buffer);
}

define_module("ext2", null_ptr_func, null_ptr_func, fs_init);