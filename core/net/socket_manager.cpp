#include <net/socket_manager.h>

#include <driver/nic.h>

#include <memory/page_frame_allocator.h>

#include <utils/assert.h>
#include <utils/abort.h>
#include <utils/log.h>

using namespace net;

namespace net {
	socket_manager* global_socket_manager;
}

socket_manager_socket::socket_manager_socket(int socket_type) {
	assert(socket_type == TCP_SOCKET || socket_type == UDP_SOCKET);

	this->socket_type = socket_type;
	nic_id = 0;

	udp_socket_ptr = nullptr;
	tcp_socket_ptr = nullptr;

	received_data = nullptr;
	num_bytes_received = 0;
}

socket_manager_socket::~socket_manager_socket() {
	disconnect();
}

void socket_manager_socket::connect(uint32_t ip_be, uint16_t port) {
	switch (socket_type) {
		case TCP_SOCKET:
			tcp_socket_ptr = driver::global_nic_manager->get_nic(nic_id)->network_stack->tcp->connect(ip_be, port);
			driver::global_nic_manager->get_nic(nic_id)->network_stack->tcp->bind(tcp_socket_ptr, this);
			break;
		case UDP_SOCKET:
			udp_socket_ptr = driver::global_nic_manager->get_nic(nic_id)->network_stack->udp->connect(ip_be, port);
			driver::global_nic_manager->get_nic(nic_id)->network_stack->udp->bind(udp_socket_ptr, this);
			break;

		default:
			abortf("socket_manager_socket::connect(): invalid socket_type: %d", socket_type);
	}
}

void socket_manager_socket::disconnect() {
	switch (socket_type) {
		case TCP_SOCKET:
			if (tcp_socket_ptr != nullptr) {
				tcp_socket_ptr->disconnect();
				tcp_socket_ptr = nullptr;
			}
			break;
		case UDP_SOCKET:
			if (udp_socket_ptr != nullptr) {
				udp_socket_ptr->disconnect();
				udp_socket_ptr = nullptr;
			}
			break;

		default:
			abortf("socket_manager_socket::disconnect(): invalid socket_type: %d", socket_type);
	}
}

void socket_manager_socket::send(uint8_t* data, uint32_t size) {
	switch (socket_type) {
		case TCP_SOCKET:
			tcp_socket_ptr->send(data, size);
			break;
		case UDP_SOCKET:
			udp_socket_ptr->send(data, size);
			break;

		default:
			abortf("socket_manager_socket::send(): invalid socket_type: %d", socket_type);
	}

	debugf("socket_manager_socket::send(): sent %d bytes\n", size);
}

int socket_manager_socket::receive(uint8_t* data, uint32_t size) {
	while (num_bytes_received == 0) {
		__asm__ __volatile__("pause" ::: "memory");
	}

	int num_bytes_to_copy = num_bytes_received;

	if (num_bytes_to_copy > size) {
		num_bytes_to_copy = size;
	}

	memcpy(data, received_data, num_bytes_to_copy);

	num_bytes_received -= num_bytes_to_copy;
	memcpy(received_data, received_data + num_bytes_to_copy, num_bytes_received);

	return num_bytes_to_copy;
}

bool socket_manager_socket::on_tcp_message(tcp_socket* socket, uint8_t* data, size_t size) {
	debugf("socket_manager_socket::on_tcp_message(): %d bytes\n", size);
	if (socket != tcp_socket_ptr) {
		return false;
	}

	if (received_data == nullptr) {
		received_data = (uint8_t*) memory::global_allocator.request_pages(size / 0x1000 + 1);
	} else {
		uint8_t* new_data = (uint8_t*) memory::global_allocator.request_pages(num_bytes_received + size / 0x1000 + 1);
		memcpy(new_data, received_data, num_bytes_received);
		memory::global_allocator.free_pages(received_data, num_bytes_received / 0x1000 + 1);
	}
	
	memcpy(received_data + num_bytes_received, data, size);
	num_bytes_received += size;

	return true;
}

void socket_manager_socket::on_udp_message(udp_socket* socket, uint8_t* data, size_t size) {
	debugf("socket_manager_socket::on_udp_message(): %d bytes\n", size);
	if (socket != udp_socket_ptr) {
		return;
	}

	if (received_data == nullptr) {
		received_data = (uint8_t*) memory::global_allocator.request_pages(size / 0x1000 + 1);
	} else {
		uint8_t* new_data = (uint8_t*) memory::global_allocator.request_pages(num_bytes_received + size / 0x1000 + 1);
		memcpy(new_data, received_data, num_bytes_received);
		memory::global_allocator.free_pages(received_data, num_bytes_received / 0x1000 + 1);
	}

	memcpy(received_data + num_bytes_received, data, size);
	num_bytes_received += size;
}

socket_manager::socket_manager() : sockets(10) {
	this->curr_socket = 10;
}

int socket_manager::alloc_socket() {
	return ++this->curr_socket;
}

void socket_manager::free_socket(int socket_id) {
	list<socket_manager_socket*>::node* n = this->sockets.find<int>([](int _socket_id, list<socket_manager_socket*>::node* n) {
		return _socket_id == n->data->socket_id;
	}, socket_id);

	if (!(n == nullptr)) {
		sockets.remove(n);
	}
}
void socket_manager::register_socket(socket_manager_socket* sock) {
	sockets.add(sock);
}

socket_manager_socket* socket_manager::get_socket(int socket_id) {
	list<socket_manager_socket*>::node* n = this->sockets.find<int>([](int _socket_id, list<socket_manager_socket*>::node* n) {
		return _socket_id == n->data->socket_id;
	}, socket_id);

	if (!(n == nullptr)) {
		return n->data;
	} else {
		return nullptr;
	}
}