#include <xhci.h>

#include <utils/log.h>
#include <utils/assert.h>
#include <memory/page_table_manager.h>
#include <memory/page_frame_allocator.h>

#include <timer/timer.h>

using namespace xhci;

template<typename F>
bool xhci_timeout(uint64_t ms, F f) {
	while(!f()) {
		ms--;
		if(ms == 0) {
			return false;
		}

		timer::global_timer->sleep(1);
	}

	return true;
}

xhci_driver::xhci_driver(pci::pci_header_0_t* header, uint16_t bus, uint16_t device, uint16_t function) : interrupt_handler(header->interrupt_line + 0x20), protocols(10) {
	this->header = header;
	this->bus = bus;
	this->device = device;
	this->function = function;
}


void xhci_driver::handle() {
	debugf("XHCI driver handle\n");
	trace();
}

void xhci_driver::activate() {
	pci::enable_mmio(bus, device, function);
	pci::become_bus_master(bus, device, function);

	uint64_t addr_low  = header->BAR0 & 0xFFFFFFF0;
	uint64_t addr_high = header->BAR1 & 0xFFFFFFFF; /* I think this is right? */
	uint64_t mmio_addr = (addr_high << 32) | addr_low;

	for (int i = 0; i < 4; i++) {
		uint64_t addr = mmio_addr + (i * 0x1000);
		memory::global_page_table_manager.map_memory((void*) addr, (void*) addr);
		memory::global_allocator.lock_page((void*) addr);
	}

	debugf("mmio_address %x\n", mmio_addr);

	capabilites = (capabilites_regs*) mmio_addr;
	operational = (operational_regs*) (mmio_addr + (capabilites->caplength & 0xff));
	runtime = (runtime_regs*) (mmio_addr + (capabilites->rtsoff & ~0x1f));
	db = (uint32_t*) (mmio_addr + (capabilites->dboff & ~0x3));

	debugf("hci_version %x\n", capabilites->hci_version);

	if (header->header.vendor_id == 0x8086) {
		is_intel == true;
		debugf("Intel xHCI\n");
		intel_enable_ports();
	} else {
		is_intel == false;
	}

	pci::pci_writeb(bus, device, function, 0x61, 0x20); // write to fladj

	debugf("xhci: Detected HCI %d,%d,%d\n", (capabilites->hci_version >> 8) & 0xFF, (capabilites->hci_version >> 4) & 0xF, capabilites->hci_version & 0xF);

	n_slots = capabilites->hcsparams1 & 0xff;
	n_interrupters = (capabilites->hcsparams1 >> 8) & 0x7ff;
	n_ports = (capabilites->hcsparams1 >> 24) & 0xff;

	debugf("n_slots %d\n", n_slots);
	debugf("n_interrupters %d\n", n_interrupters);
	debugf("n_ports %d\n", n_ports);

	uint8_t bit = 0;
	for(uint8_t i = 0; i < 16; i++) {
		if(operational->pagesize & (1 << i)) {
			bit = i;
			break;
		}
	}

	page_size = 1 << (bit + 12);

	debugf("page_size %d\n", page_size);

	uint16_t off = (capabilites->hccparams1 >> 16) & 0xFFFF;
	ext_caps = (volatile uint32_t*)(mmio_addr + (off * 4));

	int i = 0;
	while(true) {
		uint32_t id = ext_caps[i] & 0xFF;
		uint32_t off = (ext_caps[i] >> 8) & 0xFF;

		if(id == 1) {
			// BIOS-OS handoff was already done
		} else if(id == 2) {
			volatile protocol_cap* item = (volatile protocol_cap*) &ext_caps[i];

			protocol proto;

			proto.major = (item->version >> 24) & 0xFF;
			proto.minor = (item->version >> 16) & 0xFF;

			proto.port_count = (item->ports_info >> 8) & 0xFF;
			proto.port_off = (item->ports_info & 0xFF) - 1;

			proto.slot_type = item->slot_type & 0x1F;

			protocols.add(proto);

			debugf("Protocol: %c%c%c%c%d.%d\n", (char)item->name, (char)(item->name >> 8), (char)(item->name >> 16), (char)(item->name >> 24), (uint16_t)proto.major, (uint16_t)proto.minor);
		} else if(id >= 192) {
			// Vendor Specific, so ignore
		} else {
			debugf("Unhandled Extended Cap with id %d\n", id);
		}

		if (off == 0) {
			break;
		}

		i += off;
	}

	reset_controller();
}

char* xhci_driver::get_name() {
	return "xhci";
}

void xhci_driver::intel_enable_ports() {
	uint16_t subsys_vid = pci::pci_readw(bus, device, function, 0x2C);
	uint16_t subsys_did = pci::pci_readw(bus, device, function, 0x2E);

	debugf("subsys_vid %x\n", subsys_vid);
	debugf("subsys_did %x\n", subsys_did);

	if (subsys_vid == 0x104D && subsys_did == 0x90A8) {
		return;
	}

	pci::pci_writed(bus, device, function, 0xd8, pci::pci_readd(bus, device, function, 0xdc));
	pci::pci_readd(bus, device, function, 0xd8);

	pci::pci_writed(bus, device, function, 0xd0, pci::pci_readd(bus, device, function, 0xd4));
	pci::pci_readd(bus, device, function, 0xd0);
}

void xhci_driver::reset_controller() {
	debugf("Starting controller reset...\n");

	operational->usbcmd &= ~(usbcmd::run & usbcmd::irq_enable); // Pls stop Mr. controller

	if (is_intel) {
		timer::global_timer->sleep(100);
	}

	assert(xhci_timeout(20, [&] {
		return (operational->usbsts & usbsts::halted) != 0;
	}));

	operational->usbcmd |= usbcmd::reset;

	while(operational->usbcmd & usbcmd::reset || operational->usbsts & usbsts::not_ready) {
		__asm__ __volatile__ ("pause" ::: "memory");
	}

	timer::global_timer->sleep(100);

	debugf("Finished controller reset!\n");
}