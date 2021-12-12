#include <elf/kernel_module.h>

#include <pci/pci.h>
#include <utils/log.h>

#include <e1000.h>

#include <driver/driver.h>

void e1000_found(pci::pci_header_0_t* header, uint16_t bus, uint16_t device, uint16_t function) {
	driver::global_driver_manager->add_driver(new driver::e1000_driver(header, bus, device, function));
}

void init() {
	pci::register_pci_driver(0x8086, 0x100E, e1000_found);
}

define_module("e1000", init, null_ptr_func, null_ptr_func);