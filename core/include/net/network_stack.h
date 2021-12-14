#pragma once

#include <net/etherframe.h>
#include <net/arp.h>
#include <net/ipv4.h>

namespace net {
	struct network_stack_t {
		net::ether_frame_provider* ether;
		net::address_resolution_protocol* arp;
		net::ipv4_provider* ipv4;
		void* icmp;
		void* udp;
		void* tcp;
		void* dns;
	};
}