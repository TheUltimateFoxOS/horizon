#include <apic/apic.h>
#include <acpi/madt.h>

#include <memory/page_table_manager.h>
#include <memory/page_frame_allocator.h>
#include <utils/string.h>
#include <utils/log.h>
#include <utils/assert.h>
#include <gdt.h>
#include <interrupts/interrupts.h>

using namespace apic;

namespace apic {
	bool cpu_started[256] = { false };
	int bsp_id = 0;
}

void apic::setup() {
	LAPIC_ID(bspid);
	bsp_id = bspid;
	cpu_started[bsp_id] = true;
}

void apic::smp_spinup() {
	memory::global_page_table_manager.map_memory((void*) acpi::madt::lapic_base_addr, (void*) acpi::madt::lapic_base_addr);
	debugf("Mapped LAPIC memory at %p\n", (void*) acpi::madt::lapic_base_addr);

	void* gdt_cpy = memory::global_allocator.request_page();
	memcpy(gdt_cpy, (void*) &default_gdt, sizeof(default_gdt));
	assert((uint64_t) gdt_cpy < 0x100000000);

	gdt_descriptor_t* gdt_descriptor = (gdt_descriptor_t*) memory::global_allocator.request_page();
	assert((uint64_t) gdt_descriptor < 0x100000000);
	gdt_descriptor->size = sizeof(gdt_t) - 1;
	gdt_descriptor->offset = (uint64_t) gdt_cpy;

	memory::global_page_table_manager.map_memory((void*) SMP_TRAMPOLINE_ADDR, (void*) SMP_TRAMPOLINE_ADDR);
	memcpy((void*) SMP_TRAMPOLINE_ADDR, (void*) &ap_trampoline, 4096);

	trampoline_data* data = (trampoline_data*) (((uint64_t) &ap_trampoline_data - (uint64_t) &ap_trampoline) + SMP_TRAMPOLINE_ADDR);

	debugf("Trampoline data: 0x%x\n", data);
	debugf("Trampoline: 0x%x\n", ((uint64_t) &ap_trampoline - (uint64_t) &ap_trampoline) + SMP_TRAMPOLINE_ADDR);
	debugf("Trampoline 64: 0x%x\n", ((uint64_t) &ap_trampoline_64 - (uint64_t) &ap_trampoline) + SMP_TRAMPOLINE_ADDR);

	for (int i = 0; i < acpi::madt::lapic_count; i++) {
		if(acpi::madt::lapic_ids[i] == bsp_id) {
			debugf("Skiping BSP at %d\n", acpi::madt::lapic_ids[i]);
			continue;
		}

		debugf("Setting up trampoline for AP %d\n", acpi::madt::lapic_ids[i]);

		data->status = 0;
		data->idt = (uint64_t) &interrupts::idtr;
		data->gdt = (uint64_t) gdt_descriptor;
		data->stack_ptr = (uint64_t) memory::global_allocator.request_page() + 4096;
		data->entry = (uint64_t) &ap_trampoline_c;

		__asm__ __volatile__ ("mov %%cr3, %%rax" : "=a"(data->pagetable));

		lapic_write(0x280, 0);
		lapic_write(0x310, (lapic_read(0x310) & 0x00ffffff) | (i << 24));
		lapic_write(0x300, (lapic_read(0x300) & 0xfff00000) | 0x00C500);

		lapic_wait();

		lapic_write(0x280, 0);
		lapic_write(0x310, (lapic_read(0x310) & 0x00ffffff) | (i << 24));
		lapic_write(0x300, (lapic_read(0x300) & 0xfff00000) | 0x008500);

		lapic_wait();

		for (int j = 0; j < 2; j++) {

			lapic_write(0x280, 0);
			lapic_write(0x310, (lapic_read(0x310) & 0x00ffffff) | (i << 24));
			lapic_write(0x300, (lapic_read(0x300) & 0xfff0f800) | 0x000600 | SMP_TRAMPOLINE_PAGE);

			lapic_wait();
		}

		do {
			debugf("Waiting for cpu %d current status: %d!\n", i, data->status);
			__asm__ __volatile__ ("pause");

		} while (data->status != ap_status::incpp);

		debugf("CPU %d spinup complete!\n", i);
	}
}