#pragma once

#include <utils/list.h>
#include <net/ipv4.h>
#include <stdint.h>

namespace net {
	enum tcp_socket_state {
		CLOSED = 0,
		LISTEN,
		SYN_SENT,
		SYN_RECEIVED,

		ESTABLISHED,

		FIN_WAIT_1,
		FIN_WAIT_2,
		CLOSING,
		TIME_WAIT,

		CLOSE_WAIT
	};

	enum tcp_flag {
		FIN = 1,
		SYN = 2,
		RST = 4,
		PSH = 8,
		ACK = 16,
		URG = 32,
		ECE = 64,
		CWR = 128,
		NS = 256
	};

	struct tcp_header_t {
		uint16_t src_port;
		uint16_t dst_port;
		uint32_t seq_num;
		uint32_t ack;

		uint8_t reserved : 4;
		uint8_t data_offset : 4;
		uint8_t flags;

		uint16_t window_size;
		uint16_t checksum;
		uint16_t urgent_ptr; //Only if the urgent flag is set

		uint32_t options;
	} __attribute__((packed));

	struct tcp_pseudo_header_t {
		uint32_t src_ip;
		uint32_t dst_ip;
		uint16_t protocol;
		uint16_t total_len;
	} __attribute__((packed));

	struct tcp_sent_packet_t {
		uint32_t seq_num;
		bool data_empty;
		bool was_acked;
	};

	class tcp_socket;
	class tcp_provider;

	class tcp_handler {
		public:
			tcp_handler();
			~tcp_handler();

			virtual bool on_tcp_message(tcp_socket* socket, uint8_t* data, size_t size);
	};

	class tcp_socket {
		public:
			tcp_socket(tcp_provider *provider);
			~tcp_socket();

			virtual bool handle_tcp_message(uint8_t* data, size_t size);
			virtual void send(uint8_t* data, size_t size);
			virtual void disconnect();

			tcp_socket_state state;
			uint16_t remote_port;
			uint32_t remote_ip;
			uint16_t local_port;
			uint32_t local_ip;
			uint32_t seq_num;
			uint32_t ack_num;

			tcp_provider* provider;
			tcp_handler* handler;

			list<tcp_sent_packet_t> acknowledged;
	};

	class tcp_provider: public ipv4_handler {
		public:
			struct tcp_bind_t {
				uint16_t port;
				tcp_socket* handler;
			};

			list<tcp_bind_t> binds;

			int free_port = 1024;

			tcp_provider(ipv4_provider* ipv4_provider);
			~tcp_provider();

			virtual bool on_internet_protocol_received(uint32_t srcIP_BE, uint32_t dstIP_BE, uint8_t* payload, uint32_t size);

			virtual tcp_socket* connect(uint32_t ip, uint16_t port);
			virtual tcp_socket* listen(uint16_t port);

			virtual void disconnect(tcp_socket* socket);

			virtual void send(tcp_socket* socket, uint8_t* data, size_t size, uint16_t flags = 0);
			virtual void retransmit(tcp_socket* socket, uint8_t* packet, size_t size, list<net::tcp_sent_packet_t>::node* list_node);

			virtual void bind(tcp_socket* socket, tcp_handler* handler);
	};
}
