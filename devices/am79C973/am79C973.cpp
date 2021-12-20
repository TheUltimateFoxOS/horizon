#include <am79C973.h>

#include <memory/page_frame_allocator.h>
#include <pci/pci_bar.h>

#include <utils/assert.h>
#include <utils/log.h>
#include <utils/port.h>

using namespace driver;

am79C973_driver::am79C973_driver(pci::pci_header_0_t* header, uint16_t bus, uint16_t device, uint16_t function) : interrupt_handler(header->interrupt_line + 0x20) {
	this->header = header;
	this->_nic_data_manager = nullptr;

	this->sendBufferDescrMemory = (uint8_t*) memory::global_allocator.request_page();
	memset(this->sendBufferDescrMemory, 0, 4096);
	this->sendBuffers = (uint8_t*) memory::global_allocator.request_pages(5);
	memset(this->sendBuffers, 0, 5 * 4096);
	this->recvBufferDescrMemory = (uint8_t*) memory::global_allocator.request_page();
	memset(this->recvBufferDescrMemory, 0, 4096);
	this->recvBuffers = (uint8_t*) memory::global_allocator.request_pages(5);
	memset(this->recvBuffers, 0, 5 * 4096);
	this->init_block = (initialization_block_t*) memory::global_allocator.request_page();

	assert(!((uint64_t) this->sendBufferDescrMemory > 0xffffffff));
	assert(!((uint64_t) this->sendBuffers > 0xffffffff));
	assert(!((uint64_t) this->recvBufferDescrMemory > 0xffffffff));
	assert(!((uint64_t) this->recvBuffers > 0xffffffff));
	assert(!((uint64_t) this->init_block > 0xffffffff));

	currentSendBuffer = 0;
	currentRecvBuffer = 0;

	debugf("am79C973_driver: interrupt %d\n", header->interrupt_line + 0x20);

	this->base_port = pci::get_io_port(header, bus, device, function);

	debugf("am79C973_driver: base_port: %d\n", this->base_port);

	Port16Bit reset_port(this->base_port + 0x14);
	reset_port.Read();
	reset_port.Write(0x0);

	uint64_t mac0 = inw(this->base_port + 0x00) % 256;
	uint64_t mac1 = inw(this->base_port + 0x00) / 256;
	uint64_t mac2 = inw(this->base_port + 0x02) % 256;
	uint64_t mac3 = inw(this->base_port + 0x02) / 256;
	uint64_t mac4 = inw(this->base_port + 0x04) % 256;
	uint64_t mac5 = inw(this->base_port + 0x04) / 256;

	uint64_t MAC = mac5 << 40 | mac4 << 32 | mac3 << 24 | mac2 << 16 | mac1 << 8 | mac0;
	debugf("am79C973_driver: MAC: %x:%x:%x:%x:%x:%x\n", mac0, mac1, mac2, mac3, mac4, mac5);

	this->init_block->mode = 0x0000; // promiscuous mode = false
	this->init_block->reserved1 = 0;
	this->init_block->numSendBuffers = 3;
	this->init_block->reserved2 = 0;
	this->init_block->numRecvBuffers = 3;
	this->init_block->physicalAddress = MAC;
	this->init_block->reserved3 = 0;
	this->init_block->logicalAddress = 0;

	Port16Bit register_data_port(this->base_port + 0x10);
	Port16Bit register_address_port(this->base_port + 0x12);
	Port16Bit bus_control_register_data_port(this->base_port + 0x16);

	// enable 32 bit mode
	register_address_port.Write(20);
	bus_control_register_data_port.Write(0x102);

	// stop reset
	register_address_port.Write(0x0);
	register_data_port.Write(0x4);

	sendBufferDescr = (buffer_descriptor_t*)(uint64_t)((((uint32_t)(uint64_t)&sendBufferDescrMemory[0]) + 15) & ~((uint32_t)0xF));
	this->init_block->sendBufferDescrAddress = (uint32_t)(uint64_t)sendBufferDescr;
	recvBufferDescr = (buffer_descriptor_t*)(uint64_t)((((uint32_t)(uint64_t)&recvBufferDescrMemory[0]) + 15) & ~((uint32_t)0xF));
	this->init_block->recvBufferDescrAddress = (uint32_t)(uint64_t)recvBufferDescr;

	for (int i = 0; i < 8; i++) {
		sendBufferDescr[i].address = (((uint32_t)(uint64_t)&sendBuffers[i]) + 15 ) & ~(uint32_t)0xF;
		sendBufferDescr[i].flags = 0x7FF | 0xF000;
		sendBufferDescr[i].flags2 = 0;
		sendBufferDescr[i].avail = 0;
		
		recvBufferDescr[i].address = (((uint32_t)(uint64_t)&recvBuffers[i]) + 15 ) & ~(uint32_t)0xF;
		recvBufferDescr[i].flags = 0xF7FF | 0x80000000;
		recvBufferDescr[i].flags2 = 0;
		recvBufferDescr[i].avail = 0;
	}

	register_address_port.Write(0x1);
	register_data_port.Write((uint32_t)(uint64_t)this->init_block & 0xffff);
	register_address_port.Write(0x2);
	register_data_port.Write(((uint32_t)(uint64_t)this->init_block >> 16) & 0xffff);
}

am79C973_driver::~am79C973_driver() {
	memory::global_allocator.free_page(this->sendBufferDescrMemory);
	memory::global_allocator.free_pages(this->sendBuffers, 5);
	memory::global_allocator.free_page(this->recvBufferDescrMemory);
	memory::global_allocator.free_pages(this->recvBuffers, 5);
	memory::global_allocator.free_page(this->init_block);
}

void am79C973_driver::activate() {
	if (this->base_port == 0) {
		debugf("Am79C973Driver start-up failed: IO port is 0.\n");
		return;
	}

	Port16Bit register_data_port(this->base_port + 0x10);
	Port16Bit register_address_port(this->base_port + 0x12);

	register_address_port.Write(0x0);
	register_data_port.Write(0x41);

	register_address_port.Write(0x4);
	uint32_t tmp = register_data_port.Read();
	register_address_port.Write(0x4);
	register_data_port.Write(tmp | 0xc00);

	register_address_port.Write(0x0);
	register_data_port.Write(0x42);

	this->set_ip(0);

	driver::global_nic_manager->add_nic(this);
}

//#define DEBUG

void am79C973_driver::handle() {
	Port16Bit register_data_port(this->base_port + 0x10);
	Port16Bit register_address_port(this->base_port + 0x12);

	register_address_port.Write(0x0);
	uint32_t temp = register_data_port.Read();

#ifdef DEBUG
	debugf("am79C973_driver: interrupt: %x\n", temp);
#endif

	if ((temp & 0x100) == 0x100) {
		debugf("am79C973_driver: init done\n");
		// this->send((uint8_t*) "Hello World!\n", 12);
	}

#ifdef DEBUG
	if((temp & 0x8000) == 0x8000) debugf("AMD am79c973 ERROR\n");
	if((temp & 0x2000) == 0x2000) debugf("AMD am79c973 COLLISION ERROR\n");
	if((temp & 0x1000) == 0x1000) debugf("AMD am79c973 MISSED FRAME\n");
	if((temp & 0x0800) == 0x0800) debugf("AMD am79c973 MEMORY ERROR\n");
	if((temp & 0x0400) == 0x0400) debugf("AMD am79c973 RECEIVE\n");
	if((temp & 0x0200) == 0x0200) debugf("AMD am79c973 TRANSMIT\n");
#endif

	if ((temp & 0x0400) == 0x0400) {
		this->receive();
	}

	register_address_port.Write(0x0);
	register_data_port.Write(temp);
}

void am79C973_driver::send(uint8_t* data, int32_t length) {
	Port16Bit register_data_port(this->base_port + 0x10);
	Port16Bit register_address_port(this->base_port + 0x12);

	int send_descriptor = currentSendBuffer;
	currentSendBuffer = (currentSendBuffer + 1) % 8;

	if (length > 1518) {
		debugf("am79C973_driver: packet too long\n");
		length = 1518;
	}

	memcpy((uint8_t*) (uint64_t) sendBufferDescr[send_descriptor].address, data, length);

#ifdef DEBUG
	debugf("Am79C973Driver: sending packet: ");
	for(int i = 0; i < (length > 64 ? 64 : length); i++) {
		debugf_intrnl("%x ", data[i]);
	}
	debugf_intrnl("\n");
#endif

	sendBufferDescr[send_descriptor].avail = 0;
	sendBufferDescr[send_descriptor].flags2 = 0;
	sendBufferDescr[send_descriptor].flags = 0x8300F000 | ((uint16_t)((-length) & 0xFFF));

	register_address_port.Write(0x0);
	register_data_port.Write(0x48);
}

void am79C973_driver::receive() {
	for (; (recvBufferDescr[currentRecvBuffer].flags & 0x80000000) == 0; currentRecvBuffer = (currentRecvBuffer + 1) % 8) {
		if (!(recvBufferDescr[currentRecvBuffer].flags & 0x40000000) && (recvBufferDescr[currentRecvBuffer].flags & 0x03000000) == 0x03000000) {
			uint32_t size = recvBufferDescr[currentRecvBuffer].flags & 0xFFF;

		#ifdef DEBUG
			debugf("am79C973_driver: received packet of size %d\n", size);
		#endif

			if (size > 64) {
				size -= 4;
			}

			uint8_t* data = (uint8_t*) (uint64_t) recvBufferDescr[currentRecvBuffer].address;

		#ifdef DEBUG
			debugf("am79C973_driver: receiveing packet: ");
			for(int i = 0; i < (size > 64 ? 64 : size); i++) {
				debugf_intrnl("%x ", data[i]);
			}
			debugf_intrnl("\n");
		#endif

			if (_nic_data_manager) {
				if (_nic_data_manager->recv(data, size)) {
					send(data, size);
				}
			} else {
				debugf("am79C973_driver: no handler\n");
			}

			recvBufferDescr[currentRecvBuffer].flags2 = 0;
			recvBufferDescr[currentRecvBuffer].flags = 0x8000F7FF;
		} else {
			debugf("am79C973_driver: packet not ready\n");
		}
	}
}

char* am79C973_driver::get_name() {
	return (char*) "am79C973";
}

uint64_t am79C973_driver::get_mac() {
	uint64_t mac0 = inw(this->base_port + 0x00) % 256;
	uint64_t mac1 = inw(this->base_port + 0x00) / 256;
	uint64_t mac2 = inw(this->base_port + 0x02) % 256;
	uint64_t mac3 = inw(this->base_port + 0x02) / 256;
	uint64_t mac4 = inw(this->base_port + 0x04) % 256;
	uint64_t mac5 = inw(this->base_port + 0x04) / 256;

	uint64_t MAC = mac5 << 40 | mac4 << 32 | mac3 << 24 | mac2 << 16 | mac1 << 8 | mac0;
	return MAC;
}

uint32_t am79C973_driver::get_ip() {
	return this->init_block->logicalAddress;
}

void am79C973_driver::set_ip(uint32_t ip) {
	this->init_block->logicalAddress = ip;
}