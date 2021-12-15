#pragma once

#include <net/etherframe.h>
#include <net/arp.h>
#include <net/ipv4.h>
#include <net/icmp.h>
#include <net/udp.h>
#include <net/ntp.h>

#define HOSTNAME (char*) "FoxOS"

namespace net {
	struct network_stack_t {
		net::ether_frame_provider* ether;
		net::address_resolution_protocol* arp;
		net::ipv4_provider* ipv4;
		net::icmp_provider* icmp;
		net::udp_provider* udp;
		net::network_time_protocol* ntp;
		void* tcp;
		void* dns;
	};
}