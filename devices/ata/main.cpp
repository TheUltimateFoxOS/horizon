#include <elf/kernel_module.h>

#include <pci/pci.h>
#include <driver/driver.h>

#include <utils/log.h>

#include <ata.h>

void ata_pci_device_found(pci::pci_header_0_t* header) {
	driver::global_driver_manager->add_driver(new driver::advanced_technology_attachment(true, 0x1F0, (char*) "ata0 master"));
	driver::global_driver_manager->add_driver(new driver::advanced_technology_attachment(false, 0x1F0, (char*) "ata0 slave"));
	driver::global_driver_manager->add_driver(new driver::advanced_technology_attachment(true, 0x170, (char*) "ata1 master"));
	driver::global_driver_manager->add_driver(new driver::advanced_technology_attachment(false, 0x170, (char*) "ata1 slave"));
}

void init() {
	pci::register_pci_driver(0x1, 0x1, 0x0, ata_pci_device_found);
}

define_module("ata", init, null_ptr_func, null_ptr_func);