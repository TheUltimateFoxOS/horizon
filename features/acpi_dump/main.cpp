#include <elf/kernel_module.h>

#include <utils/log.h>
#include <utils/assert.h>
#include <acpi/acpi.h>
#include <acpi/acpi_tables.h>
#include <boot/boot.h>
#include <acpi_vfs.h>

acpi::sdt_header_t* headers[256] = {0};
int num_headers = 0;

void register_sdt(acpi::sdt_header_t* header) {
	debugf("Registering SDT %c%c%c%c\n", header->signature[0], header->signature[1], header->signature[2], header->signature[3]);
	headers[num_headers] = header;
	num_headers++;
}

void enumerate_rsdt(acpi::sdt_header_t* sdt_header) {
	int entries = (sdt_header->length - sizeof(acpi::sdt_header_t)) / 4;

	for(int t = 0; t < entries; t++) {
		acpi::sdt_header_t* new_header = (acpi::sdt_header_t*) (uint64_t) * (uint32_t*) ((uint64_t) sdt_header + sizeof(acpi::sdt_header_t) + (t * 4));
		register_sdt(new_header);
	}
}

void enumerate_xsdt(acpi::sdt_header_t* sdt_header) {
	int entries = (sdt_header->length - sizeof(acpi::sdt_header_t)) / 8;

	for(int t = 0; t < entries; t++) {
		acpi::sdt_header_t* new_header = (acpi::sdt_header_t*) * (uint64_t*) ((uint64_t) sdt_header + sizeof(acpi::sdt_header_t) + (t * 8));
		register_sdt(new_header);
	}
}

void init() {
	if (boot::boot_info.rsdp->xsdt_address != 0) {
		acpi::sdt_header_t* xsdt = (acpi::sdt_header_t*) (((acpi::rsdp2_t*) boot::boot_info.rsdp)->xsdt_address);
		enumerate_xsdt(xsdt);
	} else {
		acpi::sdt_header_t* rsdt = (acpi::sdt_header_t*) (uint64_t) (((acpi::rsdp2_t*) boot::boot_info.rsdp)->rsdt_address);
		enumerate_rsdt(rsdt);
	}

	acpi::fadt_table_t* fadt = (acpi::fadt_table_t*) acpi::find_table((char*) "FACP", 0);
	if (fadt != nullptr) {
		void* dsdt_addr = (void*) (IS_CANONICAL(fadt->X_dsdt) ? fadt->X_dsdt : fadt->dsdt);
		if (dsdt_addr != nullptr) {
			register_sdt((acpi::sdt_header_t*) dsdt_addr);
		}
	}

	debugf("Found %d SDTs\n", num_headers);

	fs::acpi_vfs* acpi_vfs = new fs::acpi_vfs(headers, num_headers);
	fs::global_vfs->register_mount((char*) "acpi", acpi_vfs);

	debugf("use 'safm acpi: acpi.saf' to dump them\n");
}

define_module("acpi_dump", init, null_ptr_func, null_ptr_func);