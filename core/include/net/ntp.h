#pragma once

#include <net/udp.h>
#include <driver/clock.h>

namespace net {
	struct ntp_packet_t {
		uint8_t li_vn_mode;
		uint8_t stratum;
		uint8_t poll;
		uint8_t precision;
		uint32_t rootDelay;
		uint32_t rootDispersion;
		uint32_t refId;
		uint32_t refTm_s;
		uint32_t refTm_f;
		uint32_t origTm_s;
		uint32_t origTm_f;
		uint32_t rxTm_s;
		uint32_t rxTm_f;
		uint32_t txTm_s;
		uint32_t txTm_f;
	} __attribute__((packed));

	class network_time_protocol : public net::udp_handler, public driver::clock_device {
		public:
			network_time_protocol(udp_socket* socket);

			virtual void on_udp_message(udp_socket *socket, uint8_t* data, size_t size);
			virtual driver::clock_device::clock_result_t time();
		
		private:
			udp_socket* socket;
			ntp_packet_t last_packet;
			bool received_packet;
	};
}