#pragma once

#include <stdint.h>

#include <driver/nic.h>

#include <utils/list.h>

namespace net {
	struct ether_frame_header_t {
		uint64_t dest_mac_be : 48;
		uint64_t src_mac_be : 48;
		uint16_t ether_type_be;
	} __attribute__((packed));

	typedef uint32_t ether_frame_footer_t;

	class ether_frame_provider;

	class ether_frame_handler {
		public:
			ether_frame_provider* backend;
			uint16_t ether_type_be;
		
			ether_frame_handler(ether_frame_provider* backend, uint16_t ether_type);
			~ether_frame_handler();

			virtual bool on_ether_frame_received(uint8_t* payload, uint32_t size);
			void send(uint64_t dest_mac_be, uint8_t* payload, uint32_t size);
	};

	class ether_frame_provider : public driver::nic_data_manager {
		public:
			list<ether_frame_handler*> handlers;

			ether_frame_provider(int nic_id);
			~ether_frame_provider();

			virtual bool recv(uint8_t* data, int32_t size) override;
			void send_f(uint64_t dest_mac_be, uint16_t ether_type_be, uint8_t* payload, uint32_t size);
	};
}