#pragma once

#include <stdint.h>
#include <fs/vfs.h>
#include <fs/dev_fs.h>

#define DISK_NUM 265

namespace driver {
	class disk_device {
		public:
			disk_device();
			~disk_device();
			virtual void read(uint64_t sector, uint32_t sector_count, void* buffer);
			virtual void write(uint64_t sector, uint32_t sector_count, void* buffer);

			static bool get_disk_label(char* out, fs::vfs::vfs_mount* mount);
	};

	class raw_disk_dev_fs : public fs::dev_fs_file {
		public:
			raw_disk_dev_fs(int disk_num);

			virtual void write(fs::file_t* file, void* buffer, size_t size, size_t offset);

			virtual char* get_name();

			char name[32];
			int disk_num;

			struct raw_disk_dev_fs_command {
				uint8_t command;
				uint64_t sector;
				uint32_t sector_count;
				uint64_t buffer;
			};
	};

	class disk_driver_manager {
		public:
			disk_device* disks[DISK_NUM];
			
			int num_disks;
			disk_driver_manager();

			void read(int disk_num, uint64_t sector, uint32_t sector_count, void* buffer);
			void write(int disk_num, uint64_t sector, uint32_t sector_count, void* buffer);

			int add_disk(disk_device* disk);
	};

	extern disk_driver_manager* global_disk_manager;
}