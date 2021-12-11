#pragma once

#include <fs/vfs.h>
#include <utils/list.h>

namespace fs {
	using namespace vfs;
	class file_descriptor {
		public:
			int fd;

			file_descriptor(int fd);
			~file_descriptor();

			void open(char* path);
			void close();
			void read(void* buffer, size_t size, size_t offset);
			void write(void* buffer, size_t size, size_t offset);

			file_t* file;
	};

	class file_descriptor_manager {
		public:
			file_descriptor_manager();

			list<file_descriptor*> fds;

			int curr_fd = 0;

			int alloc_fd();
			void free_fd(int fd);
			void register_fd(file_descriptor* fd_obj);
			file_descriptor* get_fd(int fd);
	};

	extern file_descriptor_manager* global_fd_manager;
}