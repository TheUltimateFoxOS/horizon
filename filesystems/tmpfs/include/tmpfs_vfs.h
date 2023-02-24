#pragma once

#include <fs/vfs.h>

#define TMPFS_BLOCK_SIZE 4096
#define BLOCKS_PER_DATA_BLOCK ((TMPFS_BLOCK_SIZE / 8) - 1)

#define TMPFS_DIR 1
#define TMPFS_FIL 2

namespace fs {
	typedef struct tmpfs_block {
		uint8_t data[TMPFS_BLOCK_SIZE];
	} tmpfs_block_t;

	typedef struct tmpfs_dirent_data {
		tmpfs_block* data[BLOCKS_PER_DATA_BLOCK];
		tmpfs_dirent_data* next;
	} tmpfs_dirent_data_t;

	typedef struct tmpfs_dirent {
		char name[256];
		int type;

		tmpfs_dirent* parent;

		size_t size;
		tmpfs_dirent_data* data;
	} tmpfs_dirent_t;

	using namespace vfs;

	class tmpfs_mount : public vfs_mount {
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

			char* name();

			tmpfs_mount(char* name);
			~tmpfs_mount();
		
		private:
			tmpfs_dirent* root_node;

			bool read_dirent(tmpfs_dirent* dirent, void* buffer, size_t size, size_t offset);
			void write_dirent(tmpfs_dirent* dirent, void* buffer, size_t size, size_t offset);

			bool readdir(tmpfs_dirent* dir, int idx, tmpfs_dirent* buffer);
			void writedir(tmpfs_dirent* dir, tmpfs_dirent_t* dirent);

			void create_diretnt(tmpfs_dirent* parent, char* name, int type);
			bool get_dirent(char* path, tmpfs_dirent_t* buffer);
			bool get_parent_dirent(char* path, tmpfs_dirent_t* buffer);
	};
}