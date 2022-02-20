#include <net/tcp.h>

#include <utils/log.h>

#include <timer/timer.h>

using namespace net;

tcp_handler::tcp_handler() {

}

tcp_handler::~tcp_handler() {

}

bool tcp_handler::on_tcp_message(tcp_socket* socket, uint8_t* data, size_t size) {
	return true;
}

tcp_socket::tcp_socket(tcp_provider* provider): acknowledged(1) {
	this->provider = provider;
	this->handler = nullptr;
	this->state = tcp_socket_state::CLOSED;
}

tcp_socket::~tcp_socket() {
	this->acknowledged.~list();
}

bool tcp_socket::handle_tcp_message(uint8_t* data, size_t size) {
	if (this->handler != nullptr) {
		return this->handler->on_tcp_message(this, data, size);
	} else {
		return false;
	}
}

void tcp_socket::send(uint8_t* data, size_t size) {
	uint64_t timeout = 1000;
	while (this->state != tcp_socket_state::ESTABLISHED) {
		if (timeout-- == 0) {
			debugf("TCP: Send timeout, connection not established.\n");
			return;
		}

		timer::global_timer->sleep(1);
	}
	provider->send(this, data, size, tcp_flag::PSH | tcp_flag::ACK);
}

void tcp_socket::disconnect() {
	provider->disconnect(this);
}

tcp_provider::tcp_provider(ipv4_provider *ipv4_provider): ipv4_handler(ipv4_provider, 0x06), binds(100) {
}

struct tcp_search_t {
	uint32_t src_ip;
	uint32_t dst_ip;
	tcp_header_t* tcp_header;
};

bool tcp_provider::on_internet_protocol_received(uint32_t srcIP_BE, uint32_t dstIP_BE, uint8_t* payload, uint32_t size) {
	if (size < 20) {
		return false;
	}

	tcp_header_t* tcp = (tcp_header_t*)payload;

	tcp_search_t search = {
		.src_ip = srcIP_BE,
		.dst_ip = dstIP_BE,
		.tcp_header = tcp
	};

	list<tcp_bind_t>::node* n = binds.find<tcp_search_t>([](tcp_search_t u, list<tcp_bind_t>::node* n) {
		if (n->data.handler->local_port == u.tcp_header->dst_port && n->data.handler->local_ip == u.dst_ip && n->data.handler->state == tcp_socket_state::LISTEN && (u.tcp_header->flags & (tcp_flag::SYN | tcp_flag::ACK)) == tcp_flag::SYN) {
			return true;
		}

		if (n->data.handler->remote_port == u.tcp_header->src_port && (n->data.handler->remote_ip == u.src_ip || u.dst_ip == 0xFFFFFFFF || u.src_ip == 0xFFFFFFFF || n->data.handler->remote_ip == 0xFFFFFFFF)) {
			return true;
		}

		return false;
	}, search);

	if (n == nullptr) {
		return false;
	}

	tcp_socket* socket = n->data.handler;
	bool reset = false;

	if (socket != 0 && tcp->flags & tcp_flag::RST) {
		socket->state = tcp_socket_state::CLOSED;
	}

	if (socket != 0 && socket->state != tcp_socket_state::CLOSED) {
		if (socket->seq_num != __builtin_bswap32(tcp->ack)) {
			if (tcp->flags & (1 << 4)) { //If the message has the ACK flag.
				list<tcp_sent_packet_t>::node* ack_node = socket->acknowledged.find<uint32_t>([](uint32_t ack_num, list<tcp_sent_packet_t>::node* ack_node) {
					int add = 0;
					if (ack_node->data.data_empty) {
						add = 1;
					}

					if (ack_node->data.seq_num + add == ack_num) {
						return true;
					}

					return false;
				}, __builtin_bswap32(tcp->ack));

				if (ack_node == nullptr) {
					reset = true;
					debugf("TCP: Packet that wasn't sent was acknowledged, connection reset.\n");
				} else {
					ack_node->data.was_acked = true;
				}
			}
		}

		if (!reset) {
			switch((tcp->flags) & (tcp_flag::SYN | tcp_flag::ACK | tcp_flag::FIN)) {
				case tcp_flag::SYN: {
					if (socket->state == tcp_socket_state::LISTEN) {
						socket->state = tcp_socket_state::SYN_RECEIVED;
						socket->remote_port = tcp->src_port;
						socket->remote_ip = srcIP_BE;
						socket->ack_num = __builtin_bswap32(tcp->seq_num) + 1;

						#warning The TCP sequence starting offset should be random
						socket->seq_num = 0xbeefcafe;

						send(socket, 0, 0, tcp_flag::SYN | tcp_flag::ACK);
						socket->seq_num++;
					} else {
						reset = true;
					}
					break;
				}
				case tcp_flag::SYN | tcp_flag::ACK: {
					if (socket->state == tcp_socket_state::SYN_SENT) {
						socket->state = tcp_socket_state::ESTABLISHED;
						socket->ack_num = __builtin_bswap32(tcp->seq_num) + 1;
						socket->seq_num++;

						send(socket, 0, 0, tcp_flag::ACK);
					} else {
						reset = true;
					}
					break;
				}
				case tcp_flag::SYN | tcp_flag::FIN:
				case tcp_flag::SYN | tcp_flag::FIN | tcp_flag::ACK: {
					reset = true;
					break;
				}
				case tcp_flag::FIN:
				case tcp_flag::FIN | tcp_flag::ACK: {
					if (socket->state == tcp_socket_state::ESTABLISHED) {
						socket->state = tcp_socket_state::CLOSE_WAIT;
						socket->ack_num++;

						send(socket, 0, 0, tcp_flag::ACK);
						send(socket, 0, 0, tcp_flag::FIN | tcp_flag::ACK);
						debugf("TCP: Socket closed.\n");
					} else if (socket->state == tcp_socket_state::CLOSE_WAIT) {
						socket->state = tcp_socket_state::CLOSED;
					} else if (socket->state == tcp_socket_state::FIN_WAIT_1 || socket->state == tcp_socket_state::FIN_WAIT_2) {
						socket->state = tcp_socket_state::CLOSED;
						socket->ack_num++;
						debugf("TCP: Socket closed.\n");
						send(socket, 0, 0, tcp_flag::ACK);
					} else {
						reset = true;
					}
					break;
				}
				case tcp_flag::ACK: {
					if (socket->state == tcp_socket_state::SYN_RECEIVED) {
						socket->state = tcp_socket_state::ESTABLISHED;
						return false;
					} else if (socket->state == tcp_socket_state::FIN_WAIT_1) {
						socket->state = tcp_socket_state::FIN_WAIT_2;
						return false;
					} else if (socket->state == tcp_socket_state::CLOSE_WAIT) {
						socket->state = tcp_socket_state::CLOSED;
						break;
					} else if (*(payload + (tcp->data_offset * 4)) == 0) {
						break;
					}
				}
				default:
					if (__builtin_bswap32(tcp->seq_num) == socket->ack_num) {
						debugf("TCP: Processigng packet.\n");
						reset = !(socket->handle_tcp_message(payload + (tcp->data_offset * 4), size - (tcp->data_offset * 4)));
						if (!reset) { //Acknowledge a sent message.
							socket->ack_num += size - (tcp->data_offset * 4);
							send(socket, 0, 0, tcp_flag::ACK);
						}
					} else { //Data is in the wrong order
						debugf("TCP: Reset because packets are in the wrong order.\n");
						reset = true;
					}
			}
		}
	}

	if (reset) {
		debugf("TCP: Conection was reset.\n");
		if (socket != 0) {
			send(socket, 0, 0, tcp_flag::RST);
		} else {
			tcp_socket socketTMP = tcp_socket(this);
			socketTMP.remote_port = tcp->src_port;
			socketTMP.remote_ip = srcIP_BE;
			socketTMP.local_port = tcp->dst_port;
			socketTMP.local_ip = dstIP_BE;
			socketTMP.seq_num = __builtin_bswap32(tcp->ack);
			socketTMP.ack_num = __builtin_bswap32(tcp->seq_num) + 1;
			send(&socketTMP, 0, 0, tcp_flag::RST);
		}
		return false;
	}

	if (socket->state == tcp_socket_state::CLOSED) {
		list<tcp_bind_t>::node* n = binds.find<tcp_socket*>([](tcp_socket* s, list<tcp_bind_t>::node* n) {
			return s == n->data.handler;
		}, socket);

		n->data.handler->~tcp_socket();
		memory::free(n->data.handler);

		binds.remove(n);
	}

	return false;
}

tcp_socket* tcp_provider::connect(uint32_t ip, uint16_t port) {
	tcp_socket* socket = new tcp_socket(this);

	socket->remote_ip = ip;
	socket->remote_port = port;
	socket->local_port = free_port++;
	socket->local_ip = backend->backend->nic->get_ip();

	socket->local_port = __builtin_bswap16(socket->local_port);
	socket->remote_port = __builtin_bswap16(socket->remote_port);

	tcp_bind_t bind = {
		.port = socket->local_port,
		.handler = socket
	};

	this->binds.add(bind);
	socket->state = tcp_socket_state::SYN_SENT;

	#warning The TCP sequence starting offset should be random
	socket->seq_num = 0xbeefcafe;

	send(socket, 0, 0, tcp_flag::SYN);

	return socket;
}

tcp_socket* tcp_provider::listen(uint16_t port) {
	tcp_socket* socket = new tcp_socket(this);

	socket->local_port = port;
	socket->local_ip = backend->backend->nic->get_ip();
	socket->state = tcp_socket_state::LISTEN;

	socket->local_port = __builtin_bswap16(socket->local_port);

	tcp_bind_t bind = {
		.port = socket->local_port,
		.handler = socket
	};

	this->binds.add(bind);

	return socket;
}

void tcp_provider::disconnect(tcp_socket* socket) {
	socket->state = tcp_socket_state::FIN_WAIT_1;
	send(socket, 0, 0, tcp_flag::FIN + tcp_flag::ACK);
	socket->seq_num++;
}

void tcp_provider::send(tcp_socket* socket, uint8_t* data, size_t size, uint16_t flags) {
	uint16_t total_size = size + sizeof(tcp_header_t);
	uint16_t total_size_phdr = total_size + sizeof(tcp_pseudo_header_t);

	uint8_t* packet = (uint8_t*) memory::malloc(total_size_phdr);
	memset(packet, 0, total_size_phdr);

	tcp_pseudo_header_t* phdr = (tcp_pseudo_header_t*)packet;
	tcp_header_t* tcp = (tcp_header_t*)(packet + sizeof(tcp_pseudo_header_t));

	tcp->data_offset = sizeof(tcp_header_t) / 4;
	tcp->src_port = socket->local_port;
	tcp->dst_port = socket->remote_port;

	tcp->ack = __builtin_bswap32(socket->ack_num);
	tcp->seq_num = __builtin_bswap32(socket->seq_num);
	tcp->reserved = 0;
	tcp->flags = flags;
	#warning Maybe this should be set to actually look at the amount of RAM available
	tcp->window_size = 0xFFFF;
	tcp->urgent_ptr = 0;
	tcp->options = ((flags & tcp_flag::SYN) != 0 ? 0xB4050402 : 0);

	socket->seq_num += size;

	memcpy(packet + sizeof(tcp_header_t) + sizeof(tcp_pseudo_header_t), data, size);

	phdr->src_ip = socket->local_ip;
	phdr->dst_ip = socket->remote_ip;
	phdr->protocol = 0x0600;
	phdr->total_len = __builtin_bswap16(total_size);

	tcp->checksum = 0;
	tcp->checksum = this->backend->checksum((uint16_t*) packet, total_size_phdr);

	bool expects_ack = false;
	if ((flags != (tcp_flag::FIN | tcp_flag::ACK) && flags != tcp_flag::ACK && flags != tcp_flag::RST)) {
		expects_ack = true;
	}

	debugf("expects_ack: %d\n", expects_ack);

	list<net::tcp_sent_packet_t>::node* list_node;
	if (expects_ack) {
		tcp_sent_packet_t sent_packet;
		sent_packet.seq_num = socket->seq_num;
		sent_packet.data_empty = size == 0;
		sent_packet.was_acked = false;
		list_node = socket->acknowledged.add(sent_packet);
	}

	ipv4_handler::send(socket->remote_ip, (uint8_t*) tcp, total_size);

	debugf("expects_ack: %d\n", expects_ack);
	bool retransmit = false;
	if (expects_ack) {
		int timeout = 1000;
		while (!list_node->data.was_acked && socket->state == tcp_socket_state::ESTABLISHED) {
			if (--timeout == 0) {
				debugf("TCP: Message timeout, retransmitting.\n");
				retransmit = true;
				break;
			}

			timer::global_timer->sleep(10);
		}
	}

	if (retransmit && socket->state == tcp_socket_state::ESTABLISHED) {
		this->retransmit(socket, packet, size, list_node);
	} else {
		retransmit = false;
		if (socket->state == tcp_socket_state::SYN_SENT) {
			int timeout = 1000;
			while (!list_node->data.was_acked && socket->state == tcp_socket_state::SYN_SENT) {
				if (--timeout == 0) {
					debugf("TCP: Connection timeout.\n");
					retransmit = true;
					break;
				}

				timer::global_timer->sleep(10);
			}
		}
		if (retransmit) {
			send(socket, 0, 0, tcp_flag::RST);
			socket->state = tcp_socket_state::CLOSED;
		}

		socket->acknowledged.remove(list_node);
		memory::free(packet);
	}
}

void tcp_provider::retransmit(tcp_socket* socket, uint8_t* packet, size_t size, list<net::tcp_sent_packet_t>::node* list_node) {
	if (socket->state != tcp_socket_state::ESTABLISHED) {
		socket->acknowledged.remove(list_node);
		memory::free(packet);
		return;
	}

	ipv4_handler::send(socket->remote_ip, packet, size);

	bool retransmit = false;
	int timeout = 1000;
	while (!list_node->data.was_acked && socket->state == tcp_socket_state::ESTABLISHED) {
		if (--timeout == 0) {
			debugf("TCP: Message timeout, retransmitting.\n");
			retransmit = true;
			break;
		}

		timer::global_timer->sleep(10);
	}

	if (retransmit && socket->state == tcp_socket_state::ESTABLISHED) {
		this->retransmit(socket, packet, size, list_node);
	} else {
		socket->acknowledged.remove(list_node);
		memory::free(packet);
	}
}

void tcp_provider::bind(tcp_socket* socket, tcp_handler* handler) {
	socket->handler = handler;
}