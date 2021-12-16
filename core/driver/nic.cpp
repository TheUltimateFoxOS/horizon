#include <driver/nic.h>

#include <net/network_stack.h>
#include <net/etherframe.h>
#include <net/arp.h>
#include <net/ipv4.h>
#include <net/udp.h>
#include <net/dhcp.h>
#include <net/ntp.h>

#include <renderer/font_renderer.h>

#include <utils/log.h>

using namespace driver;

namespace driver {
	nic_driver_manager* global_nic_manager;
}

void driver::load_network_stack() {
	for (int i = 0; i < driver::global_nic_manager->num_nics; i++) {
		printf("Loading NIC %d...", i);

		driver::nic_device* nic = driver::global_nic_manager->get_nic(i);
		if (nic == nullptr) {
			renderer::global_font_renderer->cursor_position.x = renderer::global_font_renderer->target_frame_buffer->width - 8 * 13;
			uint64_t old_color = renderer::global_font_renderer->color;
			printf("[");
			renderer::global_font_renderer->color = 0xff00ff00;
			printf("no device");
			renderer::global_font_renderer->color = old_color;
			printf("]\n");

			continue;
		}

		net::ether_frame_provider* ether = new net::ether_frame_provider(i);
		net::address_resolution_protocol* arp = new net::address_resolution_protocol(ether);
		net::ipv4_provider* ipv4 = new net::ipv4_provider(ether, arp, 0xffffffff, 0xffffffff);
		net::icmp_provider* icmp = new net::icmp_provider(ipv4);
		net::udp_provider* udp = new net::udp_provider(ipv4);

		net::udp_socket* dhcp_socket = udp->connect(0xffffffff, 67);
		net::dhcp_protocol* dhcp = new net::dhcp_protocol(dhcp_socket);
		udp->bind(dhcp_socket, dhcp);

		dhcp->request();
		nic->set_ip(dhcp->ip);
		ipv4->gateway_ip_be = dhcp->gateway;
		ipv4->subnet_mask_be = dhcp->subnet;

		delete dhcp;
		delete dhcp_socket;

		arp->broadcast_mac(ipv4->gateway_ip_be);

		driver::ip_u ip;
		ip.ip = nic->get_ip();

		driver::ip_u gateway;
		gateway.ip = ipv4->gateway_ip_be;

		driver::ip_u subnet;
		subnet.ip = ipv4->subnet_mask_be;

		driver::ip_u dns_ip;
		dns_ip.ip = 0;

		driver::ip_u ntp_ip;
		// 129.6.15.28	(time-a-g.nist.gov)
		ntp_ip.ip_p[0] = 129;
		ntp_ip.ip_p[1] = 6;
		ntp_ip.ip_p[2] = 15;
		ntp_ip.ip_p[3] = 28;

		net::udp_socket* ntp_socket = udp->connect(ntp_ip.ip, 123);
		net::network_time_protocol* ntp = new net::network_time_protocol(ntp_socket);
		udp->bind(ntp_socket, ntp);

		//driver::clock_device::clock_result_t ntp_res = ntp->time();
		//debugf("ntp: %d %d %d %d:%d:%d\n", ntp_res.year, ntp_res.month, ntp_res.day, ntp_res.hours, ntp_res.minutes, ntp_res.seconds);
		//driver::default_clock_device = ntp;

		net::network_stack_t* network_stack = new net::network_stack_t;
		*network_stack = {
			.ether = ether,
			.arp = arp,
			.ipv4 = ipv4,
			.icmp = icmp,
			.udp = udp,
			.ntp = ntp,
			.tcp = nullptr,
			.dns = nullptr
		};

		nic->load_network_stack(network_stack);

		renderer::global_font_renderer->cursor_position.x = renderer::global_font_renderer->target_frame_buffer->width - 8 * 6;
		uint64_t old_color = renderer::global_font_renderer->color;
		printf("[");
		renderer::global_font_renderer->color = 0xff00ff00;
		printf("ok");
		renderer::global_font_renderer->color = old_color;
		printf("]\n");

		printf("ip: %d.%d.%d.%d, gateway: %d.%d.%d.%d, dns: %d.%d.%d.%d\n", ip.ip_p[0], ip.ip_p[1], ip.ip_p[2], ip.ip_p[3], gateway.ip_p[0], gateway.ip_p[1], gateway.ip_p[2], gateway.ip_p[3], dns_ip.ip_p[0], dns_ip.ip_p[1], dns_ip.ip_p[2], dns_ip.ip_p[3]);
	}
}

nic_device::nic_device() {};
nic_device::~nic_device() {};

void nic_device::send(uint8_t* data, int32_t len) {
	debugf("driver::nic_device::send not implemented\n");
}

void nic_device::register_nic_data_manager(nic_data_manager* _nic_data_manager) {
	this->_nic_data_manager = _nic_data_manager;
	_nic_data_manager->nic = this;
}

uint64_t nic_device::get_mac() {
	debugf("driver::nic_device::get_mac not implemented\n");
	return 0;
}

uint32_t nic_device::get_ip() {
	debugf("driver::nic_device::get_ip not implemented\n");
	return 0;
}

void nic_device::set_ip(uint32_t ip) {
	debugf("driver::nic_device::set_ip not implemented\n");
}

void nic_device::load_network_stack(net::network_stack_t* network_stack) {
	this->network_stack = network_stack;

	debugf("[NIC] Network stack loaded.\n");
	debugf("      etherframe: %x\n", network_stack->ether);
	debugf("      arp: %x\n", network_stack->arp);
	debugf("      ipv4: %x\n", network_stack->ipv4);
	debugf("      icmp: %x\n", network_stack->icmp);
	debugf("      udp: %x\n", network_stack->udp);
	debugf("      ntp: %x\n", network_stack->ntp);
	debugf("      tcp: %x\n", network_stack->tcp);
	debugf("      dns: %x\n", network_stack->dns);
}

nic_data_manager::nic_data_manager(int id) {
	this->nic_id = id;
	global_nic_manager->get_nic(id)->register_nic_data_manager(this);
	debugf("nic_data_manager created for nic id %d\n", id);
}

nic_data_manager::~nic_data_manager() {}

void nic_data_manager::send(uint8_t* data, int32_t len) {
	this->nic->send(data, len);
}

bool nic_data_manager::recv(uint8_t* data, int32_t len) {
	debugf("driver::nic_data_manager::recv not implemented\n");
	return false;
}

nic_driver_manager::nic_driver_manager() {
	this->num_nics = 0;
}

void nic_driver_manager::add_nic(nic_device* nic) {
	this->nics[this->num_nics] = nic;
	debugf("Adding new Nic at idx %d!\n", this->num_nics);
	this->num_nics++;
}

nic_device* nic_driver_manager::get_nic(int nic_id) {
	return this->nics[nic_id];
}
