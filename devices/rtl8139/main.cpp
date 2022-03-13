#include <elf/kernel_module.h>

#include <pci/pci.h>
#include <utils/log.h>

#include <rtl8139.h>

#include <driver/driver.h>

void rtl8139_found(pci::pci_header_0_t* header, uint16_t bus, uint16_t device, uint16_t function) {
	driver::global_driver_manager->add_driver(new driver::rtl8139_driver(header, bus, device, function));
}

void init() {
	pci::register_pci_driver(0x10EC, 0x8139, rtl8139_found);
}

define_module("rtl8139", init, null_ptr_func, null_ptr_func);