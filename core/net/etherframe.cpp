#include <net/etherframe.h>

#include <memory/page_frame_allocator.h>

#include <utils/log.h>
#include <utils/string.h>

using namespace net;

ether_frame_handler::ether_frame_handler(ether_frame_provider* backend, uint16_t ether_type) {
	this->ether_type_be = ((ether_type & 0x00FF) << 8) | ((ether_type & 0xFF00) >> 8);
	this->backend = backend;
	backend->handlers.add(this);
}

ether_frame_handler::~ether_frame_handler() {
	this->backend->handlers.remove(this->backend->handlers.find<ether_frame_handler*>([](ether_frame_handler* h, list<ether_frame_handler*>::node* n) {
		return h == n->data;
	}, this));
}

bool ether_frame_handler::on_ether_frame_received(uint8_t* payload, uint32_t size) {
	debugf("Unhandled etherframe packet!\n");
	return false;
}

void ether_frame_handler::send(uint64_t dest_mac_be, uint8_t* payload, uint32_t size) {
	this->backend->send_f(dest_mac_be, this->ether_type_be, payload, size);
}

ether_frame_provider::ether_frame_provider(int nic_id) : driver::nic_data_manager(nic_id), handlers(100) {

}

ether_frame_provider::~ether_frame_provider() {

}

bool ether_frame_provider::recv(uint8_t* data, int32_t size) {
	ether_frame_header_t* frame = (ether_frame_header_t*) data;

	bool send_back = false;

	if (frame->dest_mac_be == 0xFFFFFFFFFFFF || frame->dest_mac_be == nic->get_mac()) {
		list<ether_frame_handler*>::node* n = this->handlers.find<uint16_t>([](uint16_t t, list<ether_frame_handler*>::node* n) {
			return t == n->data->ether_type_be;
		}, frame->ether_type_be);

		if (n != nullptr) {
			send_back = n->data->on_ether_frame_received(data + sizeof(ether_frame_header_t), size - sizeof(ether_frame_header_t));
		} else {
			debugf("Unhandled etherframe!\n");
		}
	}

	if (send_back) {
		frame->dest_mac_be = frame->src_mac_be;
		frame->src_mac_be = this->nic->get_mac();
	}

	return send_back;
}

void ether_frame_provider::send_f(uint64_t dest_mac_be, uint16_t ether_type_be, uint8_t* payload, uint32_t size) {
	uint8_t* buffer = (uint8_t*) memory::global_allocator.request_page();

	ether_frame_header_t* frame = (ether_frame_header_t*) buffer;

	frame->dest_mac_be = dest_mac_be;
	frame->src_mac_be = this->nic->get_mac();
	frame->ether_type_be = ether_type_be;

	memcpy(buffer + sizeof(ether_frame_header_t), payload, size);
	this->nic->send(buffer, size + sizeof(ether_frame_header_t));
	memory::global_allocator.free_page(buffer);
}