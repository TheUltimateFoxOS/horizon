#include <net/arp.h>

#include <timer/pit.h>
#include <timer/hpet.h>

#include <utils/log.h>

using namespace net;

address_resolution_protocol::address_resolution_protocol(ether_frame_provider* ether): ether_frame_handler(ether, 0x806) {
	this->num_cache_entry = 0;
}

address_resolution_protocol::~address_resolution_protocol() {

}

bool address_resolution_protocol::on_ether_frame_received(uint8_t* payload, uint32_t size) {
	if (size < sizeof(arp_message_t)) {
		return false;
	}

	arp_message_t* arp = (arp_message_t*) payload;
	if (arp->hardware_type == 0x0100) {
		if(arp->protocol == 0x0008 && arp->hardware_address_size == 6 && arp->protocol_address_size == 4 && arp->dest_ip == this->backend->nic->get_ip()) {
			switch (arp->command) {
				case 0x0100: // request
					{
						arp->command = 0x0200;
						arp->dest_ip = arp->src_ip;
						arp->dest_mac = arp->src_mac;
						arp->src_ip = this->backend->nic->get_ip();
						arp->src_mac = this->backend->nic->get_mac();
						this->send(arp->dest_mac, (uint8_t*) arp, sizeof(arp_message_t));
						return false;
					}
					break;
				case 0x0200: // response
					{
						if (this->num_cache_entry < 128) {
							this->ip_cache[this->num_cache_entry] = arp->src_ip;
							this->mac_cache[this->num_cache_entry] = arp->src_mac;
							this->num_cache_entry++;
						}
					}
					break;
			}
		}
	}

	return false;
}

void address_resolution_protocol::broadcast_mac(uint32_t ip_be) {
	arp_message_t arp = {
		.hardware_type = 0x0100,
		.protocol = 0x0008,
		.hardware_address_size = 6,
		.protocol_address_size = 4,
		.command = 0x0200,
		.src_mac = this->backend->nic->get_mac(),
		.src_ip = this->backend->nic->get_ip(),
		.dest_mac = resolve(ip_be),
		.dest_ip = ip_be
	};

	this->send(arp.dest_mac, (uint8_t*) &arp, sizeof(arp_message_t));
}

void address_resolution_protocol::request_mac_address(uint32_t ip_be) {
	arp_message_t arp = {
		.hardware_type = 0x0100,
		.protocol = 0x0008,
		.hardware_address_size = 6,
		.protocol_address_size = 4,
		.command = 0x0100,
		.src_mac = this->backend->nic->get_mac(),
		.src_ip = this->backend->nic->get_ip(),
		.dest_mac = 0xFFFFFFFFFFFF,
		.dest_ip = ip_be
	};

	this->send(arp.dest_mac, (uint8_t*) &arp, sizeof(arp_message_t));
}

uint64_t address_resolution_protocol::get_mac_from_cache(uint32_t ip_be) {
	for (int i = 0; i < this->num_cache_entry; i++) {
		if (this->ip_cache[i] == ip_be) {
			return this->mac_cache[i];
		}
	} 
	return 0xFFFFFFFFFFFF;
}

uint64_t address_resolution_protocol::resolve(uint32_t ip_be) {
	if (ip_be == 0xffffffff) {
		return 0xFFFFFFFFFFFF;
	}
	
	uint64_t result = this->get_mac_from_cache(ip_be);

	if (result == 0xFFFFFFFFFFFF) {
		this->request_mac_address(ip_be);
	}

	int timeout = 100000;

	while (result == 0xFFFFFFFFFFFF) {
		result = this->get_mac_from_cache(ip_be);
		if (--timeout == 0) {
			debugf("timeout for arp request!\n");
			return 0;
		}

		if (timer::global_hpet_timer->is_available()) {
			timer::global_hpet_timer->sleep(10);
		} else {
			timer::global_pit_timer->sleep(10);
		}
	}

	return result;
}