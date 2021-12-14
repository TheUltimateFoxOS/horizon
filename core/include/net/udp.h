#pragma once

#include <net/ipv4.h>

#include <utils/list.h>

namespace net {
	struct udp_header_t {
		uint16_t src_port;
		uint16_t dst_port;
		uint16_t length;
		uint16_t checksum;
	} __attribute__((packed));

	class udp_socket;
	class udp_provider;

	class udp_handler {
		public:
			udp_handler();
			~udp_handler();

			virtual void on_udp_message(udp_socket* socket, uint8_t* data, size_t size);
	};

	class udp_socket {
		public:
			udp_socket(udp_provider *provider);
			~udp_socket();

			virtual void handle_udp_message(uint8_t* data, size_t size);
			virtual void send(uint8_t* data, size_t size);
			virtual void disconnect();

			bool listening;
			uint16_t remotePort;
			uint32_t remoteIp;
			uint16_t localPort;
			uint32_t localIp;

			udp_provider* provider;
			udp_handler* handler;
	};

	class udp_provider: public ipv4_handler {
		public:

			struct udp_bind_t {
				uint16_t port;
				udp_socket* handler;
			};

			list<udp_bind_t> binds;

			int free_port = 1024;
			

			udp_provider(ipv4_provider *ipv4Provider);
			~udp_provider();

			virtual bool on_internet_protocol_received(uint32_t srcIP_BE, uint32_t dstIP_BE, uint8_t* payload, uint32_t size);

			virtual udp_socket* connect(uint32_t ip, uint16_t port);
			virtual udp_socket* listen(uint16_t port);

			virtual void disconnect(udp_socket* socket);

			virtual void send(udp_socket* socket, uint8_t* data, size_t size);

			virtual void bind(udp_socket* socket, udp_handler* handler);
	};
}