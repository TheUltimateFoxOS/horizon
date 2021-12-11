#include <fat32_vfs.h>

#include <fatfs/ff.h>

#include <memory/page_frame_allocator.h>
#include <memory/heap.h>

#include <utils/log.h>

using namespace fs;
using namespace fs::vfs;

fat32_mount::fat32_mount(int disk_id, char* name) {
	this->fatfs = (FATFS*) memory::global_allocator.request_page();
	this->fatfs->pdrv = disk_id;

	f_mount(this->fatfs, name, 1);
}

fat32_mount::~fat32_mount() {
	memory::global_allocator.free_page((void*) this->fatfs);
}

file_t* fat32_mount::open(char* path) {
	debugf("Opening file %s\n", path);

	file_t* file = new file_t;
	FIL fil;

	FRESULT fr = f_open(&fil, path, FA_READ | FA_WRITE);
	if (fr != FR_OK) {
		delete file;
		return nullptr;
	}

	file->mount = this;
	file->size = f_size(&fil);
	file->data = (void*) memory::malloc(sizeof(FIL));
	memcpy(file->data, &fil, sizeof(FIL));

	return file;
}

void fat32_mount::close(file_t* file) {
	debugf("Closing file\n");

	f_close((FIL*) file->data);
	memory::free(file->data);
	delete file;
}

void fat32_mount::read(file_t* file, void* buffer, size_t size, size_t offset) {
	debugf("Reading %d bytes from %d\n", size, offset);

	UINT has_read;
	f_read((FIL*) file->data, buffer, size, &has_read);
}

void fat32_mount::write(file_t* file, void* buffer, size_t size, size_t offset) {
	debugf("Writing %d bytes to %d\n", size, offset);

	unsigned int has_written;
	f_write((FIL*) file->data, buffer, size, &has_written);
}