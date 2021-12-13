#pragma once

#include <net/etherframe.h>

namespace net {
	struct arp_message_t {
		uint16_t hardware_type;
		uint16_t protocol;
		uint8_t hardware_address_size;
		uint8_t protocol_address_size;
		uint16_t command;

		uint64_t src_mac: 48;
		uint32_t src_ip;
		uint64_t dest_mac: 48;
		uint32_t dest_ip;
	} __attribute__((packed));

	class address_resolution_protocol : public ether_frame_handler {
		private:
			uint32_t ip_cache[128];
			uint64_t mac_cache[128];
			int num_cache_entry;

		public:
			address_resolution_protocol(ether_frame_provider* ether);
			~address_resolution_protocol();

			virtual bool on_ether_frame_received(uint8_t* payload, uint32_t size);
			void broadcast_mac(uint32_t ip_be);
			void request_mac_address(uint32_t ip_be);
			uint64_t get_mac_from_cache(uint32_t ip_be);
			uint64_t resolve(uint32_t ip_be);
	};
}