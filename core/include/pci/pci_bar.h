#pragma once

#include <stdint.h>
#include <pci/pci.h>

namespace pci {
	enum pci_bar_type_t {
		NONE = 0,
		MMIO64,
		MMIO32,
		IO
	};

	struct pci_bar_t {
		uint64_t mem_address;
		uint16_t io_address;
		pci_bar_type_t type;
		uint16_t size;
	};

	pci_bar_t get_bar(uint32_t* bar0, int bar_num, uint16_t bus, uint16_t device, uint16_t function);
	uint16_t get_io_port(pci::pci_header_0_t* header, uint16_t bus, uint16_t device, uint16_t function);
}