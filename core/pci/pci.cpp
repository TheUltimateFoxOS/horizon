#include <pci/pci.h>

#include <utils/log.h>
#include <memory/page_table_manager.h>

using namespace pci;

namespace pci {
	list<pci_driver>* pci_drivers = nullptr;
}

void enumerate_function(uint64_t address, uint64_t function, uint16_t bus, uint16_t device) {
	uint64_t offset = function << 12;

	uint64_t function_address = address + offset;

	memory::global_page_table_manager.map_memory((void*) function_address, (void*) function_address);

	pci_device_header_t* pci_device_header = (pci_device_header_t*) function_address;

	if(pci_device_header->device_id == 0) {
		return;
	}
	if(pci_device_header->device_id == 0xffff) {
		return;
	}

	debugf("Vendor name: %s, ", get_vendor_name(pci_device_header->vendor_id));
	debugf("Device name: %s, ", get_device_name(pci_device_header->vendor_id, pci_device_header->device_id));
	debugf("Device class: %s, ", device_classes[pci_device_header->class_]);
	debugf("Sub class name: %s, ", get_subclass_name(pci_device_header->class_, pci_device_header->subclass));
	debugf("Prog interface name: %s\n", get_prog_IF_name(pci_device_header->class_, pci_device_header->subclass, pci_device_header->prog_if));

	if (pci_drivers != nullptr) {
		debugf("Searching for pci driver...\n");
		list<pci_driver>::node* driver = pci_drivers->find<pci_header_0_t*>([](pci_header_0_t* header, list<pci_driver>::node* node) {
			if (node->data.use_class_subclass_prog_IF) {
				return (header->header.class_ == node->data._class && header->header.subclass == node->data.subclass && header->header.prog_if == node->data.prog_IF);
			} else if (node->data.use_vendor_device_id) {
				return (header->header.vendor_id == node->data.vendor_id && header->header.device_id == node->data.device_id);
			} else {
				debugf("Error: Driver does not use vendor/device id or class/subclass/prog_IF witch is invalid\n");
				return false;
			}
		}, (pci_header_0_t*) pci_device_header);

		if (driver != nullptr) {
			debugf("Found driver. Init function: 0x%x\n", driver->data.load_driver);
			driver->data.load_driver((pci::pci_header_0_t*) pci_device_header);
		} else {
			debugf("No driver found\n");
		}
	}
}

void enumerate_device(uint64_t bus_address, uint64_t device, uint16_t bus) {
	uint64_t offset = device << 15;

	uint64_t device_address = bus_address + offset;
	memory::global_page_table_manager.map_memory((void*) device_address, (void*) device_address);

	pci_device_header_t* pci_device_header = (pci_device_header_t*) device_address;

	if(pci_device_header->device_id == 0) {
		return;
	}
	if(pci_device_header->device_id == 0xffff) {
		return;
	} 

	for (uint64_t function = 0; function < 8; function++){
		enumerate_function(device_address, function, bus, device);
	}
}

void enumerate_bus(uint64_t base_address, uint64_t bus) {
	uint64_t offset = bus << 20;

	uint64_t bus_address = base_address + offset;
	memory::global_page_table_manager.map_memory((void*) bus_address, (void*) bus_address);

	pci_device_header_t* pci_device_header = (pci_device_header_t*) bus_address;

	if(pci_device_header->device_id == 0) {
		return;
	}
	if(pci_device_header->device_id == 0xffff) {
		return;
	} 

	for (uint64_t device = 0; device < 32; device++){
		enumerate_device(bus_address, device, bus);
	}
}

void enumerate_mcfg(acpi::mcfg_header_t* mcfg) {
	int entries = ((mcfg->header.length) - sizeof(acpi::mcfg_header_t)) / sizeof(acpi::mcfg_device_config_t);

	for(int t = 0; t < entries; t++) {
		acpi::mcfg_device_config_t* new_device_config = (acpi::mcfg_device_config_t*) ((uint64_t) mcfg + sizeof(acpi::mcfg_header_t) + (sizeof(acpi::mcfg_device_config_t) * t));
		for (uint64_t bus = new_device_config->start_bus; bus < new_device_config->end_bus; bus++){
			enumerate_bus(new_device_config->base_address, bus);
		}
	}
}

void enumerate_legacy() {
	for (uint16_t bus = 0; bus < 8; bus++) {
		for (uint16_t device = 0; device < 32; device++) {
			int num_functions = device_has_functions(bus, device) ? 8 : 1;
			for (uint64_t function = 0; function < num_functions; function++) {
				pci::pci_header_0_t pci_header = get_device_header(bus, device, function);
				if(pci_header.header.vendor_id == 0x0000 || pci_header.header.vendor_id == 0xffff) {
					continue;
				}

				pci_device_header_t* pci_device_header = &pci_header.header;

				debugf("Vendor name: %s, ", get_vendor_name(pci_device_header->vendor_id));
				debugf("Device name: %s, ", get_device_name(pci_device_header->vendor_id, pci_device_header->device_id));
				debugf("Device class: %s, ", device_classes[pci_device_header->class_]);
				debugf("Sub class name: %s, ", get_subclass_name(pci_device_header->class_, pci_device_header->subclass));
				debugf("Prog interface name: %s\n", get_prog_IF_name(pci_device_header->class_, pci_device_header->subclass, pci_device_header->prog_if));

				pci::pci_header_0_t* header_copy = new pci::pci_header_0_t;
				memcpy(header_copy, &pci_header, sizeof(pci::pci_header_0_t));

				if (pci_drivers != nullptr) {
					debugf("Searching for pci driver...\n");
					list<pci_driver>::node* driver = pci_drivers->find<pci_header_0_t*>([](pci_header_0_t* header, list<pci_driver>::node* node) {
						if (node->data.use_class_subclass_prog_IF) {
							return (header->header.class_ == node->data._class && header->header.subclass == node->data.subclass && header->header.prog_if == node->data.prog_IF);
						} else if (node->data.use_vendor_device_id) {
							return (header->header.vendor_id == node->data.vendor_id && header->header.device_id == node->data.device_id);
						} else {
							debugf("Error: Driver does not use vendor/device id or class/subclass/prog_IF witch is invalid\n");
							return false;
						}
					}, header_copy);

					if (driver != nullptr) {
						debugf("Found driver. Init function: 0x%x\n", driver->data.load_driver);
						driver->data.load_driver(header_copy);
					} else {
						debugf("No driver found\n");
					}
				}
			}
		}
	}
}

void pci::enumerate_pci() {
	acpi::mcfg_header_t* mcfg_table = (acpi::mcfg_header_t*) acpi::find_table(global_bootinfo, (char*) "MCFG", 0);

	if (mcfg_table == NULL) {
		debugf("No mcfg found! Using PCI Configuration Space Access Mechanism!\n");
		enumerate_legacy();
	} else {
		debugf("Found MCFG!\n");
		enumerate_mcfg(mcfg_table);
	}
}

void pci::register_pci_driver(uint8_t _class, uint8_t subclass, uint8_t prog_IF, void (*load_driver)(pci_header_0_t* header)) {
	debugf("Registering pci driver for class: %d, subclass: %d, prog_IF: %d\n", _class, subclass, prog_IF);
	pci_driver driver = {
		._class = _class,
		.subclass = subclass,
		.prog_IF = prog_IF,
		.use_class_subclass_prog_IF = true,
		.load_driver = load_driver
	};

	if (pci_drivers == nullptr) {
		pci_drivers = new list<pci_driver>(10);
	}

	pci_drivers->add(driver);
}

void pci::register_pci_driver(uint16_t vendor_id, uint16_t device_id, void (*load_driver)(pci_header_0_t* header)) {
	debugf("Registering pci driver for vendor: 0x%x, device: 0x%x\n", vendor_id, device_id);
	pci_driver driver = {
		.vendor_id = vendor_id,
		.device_id = device_id,
		.use_vendor_device_id = true,
		.load_driver = load_driver
	};

	if (pci_drivers == nullptr) {
		pci_drivers = new list<pci_driver>(10);
	}

	pci_drivers->add(driver);
}