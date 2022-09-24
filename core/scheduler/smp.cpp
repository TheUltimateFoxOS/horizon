#include <scheduler/smp.h>

#include <apic/apic.h>
#include <acpi/madt.h>
#include <utils/log.h>
#include <memory/page_table_manager.h>
#include <memory/page_frame_allocator.h>
#include <boot/boot.h>
#include <gdt.h>
#include <interrupts/interrupts.h>
#include <timer/timer.h>

extern "C" void bootstrap();
extern "C" void bootstrap_data();

namespace smp {
	bootstrap_data_t* data;
}

using namespace smp;

void smp::ap_main() {
	data->status = ap_status::init_done;

	LAPIC_ID(id);
	debugf("Hello from cpu %d\n", id);

	apic::lapic_enable();
	apic::cpu_started[id] = true;

	while (true) {
		__asm__ __volatile__("hlt");
	}
}

void smp::startup() {
	LAPIC_ID(bspid);
	volatile uint8_t aprunning = 0;

	memory::global_page_table_manager.map_memory((void*) acpi::madt::lapic_base_addr, (void*) acpi::madt::lapic_base_addr);

	for (int i = 0; i < acpi::madt::lapic_count; i++) {
		debugf("going to start cpu %d\n", i);
	next:
		if(acpi::madt::lapic_ids[i] == bspid) {
			continue;
		}

		data = (bootstrap_data_t*) bootstrap_data;

		gdt_descriptor_t gdt_descriptor;
		gdt_descriptor.size = sizeof(gdt_t) - 1;
		gdt_descriptor.offset = (uint64_t) &default_gdt;

		data->status = 0;
		data->idt = (uint64_t) &interrupts::idtr;
		data->gdt = (uint64_t) &gdt_descriptor;
		data->entry = (uint64_t) &ap_main;

		__asm__ __volatile__ ("mov %%cr3, %%rax" : "=a"(data->pagetable));


		*boot::boot_info.smp[i].target_stack = (uint64_t) memory::global_allocator.request_page() + 4096;;
		*boot::boot_info.smp[i].goto_address = (uint64_t) &bootstrap;

		int timeout = 100;
		do {
			debugf("Waiting for cpu %d current status: %d!\n", i, data->status);
			timer::global_timer->sleep(10);
			if (--timeout == 0) {
				debugf("--- WARNING --- Timeout on cpu %d!\n", i);
				goto next;
			}
		} while (data->status != ap_status::init_done);

		debugf("cpu %d init done!\n", i);
	}
}
