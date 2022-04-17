#include <boot/boot.h>

namespace boot {
	boot_info_t boot_info;
}

extern "C" void _start() {
	while (1) {
		__asm__ __volatile__("hlt");
	}
}