#include <elf/kernel_module.h>

#include <pci/pci.h>
#include <utils/log.h>

#include <am79C973.h>

#include <driver/driver.h>

void am79C973_found(pci::pci_header_0_t* header, uint16_t bus, uint16_t device, uint16_t function) {
	driver::global_driver_manager->add_driver(new driver::am79C973_driver(header, bus, device, function));
}

void init() {
	pci::register_pci_driver(0x1022, 0x2000, am79C973_found);
}

define_module("am79C973", init, null_ptr_func, null_ptr_func);