#include <timer/hpet.h>

#include <acpi/acpi.h>
#include <acpi/acpi_tables.h>
#include <memory/page_table_manager.h>
#include <utils/log.h>

using namespace timer;

namespace timer {
	hpet_timer* global_hpet_timer;
}

hpet_timer::hpet_timer() {
	acpi::hpet_table_t* hpet_table = (acpi::hpet_table_t*) acpi::find_table(global_bootinfo, (char*) "HPET", 0);
	this->hpet = (hpet_t*) hpet_table->address;
	debugf("HPET at 0x%x\n", hpet);

	memory::global_page_table_manager.map_memory(hpet, hpet);
	hpet->general_config = 1;
}

void hpet_timer::sleep(uint32_t ms) {
	ms /= 8;
	uint64_t ticks = hpet->counter_value + (ms * 10000000000000) / ((hpet->capabilities >> 32) & 0xffffffff);

	while (hpet->counter_value < ticks) {
		__asm__ __volatile__ ("pause" ::: "memory");
	}
}

bool hpet_timer::is_available() {
	acpi::hpet_table_t* hpet_table = (acpi::hpet_table_t*) acpi::find_table(global_bootinfo, (char*) "HPET", 0);
	if (hpet_table == nullptr) {
		return false;
	}

	hpet_t* hpet = (hpet_t*) hpet_table->address;

	return hpet != nullptr;
}