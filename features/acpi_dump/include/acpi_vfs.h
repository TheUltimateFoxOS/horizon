#pragma once

#include <fs/vfs.h>
#include <acpi/acpi_tables.h>

namespace fs {
	using namespace vfs;

	class acpi_vfs : public fs::vfs::vfs_mount {
		public:
			acpi_vfs(acpi::sdt_header_t** headers, int num_headers);

			file_t* open(char* path);
			void close(file_t* file);
			void read(file_t* file, void* buffer, size_t size, size_t offset);
			dir_t dir_at(int idx, char* path);
		
		private:
			acpi::sdt_header_t** headers;
			int num_headers;

	};
}