#pragma once

#include <net/etherframe.h>

namespace net {
	struct network_stack_t {
		net::ether_frame_provider* ether;
		void* arp;
		void* ipv4;
		void* icmp;
		void* udp;
		void* tcp;
		void* dns;
	};
}