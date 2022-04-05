#include <acpi/acpi.h>

#include <utils/string.h>
#include <utils/log.h>

#include <utils/port.h>
#include <utils/abort.h>

#include <pci/pci.h>

#include <timer/timer.h>

#include <memory/page_table_manager.h>

using namespace acpi;

void* acpi::find_table_xsdt(sdt_header_t* sdt_header, char* signature, int idx) {
	int entries = (sdt_header->length - sizeof(sdt_header_t)) / 8;

	for(int t = 0; t < entries; t++) {
		sdt_header_t* new_header = (sdt_header_t*) * (uint64_t*) ((uint64_t) sdt_header + sizeof(sdt_header_t) + (t * 8));
		if (memcmp(new_header->signature, signature, 4) == 0) {
			if (idx == 0) {
				return new_header;
			}
			idx--;
		}
	}

	return 0;
}

void* acpi::find_table_rsdt(sdt_header_t* sdt_header, char* signature, int idx) {
	int entries = (sdt_header->length - sizeof(sdt_header_t)) / 4;

	for(int t = 0; t < entries; t++) {
		sdt_header_t* new_header = (sdt_header_t*) (uint64_t) * (uint32_t*) ((uint64_t) sdt_header + sizeof(sdt_header_t) + (t * 4));
		if (memcmp(new_header->signature, signature, 4) == 0) {
			if (idx == 0) {
				return new_header;
			}
			idx--;
		}
	}

	return 0;
}

void* acpi::find_table(stivale2_struct* bootinfo, char* signature, int idx) {


	void* result = nullptr;

	stivale2_struct_tag_rsdp* rsdp_tag = stivale2_tag_find<stivale2_struct_tag_rsdp>(bootinfo, STIVALE2_STRUCT_TAG_RSDP_ID);

	rsdp2_t* rsdp = (rsdp2_t*) ((uint64_t) rsdp_tag->rsdp);

	if (rsdp->xsdt_address != 0) {
		sdt_header_t* xsdt = (sdt_header_t*) (((rsdp2_t*) rsdp_tag->rsdp)->xsdt_address);

		result = find_table_xsdt(xsdt, (char*) signature, idx);
	} else {
		sdt_header_t* rsdt = (sdt_header_t*) (uint64_t) (((rsdp2_t*) rsdp_tag->rsdp)->rsdt_address);
			
		result = find_table_rsdt(rsdt, (char*) signature, idx);
	}

	debugf("DEBUG: search result for %s:%d is %x\n", signature, idx, result);

	return result;
}

void acpi::init() {
	debugf("ACPI init...\n");

	fadt_table_t* fadt = (fadt_table_t*) find_table(global_bootinfo, (char*) "FACP", 0);

	debugf("Sending acpi enable command...\n");
	outb(fadt->smi_command_port, fadt->acpi_enable);
	while (!(inw(fadt->PM1a_control_block) & 1)) {
		debugf("Waiting for acpi enable...\n");
		__asm__ __volatile__("pause");
	}

	dsdt_init();

	outb(fadt->smi_command_port, fadt->acpi_enable);
	while (inw(fadt->PM1a_control_block) & 1 == 0) {
		__asm__ __volatile__("pause" ::: "memory");
	}
}

uint16_t SLP_TYPa;
uint16_t SLP_TYPb;

void acpi::dsdt_init() {
	debugf("ACPI dsdt init...\n");
	fadt_table_t* fadt = (fadt_table_t*) find_table(global_bootinfo, (char*) "FACP", 0);

	uint64_t dsdt_addr = IS_CANONICAL(fadt->X_dsdt) ? fadt->X_dsdt : fadt->dsdt;

	uint8_t *s5_addr = (uint8_t*) dsdt_addr + 36;
	uint64_t dsdt_length = ((sdt_header_t*) dsdt_addr)->length;

	dsdt_addr *= 2;
	while (dsdt_length-- > 0) {
		if (!memcmp(s5_addr, "_S5_", 4)) break;
		s5_addr++;
	}

	if (dsdt_length <= 0){
		debugf("_S5 not present in ACPI");
		return;
	}

	if ((*(s5_addr - 1) == 0x08 || (*(s5_addr - 2) == 0x08 && *(s5_addr - 1) == '\\')) && *(s5_addr + 4) == 0x12) {
		s5_addr += 5;
		s5_addr += ((*s5_addr & 0xC0) >> 6) + 2;

		if (*s5_addr == 0x0A) {
			s5_addr++;
		}

		SLP_TYPa = *(s5_addr) << 10;
		s5_addr++;

		if (*s5_addr == 0x0A) {
			s5_addr++;
		}

		SLP_TYPb = *(s5_addr) << 10;

		return;
	}

	debugf("Failed to parse _S5 in ACPI");
}

void acpi::shutdown() {
	debugf("ACPI shutdown...\n");
	fadt_table_t* fadt = (fadt_table_t*) find_table(global_bootinfo, (char*) "FACP", 0);

	outw(fadt->PM1a_control_block, (inw(fadt->PM1a_control_block) & 0xE3FF) | ((SLP_TYPa << 10) | 0x2000));

	if (fadt->PM1b_control_block) {
		outw(fadt->PM1b_control_block, (inw(fadt->PM1b_control_block) & 0xE3FF) | ((SLP_TYPb << 10) | 0x2000));
	}

	outw(fadt->PM1a_control_block, SLP_TYPa | (1 << 13));
	if (fadt->PM1b_control_block) {
		outw(fadt->PM1b_control_block, SLP_TYPb | (1 << 13));
	}

	timer::global_timer->sleep(100);

	abortf("ACPI shutdown failed");
}

void acpi::reboot() {
	debugf("ACPI reboot...\n");
	fadt_table_t* fadt = (fadt_table_t*) find_table(global_bootinfo, (char*) "FACP", 0);

	switch (fadt->reset_reg.address_space ) {
		case GENERIC_ADDRESS_SPACE_SYSTEM_IO:
			{
				debugf("ACPI reboot: system io\n");
				outb(fadt->reset_reg.address, fadt->reset_value);
			}
			break;
		
		case GENERIC_ADDRESS_SPACE_SYSTEM_MEMORY:
			{
				debugf("ACPI reboot: system memory\n");
				memory::global_page_table_manager.map_memory((void*) fadt->reset_reg.address, (void*) fadt->reset_reg.address);
				uint8_t* addr = (uint8_t*) fadt->reset_reg.address;
				*addr = fadt->reset_value;
			}
			break;
		
		case GENERIC_ADDRESS_SPACE_PCI_CONFIGURATION_SPACE:
			{
				debugf("ACPI reboot: pci configuration space\n");
				pci::pci_writeb(0, (fadt->reset_reg.address >> 32) & 0xFFFF, (fadt->reset_reg.address >> 16) & 0xFFFF, fadt->reset_reg.address & 0xFFFF, fadt->reset_value);
			}
			break;

		default:
			abortf("ACPI reboot: unknown address space %d", fadt->reset_reg.address_space);
			break;
	}

	timer::global_timer->sleep(100);

	abortf("ACPI reboot failed");
}