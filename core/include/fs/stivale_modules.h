#pragma once

#include <fs/vfs.h>
#include <stivale2.h>

namespace fs {
	using namespace vfs;

	class stivale_mount : public vfs_mount {
		public:
			virtual file_t* open(char* path);
			virtual void close(file_t* file);
			virtual void read(file_t* file, void* buffer, size_t size, size_t offset);
			
			virtual dir_t dir_at(int idx, char* path);

			stivale_mount(stivale2_struct* bootinfo);

		private:
			stivale2_struct_tag_modules* modules;
	};
}