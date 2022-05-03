#pragma once

#include <fs/dev_fs.h>
#include <smbios32.h>

namespace smbios32 {
	class sm32_dev_fs_file : public fs::dev_fs_file {
		public:
			sm32_dev_fs_file();

			virtual void read(fs::file_t* file, void* buffer, size_t size, size_t offset);

			virtual void prepare_file(fs::file_t* file);
			virtual char* get_name();

		private:
			smbios_entry_t* entry;
			smbios_structure_header_t* header;

			uint8_t* data;
			uint32_t data_size;
	};
}