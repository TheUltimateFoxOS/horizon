#include <fs/stivale_modules.h>

#include <utils/log.h>

using namespace fs;
using namespace fs::vfs;

stivale_mount::stivale_mount(stivale2_struct* bootinfo) {
	this->modules = stivale2_tag_find<stivale2_struct_tag_modules>(bootinfo, STIVALE2_STRUCT_TAG_MODULES_ID);
}

file_t* stivale_mount::open(char* path) {
	file_t* fp = new file_t;
	fp->mount = this;
	
	for (int i = 0; i < modules->module_count; i++) {
		if(strcmp(modules->modules[i].string, (char*) path) == 0) {
			fp->data = (void*) modules->modules[i].begin;
			fp->size = modules->modules[i].end - modules->modules[i].begin;
			goto found;
		}
	}

	delete fp;
	return nullptr;

found:
	return fp;
}

void stivale_mount::close(file_t* file) {
	delete file;
}

void stivale_mount::read(file_t* file, void* buffer, size_t size, size_t offset) {
	memcpy(buffer, (char*) file->data + offset, size);
}


dir_t stivale_mount::dir_at(int idx, char* path) {
	if (idx >= modules->module_count) {
		debugf("stivale_mount::dir_at: idx out of bounds\n");

		return {
			.is_none = true
		};
	}

	stivale2_module* module = &modules->modules[idx];

	dir_t dir;
	memset(&dir, 0, sizeof(dir));
	
	strcpy(dir.name, module->string);
	dir.idx = idx;
	dir.is_none = false;
	dir.type = ENTRY_FILE;

	return dir;
}
