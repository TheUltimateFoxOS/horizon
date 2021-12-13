#pragma once

#include <net/etherframe.h>
#include <net/arp.h>

namespace net {
	struct network_stack_t {
		net::ether_frame_provider* ether;
		net::address_resolution_protocol* arp;
		void* ipv4;
		void* icmp;
		void* udp;
		void* tcp;
		void* dns;
	};
}