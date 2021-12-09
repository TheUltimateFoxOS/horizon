#pragma once

#pragma once

#include <stdint.h>
#include <acpi/acpi.h>
#include <utils/list.h>

namespace pci {
	struct pci_device_header_t {
		uint16_t vendor_id;
		uint16_t device_id;
		uint16_t command;
		uint16_t status;
		uint8_t revision_id;
		uint8_t prog_if;
		uint8_t subclass;
		uint8_t class_;
		uint8_t cache_line_size;
		uint8_t latency_timer;
		uint8_t header_type;
		uint8_t bist;
	};

	struct pci_header_0_t {
		pci_device_header_t header;
		uint32_t BAR0;
		uint32_t BAR1;
		uint32_t BAR2;
		uint32_t BAR3;
		uint32_t BAR4;
		uint32_t BAR5;
		uint32_t cardbus_CIS_ptr;
		uint16_t subsystem_vendor_ID;
		uint16_t subsystem_ID;
		uint32_t expansion_ROM_base_addr;
		uint8_t capabilities_ptr;
		uint8_t rsv0;
		uint16_t rsv1;
		uint32_t rsv2;
		uint8_t interrupt_line;
		uint8_t interrupt_pin;
		uint8_t min_grant;
		uint8_t max_latency;
	};

	struct pci_device {
		pci_header_0_t* header;
		uint16_t bus;
		uint16_t device;
		uint64_t function;
	};


	extern const char* device_classes[];

	const char* get_vendor_name(uint16_t vendor_ID);
	const char* get_device_name(uint16_t vendor_ID, uint16_t device_ID);
	const char* get_subclass_name(uint8_t class_code, uint8_t subclass_code);
	const char* get_prog_IF_name(uint8_t class_code, uint8_t subclass_code, uint8_t prog_IF);

	uint32_t pci_readd(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset);
	void pci_writed(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset, uint32_t value);
	
	uint8_t pci_readb(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset);
	void pci_writeb(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset, uint8_t value);
	
	uint16_t pci_readw(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset);
	void pci_writew(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset, uint16_t value);

	int device_has_functions(uint16_t bus, uint16_t device);
	pci_header_0_t get_device_header(uint16_t bus, uint16_t device, uint16_t function);

	void enumerate_pci();

	void enable_mmio(uint16_t bus, uint16_t device, uint16_t function);
	void become_bus_master(uint16_t bus, uint16_t device, uint16_t function);

	struct pci_driver {
		uint8_t _class;
		uint8_t subclass;
		uint8_t prog_IF;
		bool use_class_subclass_prog_IF;

		uint16_t vendor_id;
		uint16_t device_id;
		bool use_vendor_device_id;

		void (*load_driver)(pci_header_0_t* header);
	};

	void register_pci_driver(uint8_t _class, uint8_t subclass, uint8_t prog_IF, void (*load_driver)(pci_header_0_t* header));
	void register_pci_driver(uint16_t vendor_id, uint16_t device_id, void (*load_driver)(pci_header_0_t* header));

	extern list<pci_driver>* pci_drivers;
}