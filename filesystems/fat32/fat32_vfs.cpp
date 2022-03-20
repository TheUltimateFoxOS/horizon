#include <fat32_vfs.h>

#include <fatfs/ff.h>

#include <memory/page_frame_allocator.h>
#include <memory/heap.h>

#include <utils/log.h>
#include <utils/assert.h>


using namespace fs;
using namespace fs::vfs;

fat32_mount::fat32_mount(int disk_id, char* name) {
	FATFS* fs = (FATFS*) memory::global_allocator.request_page(); // dont ask why but fatfs doesent like heap addresses
	
	char new_path[3] = {0};
	new_path[0] = '0' + disk_id;
	new_path[1] = ':';
	new_path[2] = 0;

	f_mount(fs, new_path, 1);

	this->fatfs = fs;
	this->drive_number = disk_id;
}

fat32_mount::~fat32_mount() {
	memory::global_allocator.free_page((void*) this->fatfs);
}

file_t* fat32_mount::open(char* path) {
	debugf("Opening file %s\n", path);

	FIL fil;
	file_t* file = new file_t;
	memset(file, 0, sizeof(file_t));

	char new_path[256] = {0};
	memset(new_path, 0, 256);
	new_path[0] = '0' + drive_number;
	new_path[1] = ':';
	strcpy(new_path + 2, path);
	new_path[strlen(path) + 2] = 0;

	FRESULT fr = f_open(&fil, new_path, FA_READ | FA_WRITE);
	if (fr != FR_OK) {
		delete file;
		return nullptr;
	}

	file->mount = this;
	strcpy(file->buffer, path);
	file->size = f_size(&fil);
	file->data = (void*) memory::global_allocator.request_pages(sizeof(FIL) / 0x1000 + 1);
	memcpy(file->data, &fil, sizeof(FIL));

	return file;
}

void fat32_mount::close(file_t* file) {
	debugf("Closing file\n");

	f_close((FIL*) file->data);
	memory::global_allocator.free_pages(file->data, sizeof(FIL) / 0x1000 + 1);
	delete file;
}

void fat32_mount::read(file_t* file, void* buffer, size_t size, size_t offset) {
	debugf("Reading %d bytes from %d\n", size, offset);

	f_lseek((FIL*) file->data, offset);

	unsigned int has_read;
	f_read((FIL*) file->data, buffer, size, &has_read);

	assert(has_read == size);
}

void fat32_mount::write(file_t* file, void* buffer, size_t size, size_t offset) {
	debugf("Writing %d bytes to %d\n", size, offset);

	f_lseek((FIL*) file->data, offset);

	unsigned int has_written;
	f_write((FIL*) file->data, buffer, size, &has_written);

	assert(has_written == size);
}

void fat32_mount::delete_(file_t* file) {
	f_unlink((char*) file->buffer);
	delete file;
}

void fat32_mount::mkdir(char* path) {
	char new_path[256] = {0};
	memset(new_path, 0, 256);
	new_path[0] = '0' + drive_number;
	new_path[1] = ':';
	strcpy(new_path + 2, path);
	new_path[strlen(path) + 2] = 0;

	f_mkdir(new_path);
}

dir_t fat32_mount::dir_at(int idx, char* path) {
	DIR dir_;
	FILINFO file_info;

	char new_path[256] = {0};
	memset(new_path, 0, 256);
	new_path[0] = '0' + drive_number;
	new_path[1] = ':';
	strcpy(new_path + 2, path);
	new_path[strlen(path) + 2] = 0;

	if (f_opendir(&dir_, new_path) != FR_OK) {
		return {
			.is_none = true
		};
	}
	
	FRESULT fr = f_readdir(&dir_, &file_info);
	assert(fr == FR_OK);

	int orig_idx = idx;

	while (idx--) {
		FRESULT res = f_readdir(&dir_, &file_info);
		if (res != FR_OK || file_info.fname[0] == 0) {
			return {
				.is_none = true
			};
		}
	}

	dir_t dir;
	memset(&dir, 0, sizeof(dir_t));

	dir.idx = orig_idx;
	dir.is_none = false;

	if (file_info.fattrib & AM_DIR) {
		dir.type = ENTRY_DIR;
	} else {
		dir.type = ENTRY_FILE;
	}

	strcpy(dir.name, file_info.fname);

	f_closedir(&dir_);

	return dir;
}

void fat32_mount::touch(char* path) {
	char new_path[256] = {0};
	memset(new_path, 0, 256);
	new_path[0] = '0' + drive_number;
	new_path[1] = ':';
	strcpy(new_path + 2, path);
	new_path[strlen(path) + 2] = 0;

	FIL file;
	f_open(&file, new_path, FA_WRITE | FA_CREATE_ALWAYS);
	f_close(&file);
}
