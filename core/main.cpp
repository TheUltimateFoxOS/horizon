#include <stivale2.h>
#include <gdt.h>

#include <interrupts/interrupts.h>

#include <output/stivale2_terminal.h>
#include <output/serial_port.h>

#include <renderer/renderer.h>
#include <renderer/render2d.h>
#include <renderer/font_renderer.h>

#include <utils/abort.h>
#include <utils/string.h>

#include <elf/elf_resolver.h>

#include <memory/memory.h>

#include <utils/log.h>

extern uint8_t logo[];

extern "C" void main() {

	output::stivale2_terminal terminal = output::stivale2_terminal();
	output::global_terminal = &terminal;
	log::stdout_device = output::global_terminal;

	output::serial_port serial_port = output::serial_port(COM1);
	output::global_serial_port = &serial_port;
	log::debug_device = output::global_serial_port;


	debugf("Hello, world!\n");
	printf("Hello, world!\n");

	setup_gdt();
	memory::prepare_memory(global_bootinfo);
	interrupts::prepare_interrupts();
	elf::setup(global_bootinfo);
	renderer::setup(global_bootinfo);

	renderer::global_font_renderer->clear(0);
	renderer::global_renderer_2d->load_bitmap(logo, 0);

	printf("Welcome to FoxOS Horizon!\n");

	while(1) {
		__asm__ __volatile__("hlt");
	}
}