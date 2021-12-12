#pragma once

#include <stdint.h>

namespace driver {
	#define NIC_NUM 265

	union ip_u {
		uint8_t ip_p[4];
		uint32_t ip;
	};

	union mac_u {
		uint8_t mac_p[6];
		uint64_t mac: 48;
	};

	class nic_device;
	class nic_data_manager {
		public:
			nic_data_manager(int nic_id);
			~nic_data_manager();

			void send(uint8_t* data, int32_t len);
			virtual bool recv(uint8_t* data, int32_t len);

			nic_device* nic;
			int nic_id;
	};

	class nic_device {
		public:
			nic_device();
			~nic_device();

			virtual void send(uint8_t* data, int32_t len);

			virtual void register_nic_data_manager(nic_data_manager* nic_data_manager);
			virtual uint64_t get_mac();

			virtual uint32_t get_ip();
			virtual void set_ip(uint32_t ip);

			nic_data_manager* _nic_data_manager;
	};

	class nic_driver_manager {
		public:
			private:
				nic_device* nics[NIC_NUM];

			public:
				int num_nics;
				nic_driver_manager();

				nic_device* get_nic(int nic_id);
				void add_nic(nic_device* nic);
	};

	extern nic_driver_manager* global_nic_manager;

	void load_network_stack();
}