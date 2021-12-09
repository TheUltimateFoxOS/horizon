#include <ahci.h>

#include <memory/page_table_manager.h>
#include <memory/page_frame_allocator.h>

#include <utils/log.h>

using namespace ahci;

ahci_driver::ahci_driver(pci::pci_header_0_t* pci_header) {
	this->pci_header = pci_header;

	ABAR = (HBA_memory*)(uint64_t)((pci::pci_header_0_t*) pci_header)->BAR5;
	memory::global_page_table_manager.map_memory(ABAR, ABAR);

	this->port_count = 0;
}

char* ahci_driver::get_name() {
	return (char*) "ahci";
}

port_type_t check_port_type(HBA_port* port) {
	uint32_t sataStatus = port->sata_status;

	uint8_t interfacePowerManagement = (sataStatus >> 8) & 0b111;
	uint8_t deviceDetection = sataStatus & 0b111;

	if (deviceDetection != HBA_PORT_DEV_PRESENT) return port_type_t::None;
	if (interfacePowerManagement != HBA_PORT_IPM_ACTIVE) return port_type_t::None;

	switch (port->signature){
		case SATA_SIG_ATAPI:
			return port_type_t::SATAPI;
		case SATA_SIG_ATA:
			return port_type_t::SATA;
		case SATA_SIG_PM:
			return port_type_t::PM;
		case SATA_SIG_SEMB:
			return port_type_t::SEMB;
		default:
			return port_type_t::None;
	}
}

void ahci_driver::probe_ports() {
	uint32_t portsImplemented = ABAR->ports_implemented;
	for (int i = 0; i < 32; i++){
		if (portsImplemented & (1 << i)) {
			port_type_t port_type = check_port_type(&ABAR->ports[i]);

			if (port_type == port_type_t::SATA || port_type == port_type_t::SATAPI) {
				debugf("Found SATA/SATAPI port %d\n", i);
				ports[port_count] = new ahci_port();
				ports[port_count]->port_type = port_type;
				ports[port_count]->hba_port = &ABAR->ports[i];
				ports[port_count]->port_number = port_count;
				port_count++;
			}
		}
	}
}


void ahci_driver::activate() {
	probe_ports();

	for (int i = 0; i < port_count; i++) {
		ahci_port* port = ports[i];

		port->configure();

		port->buffer = (uint8_t*) memory::global_allocator.request_page();
		memset(port->buffer, 0, 0x1000);
	}
}

bool ahci_driver::is_presend() {
	return true;
}