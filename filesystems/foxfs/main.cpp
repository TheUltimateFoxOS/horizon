#include <elf/kernel_module.h>

#include <foxfs_vfs.h>

#include <memory/heap.h>

#include <utils/log.h>
#include <utils/string.h>

void init() {}

void device_init() {}

void fs_init() {
	fs::foxfs_mount* foxfs_mount = new fs::foxfs_mount("tmp");
	fs::vfs::global_vfs->register_mount("tmp", foxfs_mount);

	//debugf("Searching for FOXFS filesystems...\n");
	
	//TODO: Search for foxfs filesystems
}

define_module("foxfs", null_ptr_func, null_ptr_func, fs_init);