#pragma once

#include <stdint.h>

#define DRIVER_NUM 256

namespace driver {
	class device_driver {
		public:
			device_driver();
			~device_driver();

			virtual void activate();
			virtual bool is_presend();
			virtual char* get_name();
	};

	class driver_manager {
		private:
			device_driver* drivers[DRIVER_NUM];
			int num_drivers;
			
		public:
			driver_manager();
			void add_driver(device_driver* driver);
			void set_status(char* status, uint64_t color);
			void activate_driver(bool force, device_driver* driver);
			void activate_all(bool force);

			device_driver* find_driver_by_name(char* name);
	};

	extern driver_manager* global_driver_manager;
}