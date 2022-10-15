#include <driver/driver.h>
#include <stdint.h>

extern "C" {
	struct rust_driver_data_t {
		void (*activate)(rust_driver_data_t* d);
		bool (*is_present)(rust_driver_data_t* d);
		const char* (*get_name)(rust_driver_data_t* d);
		uint64_t driver;
	};

	class rust_driver_t : public driver::device_driver {
		public:
			rust_driver_data_t* data;

			void activate() {
				this->data->activate(this->data);
			}

			bool is_presend() {
				return this->data->is_present(this->data);
			}

			char* get_name() {
				return (char*) this->data->get_name(this->data);
			}
	};

	uint64_t __rust_driver_alloc() {
		return (uint64_t) new rust_driver_t();
	}

	void __rust_driver_register(rust_driver_data_t* data) {
		((rust_driver_t*) data->driver)->data = data;
		driver::global_driver_manager->add_driver(((rust_driver_t*) data->driver));
	}
}