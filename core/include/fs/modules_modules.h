#pragma once

#include <fs/vfs.h>
#include <boot/boot.h>

namespace fs {
	using namespace vfs;

	class modules_mount : public vfs_mount {
		public:
			virtual file_t* open(char* path);
			virtual void close(file_t* file);
			virtual void read(file_t* file, void* buffer, size_t size, size_t offset);
			
			virtual dir_t dir_at(int idx, char* path);

			modules_mount(boot::boot_info_t* bootinfo);

		private:
			boot::boot_info_t* info;
	};
}