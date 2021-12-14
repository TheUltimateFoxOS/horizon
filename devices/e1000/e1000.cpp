#include <e1000.h>

#include <memory/page_frame_allocator.h>
#include <memory/page_table_manager.h>

#include <pci/pci_bar.h>

#include <utils/assert.h>
#include <utils/log.h>
#include <utils/port.h>
#include <utils/mmio.h>

using namespace driver;

#define DEBUG

e1000_driver::e1000_driver(pci::pci_header_0_t* header, uint16_t bus, uint16_t device, uint16_t function) : interrupt_handler(header->interrupt_line + 0x20) {
	this->header = header;
	this->_nic_data_manager = nullptr;

	pci::enable_mmio(bus, device, function);

	debugf("e1000_driver: interrupt %d\n", header->interrupt_line + 0x20);

	for (int i = 0; i < 6; i++) {
		uint32_t* bar_ptr = (uint32_t*) (&header->BAR0 + i * sizeof(uint32_t));
		pci::pci_bar_t pci_bar = pci::get_bar(&header->BAR0, i, bus, device, function);

		if (pci_bar.type == pci::pci_bar_type_t::MMIO32 || pci_bar.type == pci::pci_bar_type_t::MMIO64) {
			this->bar_type = 0;
			this->mem_base = pci_bar.mem_address;
			memory::global_page_table_manager.map_range((void*) this->mem_base, (void*) this->mem_base, pci_bar.size);
			debugf("e1000_driver: MMIO address: 0x%x\n", this->mem_base);
#ifdef DEBUG
			debugf("e1000_driver: SIZE = %d; BAR TYPE = %d\n", pci_bar.size, pci_bar.type);
#endif
			break;
		} else if (pci_bar.type == pci::pci_bar_type_t::IO) {
			this->bar_type = 1;
			this->io_port = pci_bar.io_address;
			debugf("e1000_driver: IO port: %d\n", this->io_port);
#ifdef DEBUG
			debugf("e1000_driver: SIZE = %d; BAR TYPE = %d\n", pci_bar.size, pci_bar.type);
#endif
			break;
		}
	}

	this->rx_ptr = (uint8_t*) memory::global_allocator.request_pages(((sizeof(struct e1000_rx_desc) * e1000_NUM_RX_DESC + 16) / 0x1000) + 1);
	this->tx_ptr = (uint8_t*) memory::global_allocator.request_pages(((sizeof(struct e1000_tx_desc) * e1000_NUM_TX_DESC + 16) / 0x1000) + 1);

	pci::become_bus_master(bus, device, function);
	this->eerprom_exists = false;
}

e1000_driver::~e1000_driver() {
	for (int i = 0; i < e1000_NUM_RX_DESC; i++) {
		memory::global_allocator.free_pages((void*) this->rx_descs[i]->addr, ((8192 + 16) / 0x1000) + 1);
	}

	memory::global_allocator.free_pages(this->rx_ptr, ((sizeof(struct e1000_rx_desc) * e1000_NUM_RX_DESC + 16) / 0x1000) + 1);
	memory::global_allocator.free_pages(this->tx_ptr, ((sizeof(struct e1000_tx_desc) * e1000_NUM_TX_DESC + 16) / 0x1000) + 1);
}

void e1000_driver::write_command(uint16_t address, uint32_t value) {
	if (this->bar_type == 0) {
		MMIO::write32(this->mem_base + address, value);
	} else {
		outl(this->io_port, address);
		outl(this->io_port + 4, value);
	}
}

uint32_t e1000_driver::read_command(uint16_t address) {
	if (this->bar_type == 0) {
		return MMIO::read32(this->mem_base + address);
	} else {
		outl(this->io_port, address);
		return inl(this->io_port + 4);
	}
}

bool e1000_driver::detect_eeprom() {
	uint32_t val = 0;
	write_command(REG_EEPROM, 0x1);

	for(int i = 0; i < 1000 && !this->eerprom_exists; i++) {
		if(read_command(REG_EEPROM) & 0x10) {
			this->eerprom_exists = true;
		}
	}
	return this->eerprom_exists;
}

uint32_t e1000_driver::read_eeprom(uint8_t addr) {
	uint16_t data = 0;
	uint32_t tmp = 0;
	if (this->eerprom_exists) {
		write_command(REG_EEPROM, (1) | ((uint32_t)(addr) << 8));
		while(!((tmp = read_command(REG_EEPROM)) & (1 << 4)));
	} else {
		write_command(REG_EEPROM, (1) | ((uint32_t)(addr) << 2));
		while(!((tmp = read_command(REG_EEPROM)) & (1 << 1)));
	}
	data = (uint16_t)((tmp >> 16) & 0xFFFF);
	return data;
}

void e1000_driver::read_mac_address() {
	if (this->eerprom_exists) {
		uint32_t temp;
		temp = read_eeprom(0);
		this->mac[0] = temp & 0xff;
		this->mac[1] = temp >> 8;
		temp = read_eeprom(1);
		this->mac[2] = temp & 0xff;
		this->mac[3] = temp >> 8;
		temp = read_eeprom(2);
		this->mac[4] = temp & 0xff;
		this->mac[5] = temp >> 8;
	} else {
		uint32_t mac_low = read_command(0x5400);
		uint32_t mac_high = read_command(0x5404);

		this->mac[0] = mac_low & 0xff;
        this->mac[1] = mac_low >> 8 & 0xff;
        this->mac[2] = mac_low >> 16 & 0xff;
        this->mac[3] = mac_low >> 24 & 0xff;

        this->mac[4] = mac_high & 0xff;
        this->mac[5] = mac_high >> 8 & 0xff;
	}
}

void e1000_driver::rx_init() {
	struct e1000_rx_desc* descs = (struct e1000_rx_desc*) this->rx_ptr;
	for (int i = 0; i < e1000_NUM_RX_DESC; i++) {
		this->rx_descs[i] = (struct e1000_rx_desc*) ((uint8_t*)descs + i * 16);
		this->rx_descs[i]->addr = (uint64_t)(uint8_t *) memory::global_allocator.request_pages(((8192 + 16) / 0x1000) + 1);
		this->rx_descs[i]->status = 0;
	}

	write_command(REG_RXDESCLO, (uint64_t) this->rx_ptr);
	write_command(REG_RXDESCHI, 0);

	write_command(REG_RXDESCLEN, e1000_NUM_RX_DESC * 16);

	write_command(REG_RXDESCHEAD, 0);
	write_command(REG_RXDESCTAIL, e1000_NUM_RX_DESC - 1);
	this->rx_cur = 0;

	write_command(REG_RCTRL, RCTL_EN| RCTL_SBP| RCTL_UPE | RCTL_MPE | RCTL_LBM_NONE | RTCL_RDMTS_HALF | RCTL_BAM | RCTL_SECRC  | RCTL_BSIZE_8192);
}

void e1000_driver::tx_init() {
	struct e1000_tx_desc* descs = (struct e1000_tx_desc*) this->tx_ptr;
	for (int i = 0; i < e1000_NUM_TX_DESC; i++) {
		this->tx_descs[i] = (struct e1000_tx_desc*) ((uint8_t*)descs + i * 16);
		this->tx_descs[i]->addr = 0;
		this->tx_descs[i]->cmd = 0;
		this->tx_descs[i]->status = TSTA_DD;
	}

	write_command(REG_TXDESCLO, (uint64_t) this->tx_ptr);
	write_command(REG_TXDESCHI, 0);

	write_command(REG_TXDESCLEN, e1000_NUM_TX_DESC * 16);

	write_command(REG_TXDESCHEAD, 0);
	write_command(REG_TXDESCTAIL, 0);
	this->tx_cur = 0;

	write_command(REG_TCTRL, 0b0110000000000111111000011111010);
	write_command(REG_TIPG, 0x0060200A);
}

void e1000_driver::start_link() {
	uint32_t val = read_command(REG_CTRL);
	write_command(REG_CTRL, val | ECTRL_SLU);
}

void e1000_driver::activate() {
	if (this->bar_type == 0) {
		if (!(this->mem_base)) {
			debugf("e1000_driver: an error occurred when getting the device's MMIO base address. Aborting setup.\n");
			return;
		}
	} else {
		if (!(this->io_port)) {
			debugf("e1000_driver: an error occurred when getting the device's IO port. Aborting setup.\n");
			return;
		}
	}

	detect_eeprom();
	read_mac_address();
	debugf("e1000_driver: MAC: %x:%x:%x:%x:%x:%x\n", this->mac[0], this->mac[1], this->mac[2], this->mac[3], this->mac[4], this->mac[5]);

	start_link();
	for(int i = 0; i < 0x80; i++) {
		write_command(0x5200 + i * 4, 0);
	}

	//Enable interrupts
	write_command(REG_IMASK, 0x1F6DC);
	write_command(REG_IMASK, 0xff & ~4);
	read_command(0xc0);

	//Enable receiving and transmitting
	rx_init();
	tx_init();

	debugf("e1000_driver: successfully activated!\n");

	this->set_ip(0);

	driver::global_nic_manager->add_nic(this);
}

void e1000_driver::receive() {
	bool got_packet = false;

	while(this->rx_descs[this->rx_cur]->status & 0x1) {
		got_packet = true;
		uint8_t* data = (uint8_t *) this->rx_descs[this->rx_cur]->addr;
		uint16_t size = this->rx_descs[this->rx_cur]->length;

#ifdef DEBUG
		debugf("e1000_driver: receiveing packet: ");
		for(int i = 0; i < (size > 64 ? 64 : size); i++) {
			debugf("%x ", data[i]);
		}
		debugf("\n");
#endif
 
		if (_nic_data_manager) {
			_nic_data_manager->recv(data, size);
		} else {
			debugf("e1000_driver: no handler\n");
		}
 
		this->rx_descs[this->rx_cur]->status = 0;
		uint16_t old_cur = this->rx_cur;
		this->rx_cur = (this->rx_cur + 1) % e1000_NUM_RX_DESC;
		write_command(REG_RXDESCTAIL, old_cur);
	}
}

void e1000_driver::send(uint8_t* data, int32_t length) {
	this->tx_descs[this->tx_cur]->addr = (uint64_t) data;
	this->tx_descs[this->tx_cur]->length = length;
	this->tx_descs[this->tx_cur]->cmd = CMD_EOP | CMD_IFCS | CMD_RS;
	this->tx_descs[this->tx_cur]->status = 0;

#ifdef DEBUG
	debugf("e1000_driver: sending packet: ");
	for(int i = 0; i < (length > 64 ? 64 : length); i++) {
		debugf("%x ", data[i]);
	}
	debugf("\n");
#endif

	uint8_t old_cur = this->tx_cur;
	this->tx_cur = (this->tx_cur + 1) % e1000_NUM_TX_DESC;
	write_command(REG_TXDESCTAIL, this->tx_cur);

	while(!(this->tx_descs[old_cur]->status & 0xff));
}

void e1000_driver::handle() {
	write_command(REG_IMASK, 0x1);

	uint32_t status = read_command(0xC0);
	if (status & 0x04) {
		start_link();
	} else if(status & 0x10) {
#ifdef DEBUG
		debugf("e1000_driver: Good threshold\n");
#endif
	} else if(status & 0x80) {
		receive();
	}
}

char* e1000_driver::get_name() {
	return (char*) "e1000";
}

uint64_t e1000_driver::get_mac() {
	driver::mac_u m;
	m.mac_p[0] = this->mac[0];
	m.mac_p[1] = this->mac[1];
	m.mac_p[2] = this->mac[2];
	m.mac_p[3] = this->mac[3];
	m.mac_p[4] = this->mac[4];
	m.mac_p[5] = this->mac[5];

	return m.mac;
}

uint32_t e1000_driver::get_ip() {
	return this->logicalAddress;
}

void e1000_driver::set_ip(uint32_t ip) {
	this->logicalAddress = ip;
}