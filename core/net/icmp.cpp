#include <net/icmp.h>

#include <driver/nic.h>
#include <timer/timer.h>
#include <utils/log.h>

using namespace net;

icmp_provider::icmp_provider(ipv4_provider* ipv4) : ipv4_handler(ipv4, 0x01) {

}

icmp_provider::~icmp_provider() {

}

bool icmp_provider::on_internet_protocol_received(uint32_t srcIP_BE, uint32_t dstIP_BE, uint8_t* payload, uint32_t size) {
	if (size < sizeof(icmp_message_t)) {
		return false;
	}

	icmp_message_t* icmp = (icmp_message_t*) payload;

	switch (icmp->type) {
		case 0:
			{
				// Echo reply
				driver::ip_u ip;
				ip.ip = srcIP_BE;

				last_echo_reply_ip = ip.ip;

				debugf("ICMP: Echo reply from %d.%d.%d.%d\n", ip.ip_p[0], ip.ip_p[1], ip.ip_p[2], ip.ip_p[3]);
				return false;
			}
			break;
		case 8:
			{
				// Echo request
				icmp->type = 0;
				icmp->checksum = 0;
				icmp->checksum = this->backend->checksum((uint16_t*) icmp, sizeof(icmp_message_t));
				this->send(srcIP_BE, (uint8_t*) icmp, sizeof(icmp_message_t));
				return false;
			}
			break;
	}

	return false;
}

void icmp_provider::send_echo_request(uint32_t dstIP_BE) {
	icmp_message_t icmp = {
		.type = 8,
		.code = 0,
		.checksum = 0,
		.data = 0x3713
	};

	icmp.checksum = backend->checksum((uint16_t*) &icmp, sizeof(icmp_message_t));
	this->send(dstIP_BE, (uint8_t*) &icmp, sizeof(icmp_message_t));
}

bool icmp_provider::send_echo_reqest_and_wait(uint32_t dstIP_BE) {
	send_echo_request(dstIP_BE);

	last_echo_reply_ip = 0;

	int timeout = 1000;
	while(timeout--) {
		timer::global_timer->sleep(10);
		if (last_echo_reply_ip == dstIP_BE) {
			return true;
		}
	}

	return false;
}
