#pragma once

#include <stdint.h>
#include <utils/port.h>

#include <driver/driver.h>
#include <driver/disk.h>

namespace driver {

	class advanced_technology_attachment: public device_driver, public disk_device {
		private:
			bool master;
			Port16Bit dataPort;
			Port8Bit error_port;
			Port8Bit sector_count_port;
			Port8Bit lba_low_port;
			Port8Bit lba_mid_port;
			Port8Bit lba_hi_port;
			Port8Bit device_port;
			Port8Bit command_port;
			Port8Bit control_port;

			uint16_t bytes_per_sector;

			char* name;

		public:
			advanced_technology_attachment(bool master, uint16_t portBase, char* name);

			void read28(uint32_t sector, uint8_t* data, int count);
			void write28(uint32_t sectorNum, uint8_t* data, uint32_t count);
			void flush();

			virtual void activate();
			virtual bool is_presend();
			virtual char* get_name();

			virtual void read(uint64_t sector, uint32_t sector_count, void* buffer);
			virtual void write(uint64_t sector, uint32_t sector_count, void* buffer);
	};
}