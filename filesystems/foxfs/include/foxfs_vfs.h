#pragma once

#include <fs/vfs.h>

#include <foxfs_fs.h>

namespace fs {
	using namespace vfs;

	class foxfs_mount : public vfs_mount {
		public:
			virtual file_t* open(char* path);
			virtual void close(file_t* file);
			virtual void read(file_t* file, void* buffer, size_t size, size_t offset);
			virtual void write(file_t* file, void* buffer, size_t size, size_t offset);
			virtual void touch(char* path);

			virtual void mkdir(char* path);
			virtual dir_t dir_at(int idx, char* path);

			virtual void delete_(file_t* file);
			virtual void delete_dir(char* path);

			foxfs_mount(char* name);
			foxfs_mount(int disk_id, char* name);
			~foxfs_mount();

		private:
			foxfs_fs_t* fs;
	};
}