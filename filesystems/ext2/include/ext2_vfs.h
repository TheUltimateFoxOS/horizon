#pragma once

#include <fs/vfs.h>

#include <ext2_fs.h>

namespace fs {
	using namespace vfs;

	class ext2_mount : public vfs_mount {
		public:
			virtual file_t* open(char* path);
			virtual void close(file_t* file);
			virtual void read(file_t* file, void* buffer, size_t size, size_t offset);
			virtual void write(file_t* file, void* buffer, size_t size, size_t offset);

			virtual void delete_(file_t* file);
			virtual void mkdir(char* path);
			virtual dir_t dir_at(int idx, char* path);

			virtual void touch(char* path);

			ext2_mount(int disk_id, char* name);
			~ext2_mount();

		private:
			ext2_fs_t* fs = nullptr;
	};
}