#pragma once

#include <net/network_stack.h>

namespace net {
	enum socket_type {
		TCP_SOCKET,
		UDP_SOCKET
	};

	class socket_manager_socket : public tcp_handler, public udp_handler {
		public:
			int socket_type;
			int nic_id;

			int num_bytes_received;
			uint8_t* received_data;

			int socket_id;

			socket_manager_socket(int socket_type);
			~socket_manager_socket();

			void connect(uint32_t ip_be, uint16_t port);
			void disconnect();

			void send(uint8_t* data, uint32_t size);
			int receive(uint8_t* data, uint32_t size);

			udp_socket* udp_socket_ptr;
			tcp_socket* tcp_socket_ptr;

			virtual bool on_tcp_message(tcp_socket* socket, uint8_t* data, size_t size);
			virtual void on_udp_message(udp_socket* socket, uint8_t* data, size_t size);
	};

	class socket_manager {
		public:
			socket_manager();

			list<socket_manager_socket*> sockets;

			int curr_socket = 0;

			int alloc_socket();
			void free_socket(int socket_id);
			void register_socket(socket_manager_socket* sock);
			socket_manager_socket* get_socket(int socket_id);
	};

	extern socket_manager* global_socket_manager;
}