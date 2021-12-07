#include <gdt.h>

#include <utils/log.h>

__attribute__((aligned(0x1000)))
gdt_t default_gdt = {
	{0, 0, 0, 0x00, 0x00, 0},
	{0, 0, 0, 0x9a, 0xa0, 0},
	{0, 0, 0, 0x92, 0xa0, 0},
	{0, 0, 0, 0x00, 0x00, 0},
	{0, 0, 0, 0x9a, 0xa0, 0},
	{0, 0, 0, 0x92, 0xa0, 0}
};

void setup_gdt() {
	gdt_descriptor_t gdt_descriptor;
	gdt_descriptor.size = sizeof(gdt_t) - 1;
	gdt_descriptor.offset = (uint64_t) &default_gdt;

	debugf("Loading gdt at 0x%x!\n", &default_gdt);

	load_gdt(&gdt_descriptor);
}