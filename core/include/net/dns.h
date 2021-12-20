#pragma once

#include <net/udp.h>

#include <utils/list.h>

#define MAX_LOOP_COUNT 10

namespace net {
	struct dnshdr_t {
		uint16_t id, opts, qdcount, ancount, nscount, arcount;
	};

	struct dns_question_t {
		uint16_t qtype, qclass;
	};

	struct dns_resource_t {
		uint16_t type, _class;
		uint32_t ttl;
		uint16_t data_len;
	} __attribute__((packed));

	struct dns_result_t {
		uint32_t ipv4;
		char cname[256];

		uint64_t expire;
		char name[256];
	} __attribute__((packed));

	class domain_name_service_provider: public udp_handler {
		public:
			domain_name_service_provider(udp_socket* socket);
			~domain_name_service_provider();

			void resolv_domain_to_hostname(char* dst_hostname, char* src_domain);
			char* resolv_hostname_to_domain(uint8_t* reader, uint8_t* buffer, int *count);
			void resolv_skip_name(uint8_t* reader, uint8_t* buffer, int* count);

			uint32_t resolve_A(char* domain);
			
			void dns_request(char* name);

			virtual void on_udp_message(udp_socket *socket, uint8_t* data, size_t size);
			
			udp_socket* socket;
			list<dns_result_t> results;

		private:
			int loop;
			bool wait_for_response;
	};
}