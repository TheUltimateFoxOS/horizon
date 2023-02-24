#include <elf/kernel_module.h>

#include <tmpfs_vfs.h>

#include <memory/heap.h>

#include <utils/log.h>
#include <utils/string.h>

void init() {}

void device_init() {}

void fs_init() {
	fs::tmpfs_mount* tmpfs_mount = new fs::tmpfs_mount((char*) "tmp");
	fs::vfs::global_vfs->register_mount(tmpfs_mount->name(), tmpfs_mount);

	//TODO: Maybe make this do physical storage
}

define_module("tmpfs", null_ptr_func, null_ptr_func, fs_init);