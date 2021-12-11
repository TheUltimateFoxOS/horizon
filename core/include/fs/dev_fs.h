#pragma once

#include <fs/vfs.h>
#include <utils/list.h>

namespace fs {
	using namespace vfs;

	class dev_fs_file {
		public:
			virtual void read(file_t* file, void* buffer, size_t size, size_t offset);
			virtual void write(file_t* file, void* buffer, size_t size, size_t offset);

			virtual char* get_name();
	};

	class dev_fs : public vfs_mount {
		public:
			dev_fs();

			virtual file_t* open(char* path);
			virtual void close(file_t* file);
			virtual void read(file_t* file, void* buffer, size_t size, size_t offset);
			virtual void write(file_t* file, void* buffer, size_t size, size_t offset);
			
			list<dev_fs_file*> files;

			void register_file(dev_fs_file* file);
			void unregister_file(dev_fs_file* file);
	};

	extern dev_fs* global_devfs;
}