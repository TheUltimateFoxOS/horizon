#include <apic/apic.h>

#include <utils/log.h>

using namespace apic;

void apic::ap_trampoline_c() {
	LAPIC_ID(id);
	debugf("Hello from cpu %d\n", id);

	while (true) {
		__asm__ __volatile__("hlt");
	}
}
