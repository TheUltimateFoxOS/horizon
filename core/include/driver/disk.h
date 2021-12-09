#pragma once

#include <stdint.h>

#define DISK_NUM 265

namespace driver {
	class disk_device {
		public:
			disk_device();
			~disk_device();
			virtual void read(uint64_t sector, uint32_t sector_count, void* buffer);
			virtual void write(uint64_t sector, uint32_t sector_count, void* buffer);
	};

	class disk_driver_manager {
		public:
			disk_device* disks[DISK_NUM];
			
			int num_disks;
			disk_driver_manager();

			void read(int disk_num, uint64_t sector, uint32_t sector_count, void* buffer);
			void write(int disk_num, uint64_t sector, uint32_t sector_count, void* buffer);

			void add_disk(disk_device* disk);
	};

	extern disk_driver_manager* global_disk_manager;
}