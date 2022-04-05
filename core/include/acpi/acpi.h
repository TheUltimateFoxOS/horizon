#pragma once

#include <acpi/acpi_tables.h>
#include <stivale2.h>

#include <interrupts/interrupt_handler.h>
#include <interrupts/interrupts.h>

namespace acpi {
	#define IS_CANONICAL(addr) (((addr <= 0x00007FFFFFFFFFFF) || ((addr >= 0xFFFF800000000000) && (addr <= 0xFFFFFFFFFFFFFFFF))))

	void* find_table_xsdt(sdt_header_t* sdt_header, char* signature, int idx);
	void* find_table_rsdt(sdt_header_t* sdt_header, char* signature, int idx);

	void* find_table(stivale2_struct* bootinfo, char* signature, int idx);

	void init();
	void dsdt_init();
	void shutdown();
	void reboot();
}