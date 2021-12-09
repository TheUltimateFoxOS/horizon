#pragma once

#include <acpi/acpi_tables.h>
#include <stivale2.h>

namespace acpi {
	void* find_table_xsdt(sdt_header_t* sdt_header, char* signature, int idx);
	void* find_table_rsdt(sdt_header_t* sdt_header, char* signature, int idx);

	void* find_table(stivale2_struct* bootinfo, char* signature, int idx);
}