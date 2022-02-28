#pragma once

#include <stddef.h>
#include <utils/list.h>

namespace fs {
	namespace vfs {
		struct file_t;
		class vfs_mount;
		class dir_t;

		class vfs_manager {
			public:
				vfs_manager();

				file_t* open(char* path);
				void close(file_t* file);
				void read(file_t* file, void* buffer, size_t size, size_t offset);
				void write(file_t* file, void* buffer, size_t size, size_t offset);
				void delete_(file_t* file);
				void mkdir(char* path);
				void touch(char* path);
				dir_t dir_at(int idx, char* path);

				void register_mount(char* device, vfs_mount* vfs_mount_point);
				void unregister_mount(char* device);

				bool fs_at(int idx, char* out);
			
			private:
				struct mount_store_t {
					char name[256];
					vfs_mount* mount;
				};

				list<mount_store_t> mounts;
		};

		class vfs_mount {
			public:
				virtual file_t* open(char* path);
				virtual void close(file_t* file);
				virtual void read(file_t* file, void* buffer, size_t size, size_t offset);
				virtual void write(file_t* file, void* buffer, size_t size, size_t offset);
				virtual void delete_(file_t* file);
				virtual void mkdir(char* path);
				virtual void touch(char* path);
				virtual dir_t dir_at(int idx, char* path);
		};

		enum dir_entry_type_e {
			ENTRY_FILE,
			ENTRY_DIR
		};

		struct dir_t {
			char name[256];
			int idx;
			bool is_none;
			dir_entry_type_e type;
		};

		struct file_t {
			vfs_mount* mount;
			char buffer[512];
			void* data;
			size_t size;
		};

		void setup();

		extern vfs_manager* global_vfs;
	}
}