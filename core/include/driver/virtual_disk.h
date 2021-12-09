#pragma once

#include <stdint.h>
#include <driver/disk.h>

namespace driver {

	class virtual_disk_device : public disk_device {
		private:
			disk_device* disk;

			uint64_t lba_offset;

		public:
			virtual_disk_device(disk_device* disk, uint64_t lba_offset);
			~virtual_disk_device();
			
			virtual void read(uint64_t sector, uint32_t sector_count, void* buffer);
			virtual void write(uint64_t sector, uint32_t sector_count, void* buffer);
	};
}