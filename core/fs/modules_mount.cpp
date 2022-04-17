#include <fs/modules_modules.h>

#include <utils/log.h>

using namespace fs;
using namespace fs::vfs;

modules_mount::modules_mount(boot::boot_info_t* bootinfo) {
	this->info = bootinfo;

	for (int i = 0; i < info->module_count; i++) {
		debugf("Module %d: %s\n", i, info->modules[i].string);
	}
}

file_t* modules_mount::open(char* path) {
	file_t* fp = new file_t;
	fp->mount = this;
	
	for (int i = 0; i < info->module_count; i++) {
		if(strcmp(info->modules[i].string, (char*) path) == 0) {
			fp->data = (void*) info->modules[i].begin;
			fp->size = info->modules[i].end - info->modules[i].begin;
			goto found;
		}
	}

	delete fp;
	return nullptr;

found:
	return fp;
}

void modules_mount::close(file_t* file) {
	delete file;
}

void modules_mount::read(file_t* file, void* buffer, size_t size, size_t offset) {
	memcpy(buffer, (char*) file->data + offset, size);
}


dir_t modules_mount::dir_at(int idx, char* path) {
	if (idx >= info->module_count) {
		debugf("modules_mount::dir_at: idx out of bounds\n");

		return {
			.is_none = true
		};
	}

	dir_t dir;
	memset(&dir, 0, sizeof(dir));
	
	strcpy(dir.name, info->modules[idx].string);
	dir.idx = idx;
	dir.is_none = false;
	dir.type = ENTRY_FILE;

	return dir;
}
