#include <net/ntp.h>

#include <utils/abort.h>
#include <utils/log.h>
#include <utils/string.h>
#include <timer/timer.h>
#include <utils/unix_time.h>

using namespace net;

network_time_protocol::network_time_protocol(udp_socket* socket) {
	this->socket = socket;
}

void network_time_protocol::on_udp_message(udp_socket *socket, uint8_t* data, size_t size) {
	debugf("ntp: got packet of size %d\n", size);
	last_packet = *(ntp_packet_t*) data;
	received_packet = true;
}

driver::clock_device::clock_result_t network_time_protocol::time() {
	debugf("ntp: sending request\n");

	ntp_packet_t packet;
	memset(&packet, 0, sizeof(ntp_packet_t));
	memset(&last_packet, 0, sizeof(ntp_packet_t));
	received_packet = false;

	*((char*) &packet) = 0x1b;

	__asm__ __volatile__("sti"); // make sure interrupts are enabled
	this->socket->send((uint8_t*) &packet, sizeof(ntp_packet_t));

	debugf("ntp: waiting for response\n");

	int timeout = 1000;
	while(timeout--) {
		timeout++;
		timer::global_timer->sleep(10);
		if(received_packet) {
			debugf("ntp: got response\n");
			break;
		}
	}

	if(!received_packet) {
		abortf("ntp: no response\n");
	}

	int unix_time = (__builtin_bswap32(last_packet.txTm_s) - 2208988800);

	debugf("ntp: unix time: %d\n", unix_time);

	driver::clock_device::clock_result_t result;
	memset(&result, 0, sizeof(driver::clock_device::clock_result_t));
	from_unix_time(unix_time, &result.year, &result.month, &result.day, &result.hours, &result.minutes, &result.seconds);

	return result;
}