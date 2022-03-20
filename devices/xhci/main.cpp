#include <elf/kernel_module.h>

#include <pci/pci.h>
#include <driver/driver.h>
#include <utils/log.h>
#include <utils/abort.h>

#include <xhci.h>

void xhci_found(pci::pci_header_0_t* header, uint16_t bus, uint16_t device, uint16_t function) {
	debugf("XHCI found on bus %d, device %d, function %d\n", bus, device, function);
	driver::global_driver_manager->add_driver(new xhci::xhci_driver(header, bus, device, function));
}


void init() {
	pci::register_pci_driver(0xc, 0x3, 0x30, xhci_found);
}

define_module("xhci", init, null_ptr_func, null_ptr_func);