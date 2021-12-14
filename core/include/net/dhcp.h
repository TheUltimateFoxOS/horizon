#pragma once

#include <net/udp.h>

namespace net {
	struct dhcp_packet_t {
		uint8_t op;
		uint8_t hardware_type;
		uint8_t hardware_addr_len;
		uint8_t hops;
		uint32_t xid;
		uint16_t seconds;
		uint16_t flags;
		uint32_t client_ip;
		uint32_t your_ip;
		uint32_t server_ip;
		uint32_t gateway_ip;
		uint8_t client_hardware_addr[16];
		uint8_t server_name[64];
		uint8_t file[128];
		uint8_t options[64];
	} __attribute__ ((packed));

	#define DHCP_REQUEST 1
	#define DHCP_REPLY 2

	#define DHCP_TRANSACTION_IDENTIFIER 0x55555555

	class dhcp_protocol : public net::udp_handler{
		public:
			dhcp_protocol(udp_socket* socket);
			~dhcp_protocol();

			void request();
			void request(uint32_t ip);

			uint32_t ip;
			uint32_t gateway;
			uint32_t subnet;
			uint32_t dns;

			bool complete = false;

			virtual void on_udp_message(udp_socket *socket, uint8_t* data, size_t size);
		
		private:
			udp_socket* socket;

			void make_dhcp_packet(dhcp_packet_t* packet, uint8_t msg_type, uint32_t request_ip);
			void* get_dhcp_options(dhcp_packet_t* packet, uint8_t type);
	};
}