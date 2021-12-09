#include <elf/kernel_module.h>

#include <utils/log.h>
#include <pci/pci.h>
#include <driver/driver.h>

#include <ahci.h>

void ahci_pci_device_found(pci::pci_header_0_t* header) {
	driver::global_driver_manager->add_driver(new ahci::ahci_driver(header));
}

void init() {
	pci::register_pci_driver(0x1, 0x6, 0x1, ahci_pci_device_found);
}

void device_init() {}

void fs_init() {}

define_module("ahci", init, device_init, fs_init);