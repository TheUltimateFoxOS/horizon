#pragma once

#include <fs/vfs.h>

#include <fatfs/ff.h>

#include <driver/disk.h>

namespace fs {
	using namespace vfs;

	class fat32_mount : public vfs_mount {
		public:
			virtual file_t* open(char* path);
			virtual void close(file_t* file);
			virtual void read(file_t* file, void* buffer, size_t size, size_t offset);
			virtual void write(file_t* file, void* buffer, size_t size, size_t offset);

			virtual void delete_(file_t* file);
			virtual void mkdir(char* path);
			virtual dir_t dir_at(int idx, char* path);

			virtual void touch(char* path);

			fat32_mount(int disk_id, char* name);
			~fat32_mount();

		private:
			FATFS* fatfs;
			int drive_number = 0;
	};
}