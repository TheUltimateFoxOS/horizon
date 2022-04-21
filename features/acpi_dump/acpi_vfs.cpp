#include <acpi_vfs.h>

#include <utils/log.h>
#include <utils/assert.h>

using namespace fs;
using namespace fs::vfs;

acpi_vfs::acpi_vfs(acpi::sdt_header_t** headers, int num_headers) {
	this->headers = headers;
	this->num_headers = num_headers;
}

file_t* acpi_vfs::open(char* path) {
	if (path[0] == '/') {
		path++;
	}
	
	acpi::sdt_header_t* header = nullptr;
	for (int i = 0; i < num_headers; i++) {
		char file_name[16] = {0};
		sprintf(file_name, "%d.%c%c%c%c", i, headers[i]->signature[0], headers[i]->signature[1], headers[i]->signature[2], headers[i]->signature[3]);

		if (strcmp(file_name, path) == 0) {
			header = headers[i];
			break;
		}
	}

	if (header == nullptr) {
		debugf("No header found for path: %s\n", path);
		return nullptr;
	}

	file_t* file = new file_t;
	file->mount = this;
	file->data = header;
	file->size = header->length;

	return file;
}
void acpi_vfs::close(file_t* file) {
	delete file;
}

void acpi_vfs::read(file_t* file, void* buffer, size_t size, size_t offset) {
	assert(size + offset <= file->size);
	memcpy(buffer, (uint8_t*) file->data + offset, size);
}

dir_t acpi_vfs::dir_at(int idx, char* path) {
	if (idx >= num_headers) {
		return {
			.is_none = true,
		};
	}

	dir_t dir;
	memset(&dir, 0, sizeof(dir));
	dir.idx = idx;
	dir.type = ENTRY_FILE;

	sprintf(dir.name, "%d.%c%c%c%c", idx, headers[idx]->signature[0], headers[idx]->signature[1], headers[idx]->signature[2], headers[idx]->signature[3]);

	return dir;
}