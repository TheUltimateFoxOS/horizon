#include <fs/stivale_modules.h>

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

void stivale_mount::write(file_t* file, void* buffer, size_t size, size_t offset) {
	memcpy((char*) file->data + offset, buffer, size);
}