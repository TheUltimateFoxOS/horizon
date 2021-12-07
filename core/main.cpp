#include <stivale2.h>

#include <output/stivale2_terminal.h>
#include <output/serial_port.h>

#include <gdt.h>

#include <utils/log.h>

extern "C" void main() {

	output::stivale2_terminal terminal = output::stivale2_terminal();
	output::global_terminal = &terminal;

	output::serial_port serial_port = output::serial_port(COM1);
	output::global_serial_port = &serial_port;

	debugf("Hello, world!\n");
	printf("Hello, world!\n");

	setup_gdt();

	while(1) {
		__asm__ __volatile__("hlt");
	}
}