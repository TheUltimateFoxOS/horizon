#include <pci/pci.h>

#include <utils/port.h>

uint32_t pci::pci_readd(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset) {
	uint32_t id = 0x1 << 31 | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) | ((function & 0x07) << 8) | (registeroffset & 0xFC);
	Port32Bit command_port = Port32Bit(0xcf8);
	Port32Bit data_port = Port32Bit(0xcfc);
	command_port.Write(id);
	uint32_t result = data_port.Read();
	return result >> (8 * (registeroffset % 4));
}

void pci::pci_writed(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset, uint32_t value) {
	uint32_t id = 0x1 << 31 | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) | ((function & 0x07) << 8) | (registeroffset & 0xFC);
	Port32Bit command_port = Port32Bit(0xcf8);
	Port32Bit data_port = Port32Bit(0xcfc);
	command_port.Write(id);
	data_port.Write(value);
}

void pci::pci_writeb(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset, uint8_t value) {
	uint32_t id = 0x1 << 31 | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) | ((function & 0x07) << 8) | (registeroffset & 0xFC);
	Port32Bit command_port = Port32Bit(0xcf8);
	Port8Bit data_port = Port8Bit(0xcfc);
	command_port.Write(id);
	data_port.Write(value);
}

uint8_t pci::pci_readb(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset) {
	uint32_t id = 0x1 << 31 | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) | ((function & 0x07) << 8) | (registeroffset & 0xFC);
	Port32Bit command_port = Port32Bit(0xcf8);
	Port8Bit data_port = Port8Bit(0xcfc);
	command_port.Write(id);
	uint8_t result = data_port.Read();
	return result >> (8 * (registeroffset % 4));
}

void pci::pci_writew(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset, uint16_t value) {
	uint32_t id = 0x1 << 31 | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) | ((function & 0x07) << 8) | (registeroffset & 0xFC);
	Port32Bit command_port = Port32Bit(0xcf8);
	Port16Bit data_port = Port16Bit(0xcfc);
	command_port.Write(id);
	data_port.Write(value);
}

uint16_t pci::pci_readw(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset) {
	uint32_t id = 0x1 << 31 | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) | ((function & 0x07) << 8) | (registeroffset & 0xFC);
	Port32Bit command_port = Port32Bit(0xcf8);
	Port16Bit data_port = Port16Bit(0xcfc);
	command_port.Write(id);
	uint16_t result = data_port.Read();
	return result >> (8 * (registeroffset % 4));
}

int pci::device_has_functions(uint16_t bus, uint16_t device) {
	return pci_readd(bus, device, 0, 0xe) & (1 << 7);
}

pci::pci_header_0_t pci::get_device_header(uint16_t bus, uint16_t device, uint16_t function) {
	pci::pci_header_0_t result;
	result.header.vendor_id = pci_readd(bus, device, function, 0);
	result.header.device_id = pci_readd(bus, device, function, 2);
	result.header.command = pci_readd(bus, device, function, 4);
	result.header.status = pci_readd(bus, device, function, 6);
	result.header.revision_id = pci_readd(bus, device, function, 8);
	result.header.prog_if = pci_readd(bus, device, function, 9);
	result.header.subclass = pci_readd(bus, device, function, 0xa);
	result.header.class_ = pci_readd(bus, device, function, 0xb);
	result.header.cache_line_size = pci_readd(bus, device, function, 16);
	result.header.latency_timer = pci_readd(bus, device, function, 18);
	result.header.header_type = pci_readd(bus, device, function, 20);
	result.header.bist = pci_readd(bus, device, function, 22);
	result.BAR0 = pci_readd(bus, device, function, 0x10);
	result.BAR1 = pci_readd(bus, device, function, 0x14);
	result.BAR2 = pci_readd(bus, device, function, 0x18);
	result.BAR3 = pci_readd(bus, device, function, 0x1c);
	result.BAR4 = pci_readd(bus, device, function, 0x20);
	result.BAR5 = pci_readd(bus, device, function, 0x24);
	result.cardbus_CIS_ptr = pci_readd(bus, device, function, 0x2c);
	result.subsystem_vendor_ID = pci_readd(bus, device, function, 0x2e);
	result.subsystem_ID = pci_readd(bus, device, function, 0x2f);
	result.expansion_ROM_base_addr = pci_readd(bus, device, function, 0x30);
	result.interrupt_line = pci_readd(bus, device, function, 0x3c);
	result.interrupt_pin = pci_readd(bus, device, function, 0x3d);
	result.min_grant = pci_readd(bus, device, function, 0x3e);
	result.max_latency = pci_readd(bus, device, function, 0x3f);
	return result;
}

void pci::enable_mmio(uint16_t bus, uint16_t device, uint16_t function) {
	pci_writed(bus, device, function, 0x4, pci_readd(bus, device, function, 0x4) | (1 << 1));
}

void pci::become_bus_master(uint16_t bus, uint16_t device, uint16_t function) {
	pci_writed(bus, device, function, 0x4, pci_readd(bus, device, function, 0x4) | (1 << 2));
}