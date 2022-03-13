#include <rtl8139.h>

#include <memory/page_frame_allocator.h>
#include <pci/pci_bar.h>

#include <utils/assert.h>
#include <utils/log.h>
#include <utils/port.h>

using namespace driver;

uint8_t TSAD_array[4] = { 0x20, 0x24, 0x28, 0x2C };
uint8_t TSD_array[4] = { 0x10, 0x14, 0x18, 0x1C };

rtl8139_driver::rtl8139_driver(pci::pci_header_0_t* header, uint16_t bus, uint16_t device, uint16_t function) : interrupt_handler(header->interrupt_line + 0x20) {
	this->header = header;
	this->bus = bus;
	this->device = device;
	this->function = function;
	this->ip = 0;
	current_packet_ptr = 0;
}

rtl8139_driver::~rtl8139_driver() {
	delete[] rx_buffer;
}


void rtl8139_driver::send(uint8_t* data, int32_t len) {
	assert(!((uint64_t) data > 0xffffffff));

	debugf("rtl8139_driver: sending packet: ");
	for(int i = 0; i < (len > 64 ? 64 : len); i++) {
		debugf_raw("%x ", data[i]);
	}
	debugf_raw("\n");

	outl(io_base + TSAD_array[tx_cur], (uint32_t) (uint64_t) data);
	outl(io_base + TSD_array[tx_cur++], len);
	if(tx_cur > 3) {
		tx_cur = 0;
	}
}

uint64_t rtl8139_driver::get_mac() {
	mac_u mac;
	uint32_t mac_part1 = inl(io_base + 0x00);
	uint16_t mac_part2 = inw(io_base + 0x04);
	mac.mac_p[0] = mac_part1 >> 0;
	mac.mac_p[1] = mac_part1 >> 8;
	mac.mac_p[2] = mac_part1 >> 16;
	mac.mac_p[3] = mac_part1 >> 24;

	mac.mac_p[4] = mac_part2 >> 0;
	mac.mac_p[5] = mac_part2 >> 8;

	return mac.mac;
}

uint32_t rtl8139_driver::get_ip() {
	return ip;
}

void rtl8139_driver::set_ip(uint32_t ip) {
	this->ip = ip;
}

void rtl8139_driver::receive() {
	uint16_t * t = (uint16_t*)(rx_buffer + current_packet_ptr);
	// Skip packet header, get packet length
	uint16_t packet_length = *(t + 1);

	debugf("Received packet of length %d\n", packet_length);

	// Skip, packet header and packet length, now t points to the packet data
	t = t + 2;

	debugf("rtl8139_driver: receiveing packet: ");
	for(int i = 0; i < (packet_length > 64 ? 64 : packet_length); i++) {
		debugf_raw("%x ", ((uint8_t*) t)[i]);
	}
	debugf_raw("\n");

	if (_nic_data_manager) {
		if (_nic_data_manager->recv((uint8_t*) t, packet_length)) {
			send((uint8_t*) t, packet_length);
		}
	} else {
		debugf("no handler\n");
	}

	current_packet_ptr = (current_packet_ptr + packet_length + 4 + 3) & RX_READ_POINTER_MASK;

	if(current_packet_ptr > RX_BUF_SIZE) {
		current_packet_ptr -= RX_BUF_SIZE;
	}

	outw(io_base + CAPR, current_packet_ptr - 0x10);
}

void rtl8139_driver::handle() {
	uint16_t status = inw(io_base + 0x3e);
	outw(io_base + 0x3E, 0x5);

	if(status & TOK) {
		debugf("Packet sent\n");
	}

	if (status & ROK) {
		debugf("Received packet\n");
		receive();
	}
}

void rtl8139_driver::activate() {
	bar_type = header->BAR0 & 1;
	io_base = header->BAR0 & (~3);
	mem_base = header->BAR0 & (~15);

	debugf("rtl8139: bar_type: %d, io_base: %x, mem_base: %x\n", bar_type, io_base, mem_base);

	tx_cur = 0;

	pci::become_bus_master(bus, device, function);

	outb(io_base + 0x52, 0x00);

	outb(io_base + 0x37, 0x10);
	while((inb(io_base + 0x37) & 0x10) != 0) {
		__asm__ __volatile__("pause" ::: "memory");
	}

	rx_buffer = (char*) memory::global_allocator.request_pages((8192 + 16 + 1500) / 0x1000 + 1);
	assert(!((uint64_t) rx_buffer > 0xffffffff));
	memset(rx_buffer, 0x0, 8192 + 16 + 1500);
	outl(io_base + 0x30, (uint32_t) (uint64_t) rx_buffer);

	// Sets the TOK and ROK bits high
	outw(io_base + 0x3C, 0x0005);

	// (1 << 7) is the WRAP bit, 0xf is AB+AM+APM+AAP
	outl(io_base + 0x44, 0xf | (1 << 7));

	// Sets the RE and TE bits high
	outb(io_base + 0x37, 0x0C);

	uint64_t mac = get_mac();
	mac_u _mac_u;
	_mac_u.mac = mac;

	debugf("rtl8139: mac: %x:%x:%x:%x:%x:%x\n", _mac_u.mac_p[0], _mac_u.mac_p[1], _mac_u.mac_p[2], _mac_u.mac_p[3], _mac_u.mac_p[4], _mac_u.mac_p[5]);

	driver::global_nic_manager->add_nic(this);
}

char* rtl8139_driver::get_name() {
	return (char*) "rtl8139";
}