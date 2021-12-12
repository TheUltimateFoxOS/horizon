#include <elf/kernel_module.h>

#include <utils/log.h>
#include <pci/pci.h>
#include <driver/driver.h>

#include <ahci.h>

void ahci_pci_device_found(pci::pci_header_0_t* header, uint16_t bus, uint16_t device, uint16_t function) {
	driver::global_driver_manager->add_driver(new ahci::ahci_driver(header));
}

void init() {
	pci::register_pci_driver(0x1, 0x6, 0x1, ahci_pci_device_found);
}

define_module("ahci", init, null_ptr_func, null_ptr_func);