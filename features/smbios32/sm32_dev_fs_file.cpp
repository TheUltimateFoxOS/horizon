#include <sm32_dev_fs_file.h>

#include <boot/boot.h>
#include <memory/page_frame_allocator.h>

using namespace smbios32;

sm32_dev_fs_file::sm32_dev_fs_file() {
	entry = (smbios_entry_t*) boot::boot_info.smbios_entry_32;
	header = (smbios_structure_header_t*) (uint64_t) entry->table_address;

	data = (uint8_t*) memory::global_allocator.request_pages((sizeof(smbios_entry_t) + entry->table_length) / 4096 + 1);
	memcpy(data, entry, sizeof(smbios_entry_t));
	memcpy(data + sizeof(smbios_entry_t), (void*) (uint64_t) entry->table_address, entry->table_length);

	data_size = sizeof(smbios_entry_t) + entry->table_length;
}


void sm32_dev_fs_file::read(fs::file_t* file, void* buffer, size_t size, size_t offset) {
	memcpy(buffer, data + offset, size);
}

void sm32_dev_fs_file::prepare_file(fs::file_t* file) {
	file->size = data_size;
}

char* sm32_dev_fs_file::get_name() {
	return (char*) "smbios32";
}