#include <acpi/madt.h>

#include <acpi/acpi.h>

#include <utils/log.h>

using namespace acpi;
using namespace acpi::madt;

namespace acpi {
	namespace madt {
		uint8_t lapic_ids[256] = {0};
		uint8_t lapic_count = 0;
		uint64_t lapic_base_addr = 0;

		uint8_t ioapic_ids[256] = {0};
		uint8_t ioapic_count = 0;
		uint64_t ioapic_base_addr[256] = {0};
	}
}

void acpi::madt::parse_madt(stivale2_struct* bootinfo) {
	madt_header_t* madt = (madt_header_t*) find_table(bootinfo, "APIC", 0);

	lapic_base_addr = madt->local_apic_address;

	int curr_count = 0;
	while(curr_count < madt->header.length - sizeof(madt_header_t)) {
		madt_record_t* record = (madt_record_t*) ((uint64_t) madt + sizeof(madt_header_t) + curr_count);
		curr_count += record->length;

		switch (record->type) {
			case 0: // Processor local apic
				{
					madt_local_processor_t* processor = (madt_local_processor_t*) record;
					lapic_ids[lapic_count++] = processor->apic_id;
				}
				break;
			
			case 1: // I/O apic
				{
					madt_io_apic_t* ioapic = (madt_io_apic_t*) record;
					ioapic_ids[ioapic_count++] = ioapic->io_apic_id;
					ioapic_base_addr[ioapic_count] = ioapic->io_apic_address;
				}
				break;
			
			default:
				{
					debugf("WARNING: Unknown MADT record type %d\n", record->type);
				}
				break;
		}
	}

	debugf("Found %d local APICs\n", lapic_count);
	debugf("Found %d I/O APICs\n", ioapic_count);
}