#include <apic/apic.h>

#include <acpi/madt.h>

using namespace apic;

void apic::lapic_write(uint64_t reg, uint32_t value) {
	*((volatile uint32_t*)(acpi::madt::lapic_base_addr + reg)) = value;
}

uint32_t apic::lapic_read(uint64_t reg) {
	return *((volatile uint32_t*)(acpi::madt::lapic_base_addr + reg));
}

void apic::lapic_wait() {
	do {
		__asm__ __volatile__ ("pause" : : : "memory");
	} while(*((volatile uint32_t*)(acpi::madt::lapic_base_addr + 0x300)) & (1 << 12));
}

void apic::lapic_eoi() {
	lapic_write(0xB0, 0);
}

void apic::lapic_enable() {
	lapic_write(0xF0, lapic_read(0xF0) | 0x1ff);
}

void apic::lapic_ipi(uint8_t lapic_id, uint8_t vector) {
	lapic_write(0x310, lapic_id << 24);
	lapic_write(0x300, vector);
}