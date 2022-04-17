#pragma once

#include <acpi/acpi_tables.h>

namespace acpi {
	namespace madt {
		void parse_madt();

		extern uint8_t lapic_ids[256];
		extern uint8_t lapic_count;
		extern uint64_t lapic_base_addr;

		extern uint8_t ioapic_ids[256];
		extern uint8_t ioapic_count;
		extern uint64_t ioapic_base_addr[256];
	}
}