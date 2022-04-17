#pragma once

#include <stdint.h>

namespace apic {
	struct trampoline_data {
		uint8_t status;
		uint64_t pagetable;
		uint64_t idt;
		uint64_t gdt;
		uint64_t stack_ptr;
		uint64_t entry;
	} __attribute__ ((packed));

	enum ap_status {
		incpp = 1
	};

	extern bool cpu_started[256];
	extern int bsp_id;

	extern "C" void ap_trampoline();
	extern "C" void ap_trampoline_64();
	extern "C" void ap_trampoline_data();

	void lapic_write(uint64_t reg, uint32_t value);
	uint32_t lapic_read(uint64_t reg);
	void lapic_wait();
	void lapic_eoi();
	void lapic_enable();
	void lapic_ipi(uint8_t lapic_id, uint8_t vector);

	void ap_trampoline_c();

	void smp_spinup();
	void setup();

	#define LAPIC_ID(vaiable_name) \
		volatile unsigned char vaiable_name = 0; \
		{\
			__asm__ __volatile__ ("mov $1, %%eax; cpuid; shrl $24, %%ebx;": "=b"(vaiable_name) : : ); \
		}
}