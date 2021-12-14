#include <net/udp.h>

#include <memory/heap.h>

using namespace net;

udp_handler::udp_handler() {

}

udp_handler::~udp_handler() {

}

void udp_handler::on_udp_message(udp_socket *socket, uint8_t* data, size_t size) {

}

udp_socket::udp_socket(udp_provider* provider) {
	this->provider = provider;
	this->handler = nullptr;
	this->listening = false;
}

udp_socket::~udp_socket() {

}

void udp_socket::handle_udp_message(uint8_t* data, size_t size) {
	if (this->handler != nullptr) {
		this->handler->on_udp_message(this, data, size);
	}
}

void udp_socket::send(uint8_t* data, size_t size) {
	provider->send(this, data, size);
}

void udp_socket::disconnect() {
	provider->disconnect(this);
}

udp_provider::udp_provider(ipv4_provider* ipv4Provider): ipv4_handler(ipv4Provider, 0x11), binds(100) {

}

struct udp_search_t {
	uint32_t src_ip;
	uint32_t dst_ip;
	udp_header_t* udp_header;
};

bool udp_provider::on_internet_protocol_received(uint32_t srcIP_BE, uint32_t dstIP_BE, uint8_t* payload, uint32_t size) {
	if (size < sizeof(udp_header_t)) {
		return false;
	}

	udp_header_t* udp = (udp_header_t*)payload;

	udp_search_t search = {
		.src_ip = srcIP_BE,
		.dst_ip = dstIP_BE,
		.udp_header = udp
	};

	list<udp_bind_t>::node* n = binds.find<udp_search_t>([](udp_search_t u, list<udp_bind_t>::node* n) {
		if (n->data.handler->localPort == u.udp_header->dst_port && n->data.handler->localIp == u.dst_ip && n->data.handler->listening ) {
			n->data.handler->listening = false;
			n->data.handler->remotePort = u.udp_header->src_port;
			n->data.handler->remoteIp = u.src_ip;
			return true;
		}

		if (n->data.handler->remotePort == u.udp_header->src_port && (n->data.handler->remoteIp == u.src_ip || u.dst_ip == 0xFFFFFFFF || u.src_ip == 0xFFFFFFFF || n->data.handler->remoteIp == 0xFFFFFFFF)) {
			return true;
		}

		return false;
	}, search);

	if (n == nullptr) {
		return false;
	}

	udp_socket* socket = n->data.handler;

	socket->handle_udp_message(payload + sizeof(udp_header_t), size - sizeof(udp_header_t));

	return false;
}

udp_socket* udp_provider::connect(uint32_t ip, uint16_t port) {
	udp_socket* socket = new udp_socket(this);

	socket->remoteIp = ip;
	socket->remotePort = port;
	socket->localPort = free_port++;
	socket->localIp = backend->backend->nic->get_ip();

	socket->localPort = __builtin_bswap16(socket->localPort);
	socket->remotePort = __builtin_bswap16(socket->remotePort);

	udp_bind_t bind = {
		.port = socket->localPort,
		.handler = socket
	};

	this->binds.add(bind);

	return socket;
}

udp_socket* udp_provider::listen(uint16_t port) {
	udp_socket* socket = new udp_socket(this);

	socket->localPort = port;
	socket->localIp = backend->backend->nic->get_ip();
	socket->listening = true;

	socket->localPort = __builtin_bswap16(socket->localPort);

	udp_bind_t bind = {
		.port = socket->localPort,
		.handler = socket
	};

	this->binds.add(bind);

	return socket;
}

void udp_provider::disconnect(udp_socket* socket) {
	list<udp_bind_t>::node* n = binds.find<udp_socket*>([](udp_socket* s, list<udp_bind_t>::node* n) {
		return s == n->data.handler;
	}, socket);

	n->data.handler->~udp_socket();
	memory::free(n->data.handler);

	binds.remove(n);
}

void udp_provider::send(udp_socket* socket, uint8_t* data, size_t size) {
	uint16_t total_size = size + sizeof(udp_header_t);
	uint8_t* packet = (uint8_t*)memory::malloc(total_size);

	udp_header_t* udp = (udp_header_t*)packet;

	udp->src_port = socket->localPort;
	udp->dst_port = socket->remotePort;
	udp->length = __builtin_bswap16(total_size);

	memcpy(packet + sizeof(udp_header_t), data, size);

	udp->checksum = 0;

	ipv4_handler::send(socket->remoteIp, packet, total_size);

	memory::free(packet);
}

void udp_provider::bind(udp_socket* socket, udp_handler* handler) {
	socket->handler = handler;
}