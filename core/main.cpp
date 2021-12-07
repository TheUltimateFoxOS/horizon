#include <stivale2.h>
#include <output/stivale2_terminal.h>
#include <utils/string.h>

extern "C" void main() {

	output::stivale2_terminal terminal = output::stivale2_terminal();
	output::global_terminal = &terminal;

	debugf("Hello, world!\n");
	printf("Hello, world!\n");

	while(1) {
		__asm__ __volatile__("hlt");
	}
}